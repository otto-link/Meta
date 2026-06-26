/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

namespace meta::qt
{

template <> struct WidgetRenderer<glm::vec3>
{
  static MetaWidget *render(Attribute<glm::vec3> &attr, QWidget *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    const float       min = meta::common::min<float>(attr);
    const float       max = meta::common::max<float>(attr);
    const float       step = meta::common::step<float>(attr);

    const int decimals = meta::common::try_get_format_decimals(format);

    glm::vec3 &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (widget_type.empty()) widget_type = "Input";

    if (widget_type == "Input")
    {
      if (!label_txt.empty())
      {
        layout->addWidget(
            new QLabel(QString::fromStdString(label_txt), widget));
      }

      auto *row = new QHBoxLayout();

      auto *spinbox_x = new QDoubleSpinBox(widget);
      auto *spinbox_y = new QDoubleSpinBox(widget);
      auto *spinbox_z = new QDoubleSpinBox(widget);

      for (auto *sp : {spinbox_x, spinbox_y})
      {
        sp->setRange(min, max);
        sp->setSingleStep(step);
        sp->setDecimals(decimals);
      }

      spinbox_x->setValue(std::clamp(value.x, min, max));
      spinbox_y->setValue(std::clamp(value.y, min, max));
      spinbox_z->setValue(std::clamp(value.z, min, max));

      row->addWidget(spinbox_x);
      row->addWidget(spinbox_y);
      row->addWidget(spinbox_z);

      layout->addLayout(row);

      QObject::connect(spinbox_x,
                       qOverload<double>(&QDoubleSpinBox::valueChanged),
                       widget,
                       [&value, widget, spinbox_x, min, max](double v)
                       {
                         float x = std::clamp(static_cast<float>(v), min, max);

                         {
                           QSignalBlocker blocker(spinbox_x);
                           spinbox_x->setValue(x);
                         }

                         value.x = x;

                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });

      QObject::connect(spinbox_y,
                       qOverload<double>(&QDoubleSpinBox::valueChanged),
                       widget,
                       [&value, widget, spinbox_y, min, max](double v)
                       {
                         float y = std::clamp(static_cast<float>(v), min, max);

                         {
                           QSignalBlocker blocker(spinbox_y);
                           spinbox_y->setValue(y);
                         }

                         value.y = y;

                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });

      QObject::connect(spinbox_z,
                       qOverload<double>(&QDoubleSpinBox::valueChanged),
                       widget,
                       [&value, widget, spinbox_z, min, max](double v)
                       {
                         float z = std::clamp(static_cast<float>(v), min, max);

                         {
                           QSignalBlocker blocker(spinbox_z);
                           spinbox_z->setValue(z);
                         }

                         value.z = z;

                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
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
