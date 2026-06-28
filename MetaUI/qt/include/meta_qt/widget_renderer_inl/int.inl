/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <format>

#include <QComboBox>
#include <QLabel>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widgets/slider_int.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<int>
{
  static MetaWidget *render(Attribute<int> &attr, QWidget *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    const int         min = meta::common::min(attr);
    const int         max = meta::common::max(attr);
    const int         step = meta::common::step(attr);
    const auto        items = meta::common::enum_items<int>(attr);
    const bool        plus_minus = meta::common::try_get<bool>(attr,
                                                        "ui.plus_minus",
                                                        false);

    int &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (!label_txt.empty())
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);
    }

    if (widget_type.empty()) widget_type = "Input";

    if (widget_type == "Input")
    {
      // --- INPUT

      auto *spinbox = new QDoubleSpinBox(widget);

      spinbox->setMinimum(min);
      spinbox->setMaximum(max);
      spinbox->setSingleStep(step);
      spinbox->setValue(std::clamp(value, min, max));
      spinbox->setDecimals(0);

      layout->addWidget(spinbox);

      QObject::connect(spinbox,
                       &QDoubleSpinBox::valueChanged,
                       spinbox,
                       [&value, widget, min, max](int v)
                       {
                         value = std::clamp(v, min, max);
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });
    }
    else if (widget_type == "EnumComboBox")
    {
      // --- ENUM COMBO BOX

      auto *combo = new QComboBox(widget);
      layout->addWidget(combo);

      int current_index = 0;
      int index = 0;

      for (const auto &[val, name] : items)
      {
        combo->addItem(QString::fromStdString(name), QVariant::fromValue(val));

        if (val == value) current_index = index;

        ++index;
      }

      combo->setCurrentIndex(current_index);

      QObject::connect(combo,
                       QOverload<int>::of(&QComboBox::currentIndexChanged),
                       widget,
                       [&value, widget, combo](int)
                       {
                         value = combo->currentData().toInt();
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });
    }
    else if (widget_type == "Slider" || widget_type == "ScrollBar" ||
             widget_type == "Dial")
    {
      if (!attr.metadata().contains_all_keys(
              {meta::keys::constraints::min, meta::keys::constraints::max}))
      {
        layout->addWidget(make_error_widget(&attr, "missing metadata", widget));
        return widget;
      }

      value = std::clamp(value, min, max);

      QAbstractSlider *control = nullptr;

      if (widget_type == "Slider")
      {
        auto *slider = new QSlider(Qt::Horizontal, widget);
        slider->setRange(min, max);
        slider->setValue(value);
        control = slider;
      }
      else if (widget_type == "ScrollBar")
      {
        auto *scrollbar = new QScrollBar(Qt::Horizontal, widget);
        scrollbar->setRange(min, max);
        scrollbar->setValue(value);
        control = scrollbar;
      }
      else if (widget_type == "Dial")
      {
        auto *dial = new QDial(widget);
        dial->setRange(min, max);
        dial->setValue(value);
        control = dial;
      }

      QObject::connect(control,
                       &QAbstractSlider::sliderPressed,
                       widget,
                       [widget]() { Q_EMIT widget->edit_started(); });

      QObject::connect(control,
                       &QAbstractSlider::valueChanged,
                       widget,
                       [&value, widget, min, max](int v)
                       {
                         value = std::clamp(v, min, max);
                         Q_EMIT widget->value_changed();
                       });

      QObject::connect(control,
                       &QAbstractSlider::sliderReleased,
                       widget,
                       [widget]() { Q_EMIT widget->edit_ended(); });

      layout->addWidget(control);
    }
    else if (widget_type == "SliderInt") // SliderInt
    {
      auto *slider = new SliderInt(label_txt,
                                   /*value_init=*/value,
                                   min,
                                   max,
                                   plus_minus,
                                   format,
                                   widget);
      slider->set_value(value);
      layout->addWidget(slider);

      QObject::connect(slider,
                       &SliderInt::value_changed,
                       widget,
                       [&value, slider, widget]()
                       {
                         value = slider->get_value();
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      QObject::connect(slider,
                       &SliderInt::edit_ended,
                       widget,
                       [&value, slider, widget]()
                       {
                         value = slider->get_value();
                         Q_EMIT widget->edit_ended();
                       });
    }
    else
    {
      layout->addWidget(
          make_error_widget(&attr, "unsupported widget type", widget));
    }

    return widget;
  }
};

} // namespace meta::qt
