/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <QLabel>
#include <QVBoxLayout>

#include "meta_common.hpp"
#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widgets/gradient_picker.hpp"

#include "meta/ext/color_gradient/color_gradient.hpp"

namespace meta::qt
{

// ---------------------------------------------------------------------------
// WidgetRenderer<ColorGradient>
//
// widget_type: "GradientEditor" (default)
//
// No extra metadata needed — presets and stops come from the attribute itself.
// ---------------------------------------------------------------------------

template <> struct WidgetRenderer<meta::ColorGradient>
{
  static MetaWidget *render(Attribute<meta::ColorGradient> &attr,
                            QWidget                        *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);

    ColorGradient &cga = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (widget_type.empty()) widget_type = "GradientEditor";

    if (widget_type == "None") // --- None
    {
      return nullptr;
    }
    else if (widget_type == "GradientEditor") // --- GradientEditor
    {
      if (!label_txt.empty())
        layout->addWidget(
            new QLabel(QString::fromStdString(label_txt), widget));

      auto *picker = new GradientPicker(cga.value(), cga.presets(), widget);
      layout->addWidget(picker);

      widget->set_sync_from_model([picker]() { picker->update(); });

      // Live edits
      QObject::connect(picker,
                       &GradientPicker::value_changed,
                       widget,
                       [widget]()
                       {
                         Q_EMIT widget->edit_started();
                         Q_EMIT widget->value_changed();
                       });

      // Committed
      QObject::connect(picker,
                       &GradientPicker::edit_ended,
                       widget,
                       [widget]() { Q_EMIT widget->edit_ended(); });
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
