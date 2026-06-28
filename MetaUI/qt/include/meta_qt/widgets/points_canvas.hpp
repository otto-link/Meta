/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <vector>

#include <QWidget>
#include <glm/glm.hpp>

namespace meta::qt
{

class PointsCanvas : public QWidget
{
  Q_OBJECT

public:
  PointsCanvas(std::vector<glm::vec3> &points,
               float                   min_x,
               float                   max_x,
               float                   min_y,
               float                   max_y,
               float                   z_step,
               QWidget                *parent = nullptr);

  void clear_all();
  void randomize(int count);
  void load_csv(const QString &path); // x,y,z per line (z clamped to [0,1])

Q_SIGNALS:
  void points_changed();
  void drag_ended();

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;
  void leaveEvent(QEvent *) override;
  void wheelEvent(QWheelEvent *e) override;

private:
  QRect     canvas_rect() const;
  QPoint    value_to_canvas(float x, float y) const;
  glm::vec2 canvas_to_value(const QPoint &p) const;
  int       hit_test(const QPoint &pos) const;
  QColor    z_to_color(float z) const; // blue(0) → red(1)

  std::vector<glm::vec3> &points_;
  float                   min_x_, max_x_, min_y_, max_y_, z_step_;

  int  hovered_idx_ = -1;
  int  drag_idx_ = -1;
  bool moved_during_drag_ = false;

  static constexpr int   PAD = 10;
  static constexpr float HIT_R = 8.f;
  static constexpr float POINT_R = 6.f;
};

} // namespace meta::qt