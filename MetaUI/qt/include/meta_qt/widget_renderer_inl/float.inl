/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <format>

#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QScrollBar>
#include <QSlider>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<float>
{
  static MetaWidget *render(Attribute<float> &attr, QWidget *parent)
  {
    const std::string widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    const float       min = meta::common::min(attr);
    const float       max = meta::common::max(attr);
    const float       step = meta::common::step(attr);
    float            &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (!label_txt.empty())
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);
    }

    if (widget_type == "Input")
    {
      // --- INPUT

      auto *spinbox = new QDoubleSpinBox(widget);

      spinbox->setMinimum(min);
      spinbox->setMaximum(max);
      spinbox->setSingleStep(step);
      spinbox->setValue(std::clamp(value, min, max));
      spinbox->setDecimals(meta::common::try_get_format_decimals(format));

      layout->addWidget(spinbox);

      QObject::connect(spinbox,
                       &QDoubleSpinBox::valueChanged,
                       spinbox,
                       [&value, widget, min, max](double v)
                       {
                         value = std::clamp(static_cast<float>(v), min, max);
                         Q_EMIT widget->value_changed();
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

      constexpr int range_min = 0;
      constexpr int range_max = 1000;

      auto to_int = [min, max](float v) -> int
      { return static_cast<int>(((v - min) / (max - min)) * range_max); };

      auto from_int = [min, max](int v) -> float
      { return min + (static_cast<float>(v) / range_max) * (max - min); };

      value = std::clamp(value, min, max);

      QWidget *control = nullptr;

      if (widget_type == "Slider")
      {
        auto *slider = new QSlider(Qt::Horizontal, widget);
        slider->setRange(range_min, range_max);
        slider->setValue(to_int(value));

        QObject::connect(slider,
                         &QSlider::valueChanged,
                         widget,
                         [&value, widget, from_int, min, max](int v)
                         {
                           value = std::clamp(from_int(v), min, max);
                           Q_EMIT widget->value_changed();
                         });

        control = slider;
      }
      else if (widget_type == "ScrollBar")
      {
        auto *scrollbar = new QScrollBar(Qt::Horizontal, widget);
        scrollbar->setRange(range_min, range_max);
        scrollbar->setValue(to_int(value));

        QObject::connect(scrollbar,
                         &QScrollBar::valueChanged,
                         widget,
                         [&value, widget, from_int, min, max](int v)
                         {
                           value = std::clamp(from_int(v), min, max);
                           Q_EMIT widget->value_changed();
                         });

        control = scrollbar;
      }
      else if (widget_type == "Dial")
      {
        auto *dial = new QDial(widget);
        dial->setRange(range_min, range_max);
        dial->setValue(to_int(value));

        QObject::connect(dial,
                         &QDial::valueChanged,
                         widget,
                         [&value, widget, from_int, min, max](int v)
                         {
                           value = std::clamp(from_int(v), min, max);
                           Q_EMIT widget->value_changed();
                         });

        control = dial;
      }

      layout->addWidget(control);
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
