/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <QWidget>
#include <glm/glm.hpp>

namespace meta::qt
{

class XYCanvas : public QWidget
{
  Q_OBJECT

public:
  XYCanvas(glm::vec2 &value,
           float      min_x,
           float      max_x,
           float      min_y,
           float      max_y,
           bool       show_grid,
           QWidget   *parent = nullptr);

  // External write (Center / Random buttons) — updates value ref + repaints.
  void set_value(glm::vec2 v);

Q_SIGNALS:
  void value_changed(glm::vec2 v);
  void drag_ended(glm::vec2 v);

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

private:
  QRect     padded_rect() const;
  int       value_to_canvas_x(float vx, const QRect &r) const;
  int       value_to_canvas_y(float vy, const QRect &r) const;
  glm::vec2 canvas_to_value(const QPoint &pos) const;
  void      apply_pos(const QPoint &pos);

  glm::vec2 &value_;
  float      min_x_, max_x_, min_y_, max_y_;
  bool       show_grid_;
  bool       dragging_ = false;

  static constexpr int pad_ = 8;
  static constexpr int point_r_ = 5;
};

} // namespace meta::qt