/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QStyle>
#include <QWidget>

namespace meta::qt
{

class Style
{
public:
  explicit Style(const QWidget *widget);

  int border_radius() const { return 4; }

  int border_width() const { return 1; }

  int border_width_hovered() const { return border_width() + 1; }

  int button_ticks() const { return 100; }

  int horizontal_spacing() const
  {
    return pixel_metric(QStyle::PM_LayoutHorizontalSpacing, 8);
  }

  int label_max_len() const { return 32; }

  int vertical_spacing() const
  {
    return pixel_metric(QStyle::PM_LayoutVerticalSpacing, 6);
  }

private:
  int pixel_metric(QStyle::PixelMetric metric, int fallback) const;

  const QWidget *widget_;
};

} // namespace meta::qt