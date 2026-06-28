/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <glm/glm.hpp>

#include "meta_common.hpp"
#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widgets/points_canvas.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<std::vector<glm::vec3>>
{
  static MetaWidget *render(Attribute<std::vector<glm::vec3>> &attr,
                            QWidget                           *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);

    const float min_x = meta::common::try_get<float>(attr, "ui.min_x", 0.f);
    const float max_x = meta::common::try_get<float>(attr, "ui.max_x", 1.f);
    const float min_y = meta::common::try_get<float>(attr, "ui.min_y", 0.f);
    const float max_y = meta::common::try_get<float>(attr, "ui.max_y", 1.f);
    const float z_step = meta::common::try_get<float>(attr, "ui.z_step", 0.05f);

    std::vector<glm::vec3> &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (widget_type.empty()) widget_type = "PointsEditor";

    if (widget_type == "PointsEditor") // --- PointsEditor
    {
      if (!label_txt.empty())
        layout->addWidget(
            new QLabel(QString::fromStdString(label_txt), widget));

      auto *canvas =
          new PointsCanvas(value, min_x, max_x, min_y, max_y, z_step, widget);
      layout->addWidget(canvas);

      // --- Toolbar

      auto *toolbar = new QHBoxLayout();

      // Clear
      auto *clear_btn = new QPushButton(QObject::tr("Clear"), widget);
      clear_btn->setFixedHeight(22);
      clear_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      toolbar->addWidget(clear_btn);

      auto *rand_btn = new QPushButton(QObject::tr("Randomize"), widget);
      rand_btn->setFixedHeight(22);
      rand_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      toolbar->addWidget(rand_btn);

      // From CSV
      auto *csv_btn = new QPushButton(QObject::tr("From CSV…"), widget);
      csv_btn->setFixedHeight(22);
      csv_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      toolbar->addWidget(csv_btn);

      layout->addLayout(toolbar);

      // --- Connections

      // Live edits (add / move / z scroll) → edit_started + value_changed
      QObject::connect(canvas,
                       &PointsCanvas::points_changed,
                       widget,
                       [widget]()
                       {
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      // Committed edits (drag release / delete / clear / randomize / csv)
      QObject::connect(canvas,
                       &PointsCanvas::drag_ended,
                       widget,
                       [widget]() { Q_EMIT widget->edit_ended(); });

      // Clear
      QObject::connect(clear_btn,
                       &QPushButton::clicked,
                       canvas,
                       &PointsCanvas::clear_all);

      // Randomize
      QObject::connect(rand_btn,
                       &QPushButton::clicked,
                       widget,
                       [value, canvas]() { canvas->randomize(value.size()); });

      // From CSV
      QObject::connect(csv_btn,
                       &QPushButton::clicked,
                       widget,
                       [canvas, widget]()
                       {
                         const QString path = QFileDialog::getOpenFileName(
                             widget,
                             QObject::tr("Load points from CSV"),
                             QDir::homePath(),
                             QObject::tr("CSV files (*.csv);;All Files (*)"),
                             nullptr,
                             QFileDialog::DontUseNativeDialog);
                         if (!path.isEmpty()) canvas->load_csv(path);
                       });
    }
    else // --- ERROR
    {
      layout->addWidget(
          make_error_widget(&attr, "unsupported widget type", widget));
    }

    return widget;
  }
};

} // namespace meta::qt
