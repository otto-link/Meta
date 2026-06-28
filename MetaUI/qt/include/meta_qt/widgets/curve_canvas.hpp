/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace meta::qt
{

class CurveCanvas : public QWidget
{
  Q_OBJECT

public:
  CurveCanvas(std::vector<float> &samples,
              int                 curve_size,
              float               min_x,
              float               max_x,
              float               min_y,
              float               max_y,
              QWidget            *parent = nullptr);

  // re-bootstrap control points from the current sample buffer.
  // call after externally modifying the value (e.g. Reset button).
  void reset_to_value();

Q_SIGNALS:
  void curve_changed();
  void drag_ended();

protected:
  void leaveEvent(QEvent *) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void paintEvent(QPaintEvent *) override;

private:
  // --- Geometry helpers

  QRect padded_rect() const;

  // map a domain point (x in [min_x,max_x], y in [min_y,max_y]) → canvas px.
  QPointF sample_to_canvas(float x, float y, const QRect &r) const;

  // map canvas px → domain point.
  QPointF canvas_to_sample(const QPointF &pt, const QRect &r) const;

  // returns the index of the first control point within
  // hit_this->style.border_radius() of pos, or -1 if none.
  int hit_test(const QPoint &pos, const QRect &r) const;

  // --- Control point helpers

  // bootstrap from the current sample buffer using 5 evenly-spaced points.
  void init_control_points();
  void sort_ctrl();

  // --- Catmull-Rom resampling

  // evaluate a Catmull-Rom segment at t in [0,1] given four points.
  float catmull_rom(float p0, float p1, float p2, float p3, float t);

  void resample();

  // --- Members

  std::vector<float> &samples_;
  int                 curve_size_;
  float               min_x_, max_x_, min_y_, max_y_;

  std::vector<QPointF> ctrl_;

  int     drag_idx_ = -1;
  int     hovered_idx_ = -1;
  QPointF drag_offset_;

  static constexpr int   pad_ = 8;       // canvas padding in px
  static constexpr int   max_ctrl_ = 32; // max control points from bootstrap
  static constexpr float point_r_ =
      5.f; // control point this->style.border_radius()
  static constexpr float hit_r_ = 8.f; // hit-test this->style.border_radius()
};

} // namespace meta::qt
