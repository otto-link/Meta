/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>

#include "meta_qt/widgets/xy_canvas.hpp"

namespace meta::qt
{

XYCanvas::XYCanvas(glm::vec2 &value,
                   float      min_x,
                   float      max_x,
                   float      min_y,
                   float      max_y,
                   bool       show_grid,
                   QWidget   *parent)
    : QWidget(parent),
      value_(value),
      min_x_(min_x),
      max_x_(max_x),
      min_y_(min_y),
      max_y_(max_y),
      show_grid_(show_grid)
{
  setMinimumSize(160, 160);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(180);
  setMouseTracking(true);
  setCursor(Qt::CrossCursor);
}

void XYCanvas::apply_pos(const QPoint &pos)
{
  const glm::vec2 nv = canvas_to_value(pos);
  if (nv.x != value_.x || nv.y != value_.y)
  {
    value_.x = nv.x;
    value_.y = nv.y;
    update();
    Q_EMIT value_changed(value_);
  }
}

glm::vec2 XYCanvas::canvas_to_value(const QPoint &pos) const
{
  const QRect r = padded_rect();
  const float tx = float(pos.x() - r.left()) / float(r.width());
  const float ty = float(r.bottom() - pos.y()) / float(r.height()); // Y flipped
  const float vx = tx * (max_x_ - min_x_) + min_x_;
  const float vy = ty * (max_y_ - min_y_) + min_y_;
  return {std::clamp(vx, min_x_, max_x_), std::clamp(vy, min_y_, max_y_)};
}

void XYCanvas::mouseMoveEvent(QMouseEvent *e)
{
  if (dragging_) apply_pos(e->pos());
}

void XYCanvas::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    dragging_ = true;
    apply_pos(e->pos());
  }
}

void XYCanvas::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton && dragging_)
  {
    dragging_ = false;
    apply_pos(e->pos());
    Q_EMIT drag_ended(value_);
    update();
  }
}

QRect XYCanvas::padded_rect() const
{
  return rect().adjusted(pad_, pad_, -pad_, -pad_);
}

void XYCanvas::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QRect r = padded_rect();

  // Background
  p.fillRect(rect(), palette().color(QPalette::Base));

  // Grid
  if (show_grid_)
  {
    QPen grid_pen(palette().color(QPalette::Mid), 1, Qt::DotLine);
    p.setPen(grid_pen);
    constexpr int div = 4;
    for (int i = 1; i < div; ++i)
    {
      const int gx = r.left() + r.width() * i / div;
      const int gy = r.top() + r.height() * i / div;
      p.drawLine(gx, r.top(), gx, r.bottom());
      p.drawLine(r.left(), gy, r.right(), gy);
    }
  }

  // Axes through origin (only when origin falls inside the domain)
  {
    QPen axis_pen(palette().color(QPalette::Mid), 1, Qt::SolidLine);
    p.setPen(axis_pen);
    if (min_x_ <= 0 && 0 <= max_x_)
    {
      const int ax = value_to_canvas_x(0, r);
      p.drawLine(ax, r.top(), ax, r.bottom());
    }
    if (min_y_ <= 0 && 0 <= max_y_)
    {
      const int ay = value_to_canvas_y(0, r);
      p.drawLine(r.left(), ay, r.right(), ay);
    }
  }

  // Border
  p.setPen(QPen(palette().color(QPalette::Mid), 1));
  p.drawRect(r);

  // Crosshair through current value
  {
    const int cx = value_to_canvas_x(value_.x, r);
    const int cy = value_to_canvas_y(value_.y, r);
    QPen      cross_pen(palette().color(QPalette::Highlight), 1, Qt::DashLine);
    p.setPen(cross_pen);
    p.drawLine(cx, r.top(), cx, r.bottom());
    p.drawLine(r.left(), cy, r.right(), cy);
  }

  // Point
  {
    const int cx = value_to_canvas_x(value_.x, r);
    const int cy = value_to_canvas_y(value_.y, r);
    p.setPen(QPen(palette().color(QPalette::Text), 1));
    p.setBrush(dragging_ ? palette().color(QPalette::Highlight)
                         : palette().color(QPalette::Button));
    p.drawEllipse(QPoint(cx, cy), point_r_, point_r_);
  }

  // Coordinate label — bottom-left corner
  {
    p.setPen(palette().color(QPalette::Text));
    p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    const QString lbl = QString("(%1, %2)")
                            .arg(double(value_.x), 0, 'f', 3)
                            .arg(double(value_.y), 0, 'f', 3);
    p.drawText(r.adjusted(4, 0, 0, -3), Qt::AlignLeft | Qt::AlignBottom, lbl);
  }
}

void XYCanvas::set_value(glm::vec2 v)
{
  value_.x = std::clamp(v.x, min_x_, max_x_);
  value_.y = std::clamp(v.y, min_y_, max_y_);
  update();
}

int XYCanvas::value_to_canvas_x(float vx, const QRect &r) const
{
  const float t = float(vx - min_x_) / float(max_x_ - min_x_);
  return r.left() + int(t * r.width());
}

int XYCanvas::value_to_canvas_y(float vy, const QRect &r) const
{
  const float t = float(vy - min_y_) / float(max_y_ - min_y_);
  return r.bottom() - int(t * r.height()); // Y flipped
}

} // namespace meta::qt
