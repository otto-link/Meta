/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta_qt/widgets/style.hpp"

namespace meta::qt
{

Style::Style(const QWidget *widget) : widget_(widget) {}

int Style::pixel_metric(QStyle::PixelMetric metric, int fallback) const
{
  if (!widget_) return fallback;

  int value = widget_->style()->pixelMetric(metric, nullptr, widget_);

  return value >= 0 ? value : fallback;
}

} // namespace meta::qt
