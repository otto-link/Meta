/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <cmath>

#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

#include "meta_qt/widgets/vector_canvas.hpp"

namespace meta::qt
{

static constexpr float DEG2RAD = float(M_PI) / 180.f;
static constexpr float RAD2DEG = 180.f / float(M_PI);

VectorCanvas::VectorCanvas(glm::vec2 &value,
                           float      k_max,
                           bool       locked,
                           QWidget   *parent)
    : QWidget(parent), value_(value), k_max_(k_max), locked_(locked)
{
  // Bootstrap polar from the initial value.
  if (locked_)
  {
    magnitude_ = value_.x; // kx == ky assumed
    angle_deg_ = 45.f;
  }
  else
  {
    magnitude_ = std::sqrt(value_.x * value_.x + value_.y * value_.y);
    angle_deg_ = (magnitude_ > 1e-6f) ? std::atan2(value_.y, value_.x) * RAD2DEG
                                      : 45.f;
  }

  setFixedSize(180, 180);
  setCursor(Qt::CrossCursor);
}

void VectorCanvas::apply_drag(const QPoint &pos)
{
  const QPoint ctr = center();
  const float  R = radius();

  const float dx = float(pos.x() - ctr.x());
  const float dy = -float(pos.y() - ctr.y()); // Y flipped

  const float dist = std::sqrt(dx * dx + dy * dy);
  magnitude_ = std::clamp(dist / R * k_max_, 0.f, k_max_);

  if (!locked_ && dist > 1e-3f) angle_deg_ = std::atan2(dy, dx) * RAD2DEG;

  commit();
  update();
}

QPoint VectorCanvas::center() const { return {width() / 2, height() / 2}; }

void VectorCanvas::commit()
{
  const float rad = angle_deg_ * DEG2RAD;

  if (locked_)
    value_ = {magnitude_, magnitude_};
  else
    value_ = {magnitude_ * std::cos(rad), magnitude_ * std::sin(rad)};

  Q_EMIT value_changed(value_);
  Q_EMIT magnitude_changed(magnitude_);
  Q_EMIT angle_changed(angle_deg_);
}

void VectorCanvas::mouseMoveEvent(QMouseEvent *e)
{
  if (dragging_) apply_drag(e->pos());
}

void VectorCanvas::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    dragging_ = true;
    apply_drag(e->pos());
  }
}

void VectorCanvas::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton && dragging_)
  {
    dragging_ = false;
    apply_drag(e->pos());
    Q_EMIT drag_ended(value_);
  }
}

void VectorCanvas::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QPoint    ctr = center();
  const float     R = radius();
  const QPalette &pal = palette();

  // Background
  p.fillRect(rect(), pal.color(QPalette::Base));

  // Concentric magnitude rings (25%, 50%, 75%, 100%)
  {
    QPen ring_pen(pal.color(QPalette::Mid), 1, Qt::DotLine);
    p.setPen(ring_pen);
    p.setBrush(Qt::NoBrush);
    for (int pct : {25, 50, 75, 100})
    {
      const int r = int(R * float(pct) / 100.f);
      p.drawEllipse(ctr, r, r);
    }
  }

  // Cross-hair axes
  {
    QPen ax(pal.color(QPalette::Mid), 1);
    p.setPen(ax);
    p.drawLine(ctr.x() - int(R), ctr.y(), ctr.x() + int(R), ctr.y());
    p.drawLine(ctr.x(), ctr.y() - int(R), ctr.x(), ctr.y() + int(R));
  }

  // Axis labels
  {
    p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    p.setPen(pal.color(QPalette::PlaceholderText));
    p.drawText(ctr.x() + int(R) - 14, ctr.y() - 3, "kx");
    p.drawText(ctr.x() + 3, ctr.y() - int(R) + 12, "ky");
  }

  // Isotropic arc (45° line when locked)
  if (locked_)
  {
    QPen iso_pen(pal.color(QPalette::Highlight).lighter(140), 1, Qt::DashLine);
    p.setPen(iso_pen);
    p.drawLine(ctr.x() - int(R * 0.707f),
               ctr.y() + int(R * 0.707f),
               ctr.x() + int(R * 0.707f),
               ctr.y() - int(R * 0.707f));
  }

  // Arrow
  const float  t = (k_max_ > 0.f) ? magnitude_ / k_max_ : 0.f;
  const float  rad = float(angle_deg_) * DEG2RAD;
  const float  ax = std::cos(rad);
  const float  ay = -std::sin(rad); // Y flipped
  const QPoint tip(ctr.x() + int(ax * R * t), ctr.y() + int(ay * R * t));

  const QColor arrow_col = locked_ ? pal.color(QPalette::Highlight).lighter(120)
                                   : pal.color(QPalette::Highlight);

  // Shaft
  p.setPen(QPen(arrow_col, 2));
  p.drawLine(ctr, tip);

  // Arrowhead
  if (t > 0.02f)
  {
    const float head_len = 8.f;
    const float head_ang = 25.f * DEG2RAD;
    const float back_ang = float(angle_deg_) * DEG2RAD + float(M_PI);

    QPointF h1(tip.x() + head_len * std::cos(back_ang + head_ang),
               tip.y() + head_len * std::sin(back_ang + head_ang));
    QPointF h2(tip.x() + head_len * std::cos(back_ang - head_ang),
               tip.y() + head_len * std::sin(back_ang - head_ang));

    p.setBrush(arrow_col);
    p.setPen(Qt::NoPen);
    QPolygonF head;
    head << QPointF(tip) << h1 << h2;
    p.drawPolygon(head);
  }

  // Tip dot
  p.setPen(Qt::NoPen);
  p.setBrush(pal.color(QPalette::Text));
  p.drawEllipse(tip, 3, 3);

  // Value readout
  {
    p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    p.setPen(pal.color(QPalette::Text));
    const QString txt = locked_ ? QString("k = %1").arg(double(magnitude_),
                                                        0,
                                                        'f',
                                                        3)
                                : QString("kx=%1  ky=%2")
                                      .arg(double(value_.x), 0, 'f', 3)
                                      .arg(double(value_.y), 0, 'f', 3);
    p.drawText(rect().adjusted(4, 4, -4, 0),
               Qt::AlignTop | Qt::AlignHCenter,
               txt);
  }
}

void VectorCanvas::set_angle_deg(float deg)
{
  if (!locked_)
  {
    angle_deg_ = deg;
    commit();
    update();
  }
}

void VectorCanvas::set_locked(bool locked)
{
  locked_ = locked;
  if (locked_) angle_deg_ = 45.f;
  commit();
  update();
}

void VectorCanvas::set_magnitude(float mag)
{
  magnitude_ = std::clamp(mag, 0.f, k_max_);
  commit();
  update();
}

float VectorCanvas::radius() const
{
  return float(std::min(width(), height())) * 0.5f - float(PAD);
}

} // namespace meta::qt
