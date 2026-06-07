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

namespace meta::qt
{

template <> struct WidgetRenderer<int>
{
  static MetaWidget *render(Attribute<int> &attr, QWidget *parent)
  {
    const std::string widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    const int         min = meta::common::min(attr);
    const int         max = meta::common::max(attr);
    const auto        items = meta::common::enum_items<int>(attr);

    int &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (!label_txt.empty())
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);
    }

    if (widget_type == "EnumComboBox")
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

      widget->connect(combo,
                      QOverload<int>::of(&QComboBox::currentIndexChanged),
                      widget,
                      [&value, widget, combo](int)
                      {
                        value = combo->currentData().toInt();
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

      value = std::clamp(value, min, max);

      QWidget *control = nullptr;

      if (widget_type == "Slider")
      {
        auto *slider = new QSlider(Qt::Horizontal, widget);
        slider->setRange(min, max);
        slider->setValue(value);

        QObject::connect(slider,
                         &QSlider::valueChanged,
                         widget,
                         [&value, widget, min, max](int v)
                         {
                           value = std::clamp(v, min, max);
                           Q_EMIT widget->value_changed();
                         });

        control = slider;
      }
      else if (widget_type == "ScrollBar")
      {
        auto *scrollbar = new QScrollBar(Qt::Horizontal, widget);
        scrollbar->setRange(min, max);
        scrollbar->setValue(value);

        QObject::connect(scrollbar,
                         &QScrollBar::valueChanged,
                         widget,
                         [&value, widget, min, max](int v)
                         {
                           value = std::clamp(v, min, max);
                           Q_EMIT widget->value_changed();
                         });

        control = scrollbar;
      }
      else if (widget_type == "Dial")
      {
        auto *dial = new QDial(widget);
        dial->setRange(min, max);
        dial->setValue(value);

        QObject::connect(dial,
                         &QDial::valueChanged,
                         widget,
                         [&value, widget, min, max](int v)
                         {
                           value = std::clamp(v, min, max);
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
