/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <array>
#include <string>
#include <vector>

#include <QWidget>

#include "meta/ext/color_gradient/color_gradient.hpp"

namespace meta::qt
{

// ---------------------------------------------------------------------------
// GradientPicker
//
// A self-contained gradient editor widget.
//
// Gradient bar:
//   Double-click empty area  → add stop at cursor position
//   Double-click stop        → open QColorDialog to edit colour
//   Left-drag stop           → move stop
//   Right-click stop         → remove stop (min 2 kept)
//
// Below the bar: a horizontal preset strip (always visible).
// Below that:    a "Shuffle colors" button.
// ---------------------------------------------------------------------------

class GradientPicker : public QWidget
{
  Q_OBJECT

public:
  explicit GradientPicker(std::vector<Stop>         &stops,
                          const std::vector<Preset> &presets,
                          QWidget                   *parent = nullptr);

  // Called externally when the attribute's preset list changes.
  void set_presets(const std::vector<Preset> &presets);

Q_SIGNALS:
  void value_changed(); // every incremental edit
  void edit_ended(); // committed (drag release, colour picked, preset applied)

protected:
  void  paintEvent(QPaintEvent *) override;
  void  mouseDoubleClickEvent(QMouseEvent *e) override;
  void  mousePressEvent(QMouseEvent *e) override;
  void  mouseMoveEvent(QMouseEvent *e) override;
  void  mouseReleaseEvent(QMouseEvent *e) override;
  void  contextMenuEvent(QContextMenuEvent *e) override;
  QSize sizeHint() const override;

private:
  // --- stop helpers

  QRectF bar_rect() const; // gradient bar area
  QRectF stop_rect(const Stop &s) const;
  int    hit_test(const QPoint &pos) const;
  void   sort_stops();
  void   rebuild_preset_strip();

  // --- colour conversion

  static QColor               to_qcolor(const std::array<float, 4> &c);
  static std::array<float, 4> from_qcolor(const QColor &c);

  std::vector<Stop>  &stops_;
  std::vector<Preset> presets_;

  int  selected_idx_ = -1;
  bool dragging_ = false;

  // Preset strip: one swatch widget per preset, held in a scroll area.
  // Rebuilt whenever presets_ changes.
  QWidget *preset_strip_ = nullptr;

  static constexpr int BAR_H = 28; // gradient bar height
  static constexpr int STOP_R = 6; // stop handle radius
  static constexpr int PAD = 10;
  static constexpr int STRIP_H = 36;  // preset strip height
  static constexpr int SWATCH_W = 60; // each preset swatch width
  static constexpr int RADIUS = 4;
};

} // namespace meta::qt