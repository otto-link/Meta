/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <random>

#include "meta_qt/widgets/range_bar.hpp"
#include "meta_qt/widgets/xy_canvas.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<glm::vec2>
{
  static MetaWidget *render(Attribute<glm::vec2> &attr, QWidget *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    const float       min = meta::common::min<float>(attr);
    const float       max = meta::common::max<float>(attr);
    const float       step = meta::common::step<float>(attr);
    const bool        show_grid = meta::common::try_get<bool>(attr,
                                                       "ui.show_grid",
                                                       true);

    const int decimals = meta::common::try_get_format_decimals(format);

    glm::vec2 &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (widget_type.empty()) widget_type = "Input";

    if (widget_type == "Input") // --- Input
    {
      if (!label_txt.empty())
      {
        layout->addWidget(
            new QLabel(QString::fromStdString(label_txt), widget));
      }

      auto *row = new QHBoxLayout();

      auto *spinbox_x = new QDoubleSpinBox(widget);
      auto *spinbox_y = new QDoubleSpinBox(widget);

      for (auto *sp : {spinbox_x, spinbox_y})
      {
        sp->setRange(min, max);
        sp->setSingleStep(step);
        sp->setDecimals(decimals);
      }

      spinbox_x->setValue(std::clamp(value.x, min, max));
      spinbox_y->setValue(std::clamp(value.y, min, max));

      row->addWidget(spinbox_x);
      row->addWidget(spinbox_y);

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
    }
    else if (widget_type == "XYCanvas") // --- XYCanvas
    {
      auto *canvas = new XYCanvas(value, min, max, min, max, show_grid, widget);
      layout->addWidget(canvas);

      // Button row
      auto *btn_row = new QHBoxLayout();
      auto *center_btn = new QPushButton(QObject::tr("Center"), widget);
      auto *random_btn = new QPushButton(QObject::tr("Random"), widget);

      for (auto *btn : {center_btn, random_btn})
      {
        btn->setFixedHeight(22);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn_row->addWidget(btn);
      }
      layout->addLayout(btn_row);

      // Fires on every drag step — edit_started + value_changed only.
      QObject::connect(canvas,
                       &XYCanvas::value_changed,
                       widget,
                       [widget](glm::vec2)
                       {
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      // Fires once on mouse release — edit_ended.
      QObject::connect(canvas,
                       &XYCanvas::drag_ended,
                       widget,
                       [widget](glm::vec2) { Q_EMIT widget->edit_ended(); });

      // Center
      QObject::connect(center_btn,
                       &QPushButton::clicked,
                       widget,
                       [&value, min, max, canvas, widget]()
                       {
                         const glm::vec2 center = {(min + max) * 0.5f,
                                                   (min + max) * 0.5f};
                         value = center;
                         canvas->set_value(center);
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });

      // Random
      QObject::connect(random_btn,
                       &QPushButton::clicked,
                       widget,
                       [&value, min, max, canvas, widget]()
                       {
                         static std::mt19937 rng{std::random_device{}()};
                         std::uniform_real_distribution<float> dist_x(min, max);
                         std::uniform_real_distribution<float> dist_y(min, max);
                         const glm::vec2 rv = {dist_x(rng), dist_y(rng)};
                         value = rv;
                         canvas->set_value(rv);
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });
    }
    else if (widget_type == "RangeBar") // --- RangeBar
    {
      auto *bar = new RangeBar(value, min, max, decimals, widget);
      layout->addWidget(bar);

      // Button row
      auto *btn_row = new QHBoxLayout();
      auto *reset_btn = new QPushButton(QObject::tr("Full"), widget);
      auto *center_btn = new QPushButton(QObject::tr("Center"), widget);
      auto *unit_btn = new QPushButton(QObject::tr("[0, 1]"), widget);

      for (auto *btn : {reset_btn, center_btn, unit_btn})
      {
        btn->setFixedHeight(22);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn_row->addWidget(btn);
      }
      layout->addLayout(btn_row);

      // Live drag → edit_started + value_changed
      QObject::connect(bar,
                       &RangeBar::value_changed,
                       widget,
                       [widget](glm::vec2)
                       {
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      // Release → edit_ended
      QObject::connect(bar,
                       &RangeBar::drag_ended,
                       widget,
                       [widget](glm::vec2) { Q_EMIT widget->edit_ended(); });

      // Reset — full domain
      QObject::connect(reset_btn,
                       &QPushButton::clicked,
                       widget,
                       [&value, min, max, bar, widget]()
                       {
                         bar->set_value({min, max});
                         value = {min, max};
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });

      // Center — shift to middle of domain, preserve span
      QObject::connect(
          center_btn,
          &QPushButton::clicked,
          widget,
          [&value, min, max, bar, widget]()
          {
            const float span = value.y - value.x;
            const float mid = 0.f; // (min + max) * 0.5f;
            const float lo = std::clamp(mid - span * 0.5f, min, max - span);
            bar->set_value({lo, lo + span});
            value = {lo, lo + span};
            Q_EMIT widget->edit_started();
            Q_EMIT widget->value_changed();
            Q_EMIT widget->edit_ended();
          });

      // Unit — clamp [0, 1] to domain
      QObject::connect(unit_btn,
                       &QPushButton::clicked,
                       widget,
                       [&value, min, max, bar, widget]()
                       {
                         const float lo = std::clamp(0.f, min, max);
                         const float hi = std::clamp(1.f, min, max);
                         bar->set_value({lo, hi});
                         value = {lo, hi};
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
