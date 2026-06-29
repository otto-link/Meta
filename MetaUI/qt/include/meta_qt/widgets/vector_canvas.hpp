/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <QWidget>
#include <glm/glm.hpp>

namespace meta::qt
{

// ---------------------------------------------------------------------------
// VectorCanvas
//
// Visualises a 2-D vector as a draggable arrow on a polar
// canvas. The arrow tip sets both magnitude and direction.
//
// When locked (isotropic): kx = ky = magnitude, angle is fixed at 45°.
// When unlocked:           kx = mag * cos(angle), ky = mag * sin(angle).
//
// The magnitude is clamped to [0, k_max].
// ---------------------------------------------------------------------------

class VectorCanvas : public QWidget
{
  Q_OBJECT

public:
  VectorCanvas(glm::vec2 &value,
               float      k_max,
               bool       locked,
               QWidget   *parent = nullptr);

  // Called externally (sliders, lock toggle) — repaints without emitting.
  void set_locked(bool locked);
  void set_magnitude(float mag);
  void set_angle_deg(float deg);

  float magnitude() const { return magnitude_; }
  float angle_deg() const { return angle_deg_; }
  bool  is_locked() const { return locked_; }

Q_SIGNALS:
  void value_changed(glm::vec2 v);
  void drag_ended(glm::vec2 v);
  // Emitted when the canvas changes magnitude or angle so sliders can sync.
  void magnitude_changed(float mag);
  void angle_changed(float deg);

protected:
  void paintEvent(QPaintEvent *) override;
  void mousePressEvent(QMouseEvent *e) override;
  void mouseMoveEvent(QMouseEvent *e) override;
  void mouseReleaseEvent(QMouseEvent *e) override;

private:
  void   apply_drag(const QPoint &pos);
  void   commit(); // write polar → value_ and emit
  QPoint center() const;
  float  radius() const; // usable canvas radius in px

  glm::vec2 &value_;
  float      k_max_;
  bool       locked_;
  bool       dragging_ = false;

  float magnitude_ = 0.f;  // current k magnitude
  float angle_deg_ = 45.f; // current angle in degrees (0 = +x axis)

  static constexpr int PAD = 12;
};

} // namespace meta::qt