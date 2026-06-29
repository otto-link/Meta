/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <random>

#include <QFormLayout>

#include "meta_qt/widgets/range_bar.hpp"
#include "meta_qt/widgets/vector_canvas.hpp"
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
    const bool        locked_xy = meta::common::try_get<bool>(attr,
                                                       "ui.locked_xy",
                                                       false);

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
      // Guard value sentinel: {-1, 0} means "disabled / no range".
      const bool initially_active = !(value.x == -1.f && value.y == 0.f);

      // Restore the last meaningful range when toggling back on.
      // Seeded from the current value if it's valid, otherwise full domain.
      glm::vec2 last_active_value = initially_active ? value
                                                     : glm::vec2{min, max};

      auto *bar = new RangeBar(value, min, max, decimals, widget);
      auto *btn_row = new QHBoxLayout();
      auto *toggle_btn = new QPushButton(widget);
      auto *reset_btn = new QPushButton(QObject::tr("Full"), widget);
      auto *center_btn = new QPushButton(QObject::tr("Center"), widget);
      auto *unit_btn = new QPushButton(QObject::tr("[0, 1]"), widget);

      toggle_btn->setCheckable(true);
      toggle_btn->setChecked(initially_active);
      toggle_btn->setText(initially_active ? QObject::tr("On")
                                           : QObject::tr("Off"));
      toggle_btn->setFixedHeight(22);
      toggle_btn->setFixedWidth(40);

      for (auto *btn : {reset_btn, center_btn, unit_btn})
      {
        btn->setFixedHeight(22);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      }

      btn_row->addWidget(toggle_btn);
      btn_row->addSpacing(4);
      btn_row->addWidget(reset_btn);
      btn_row->addWidget(center_btn);
      btn_row->addWidget(unit_btn);

      layout->addWidget(bar);
      layout->addLayout(btn_row);

      // Helper: enable or disable all range controls at once.
      auto set_active = [bar, reset_btn, center_btn, unit_btn](bool active)
      {
        bar->setEnabled(active);
        reset_btn->setEnabled(active);
        center_btn->setEnabled(active);
        unit_btn->setEnabled(active);
      };

      set_active(initially_active);

      // Toggle
      QObject::connect(toggle_btn,
                       &QPushButton::toggled,
                       widget,
                       [&value,
                        bar,
                        toggle_btn,
                        set_active,
                        widget,
                        // mutable copy so each lambda invocation can update it
                        lav = last_active_value](bool active) mutable
                       {
                         toggle_btn->setText(active ? QObject::tr("On")
                                                    : QObject::tr("Off"));
                         set_active(active);

                         if (active)
                         {
                           // Restore last known good range.
                           value = lav;
                           bar->set_value(lav);
                         }
                         else
                         {
                           // Save current range before clobbering it.
                           lav = value;
                           value = {-1.f, 0.f};
                           bar->set_value({-1.f, 0.f});
                         }

                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });

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

      // Full domain
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

      // Center — shift span to domain midpoint
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

      // Unit — [0, 1] clamped to domain
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
    else if (widget_type == "VectorEditor") // --- VectorEditor
    {
      if (!label_txt.empty())
        layout->addWidget(
            new QLabel(QString::fromStdString(label_txt), widget));

      // --- Canvas ----------------------------------------------------------
      auto *canvas = new VectorCanvas(value, max, locked_xy, widget);

      // Centre the fixed-size canvas horizontally.
      auto *canvas_row = new QHBoxLayout();
      canvas_row->addStretch();
      canvas_row->addWidget(canvas);
      canvas_row->addStretch();
      layout->addLayout(canvas_row);

      // --- Controls --------------------------------------------------------
      auto *form = new QFormLayout();
      form->setContentsMargins(0, 2, 0, 2);
      form->setSpacing(3);

      // Magnitude spinbox
      auto *mag_spin = new QDoubleSpinBox(widget);
      mag_spin->setRange(0.0, double(max));
      mag_spin->setDecimals(decimals);
      mag_spin->setSingleStep(double(max) / 100.0);
      mag_spin->setValue(double(canvas->magnitude()));
      mag_spin->setFixedHeight(22);

      // Angle spinbox (disabled when locked)
      auto *angle_spin = new QDoubleSpinBox(widget);
      angle_spin->setRange(-360.0, 360.0);
      angle_spin->setDecimals(1);
      angle_spin->setSingleStep(1.0);
      angle_spin->setSuffix("°");
      angle_spin->setValue(double(canvas->angle_deg()));
      angle_spin->setEnabled(!locked_xy);
      angle_spin->setFixedHeight(22);

      form->addRow(QObject::tr("Magnitude"), mag_spin);
      form->addRow(QObject::tr("Angle"), angle_spin);
      layout->addLayout(form);

      // Lock toggle
      auto *lock_row = new QHBoxLayout();
      auto *lock_cb = new QCheckBox(QObject::tr("Isotropic  (kx = ky)"),
                                    widget);
      lock_cb->setChecked(locked_xy);
      lock_row->addStretch();
      lock_row->addWidget(lock_cb);
      layout->addLayout(lock_row);

      // --- Sync helpers ----------------------------------------------------

      // Canvas → spinboxes (keep in sync after drag)
      QObject::connect(canvas,
                       &VectorCanvas::magnitude_changed,
                       widget,
                       [mag_spin](float mag)
                       {
                         QSignalBlocker b(mag_spin);
                         mag_spin->setValue(double(mag));
                       });

      QObject::connect(canvas,
                       &VectorCanvas::angle_changed,
                       widget,
                       [angle_spin](float deg)
                       {
                         QSignalBlocker b(angle_spin);
                         angle_spin->setValue(double(deg));
                       });

      // Magnitude spinbox → canvas
      QObject::connect(mag_spin,
                       qOverload<double>(&QDoubleSpinBox::valueChanged),
                       widget,
                       [canvas](double v) { canvas->set_magnitude(float(v)); });

      // Angle spinbox → canvas
      QObject::connect(angle_spin,
                       qOverload<double>(&QDoubleSpinBox::valueChanged),
                       widget,
                       [canvas](double v) { canvas->set_angle_deg(float(v)); });

      // Lock toggle → canvas + angle spinbox enable state
      QObject::connect(lock_cb,
                       &QCheckBox::toggled,
                       widget,
                       [canvas, angle_spin](bool checked)
                       {
                         canvas->set_locked(checked);
                         angle_spin->setEnabled(!checked);
                       });

      // --- Graph signals ---------------------------------------------------

      // Live drag / spinbox change
      QObject::connect(canvas,
                       &VectorCanvas::value_changed,
                       widget,
                       [widget](glm::vec2)
                       {
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      // Committed (drag release, lock toggle, spinbox enter)
      QObject::connect(canvas,
                       &VectorCanvas::drag_ended,
                       widget,
                       [widget](glm::vec2) { Q_EMIT widget->edit_ended(); });

      // Spinboxes commit on editingFinished (Return / focus-out)
      auto spinbox_commit = [widget]() { Q_EMIT widget->edit_ended(); };
      QObject::connect(mag_spin,
                       &QDoubleSpinBox::editingFinished,
                       widget,
                       spinbox_commit);
      QObject::connect(angle_spin,
                       &QDoubleSpinBox::editingFinished,
                       widget,
                       spinbox_commit);

      // Lock toggle commits immediately
      QObject::connect(lock_cb,
                       &QCheckBox::toggled,
                       widget,
                       [widget](bool) { Q_EMIT widget->edit_ended(); });
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
