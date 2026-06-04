/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <format>

#include <QDoubleSpinBox>
#include <QLabel>
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

    MetaWidget *widget = make_meta_widget_hbox(parent);
    auto       *layout = widget->layout();

    if (widget_type == "Input")
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);

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
    else
    {
      const std::string msg = "Unsupported widget type: " + label_txt + "/" +
                              widget_type;
      QLabel *label = new QLabel(msg.c_str(), widget);
      layout->addWidget(label);
    }

    return widget;
  }
};

} // namespace meta::qt
