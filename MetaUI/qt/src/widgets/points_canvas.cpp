/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <algorithm>
#include <random>

#include <QFile>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QTextStream>
#include <QWheelEvent>

#include "meta_qt/widgets/points_canvas.hpp"

namespace meta::qt
{

PointsCanvas::PointsCanvas(std::vector<glm::vec3> &points,
                           float                   min_x,
                           float                   max_x,
                           float                   min_y,
                           float                   max_y,
                           float                   z_step,
                           QWidget                *parent)
    : QWidget(parent),
      points_(points),
      min_x_(min_x),
      max_x_(max_x),
      min_y_(min_y),
      max_y_(max_y),
      z_step_(z_step)
{
  setMinimumSize(200, 180);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(220);
  setMouseTracking(true);
  setCursor(Qt::CrossCursor);
}

QRect PointsCanvas::canvas_rect() const
{
  return rect().adjusted(PAD, PAD, -PAD, -PAD);
}

glm::vec2 PointsCanvas::canvas_to_value(const QPoint &p) const
{
  const QRect r = canvas_rect();
  const float tx = float(p.x() - r.left()) / float(r.width());
  const float ty = float(r.bottom() - p.y()) / float(r.height());
  return {min_x_ + tx * (max_x_ - min_x_), min_y_ + ty * (max_y_ - min_y_)};
}

void PointsCanvas::clear_all()
{
  points_.clear();
  hovered_idx_ = -1;
  drag_idx_ = -1;
  update();
  Q_EMIT points_changed();
  Q_EMIT drag_ended();
}

int PointsCanvas::hit_test(const QPoint &pos) const
{
  float best_d = HIT_R * HIT_R;
  int   best_i = -1;

  for (int i = 0; i < static_cast<int>(points_.size()); ++i)
  {
    const QPoint cp = value_to_canvas(points_[i].x, points_[i].y);
    const float  dx = float(pos.x() - cp.x());
    const float  dy = float(pos.y() - cp.y());
    const float  d = dx * dx + dy * dy;
    if (d < best_d)
    {
      best_d = d;
      best_i = i;
    }
  }
  return best_i;
}

void PointsCanvas::leaveEvent(QEvent *)
{
  hovered_idx_ = -1;
  update();
}

void PointsCanvas::load_csv(const QString &path)
{
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

  std::vector<glm::vec3> loaded;
  QTextStream            in(&file);

  while (!in.atEnd())
  {
    const QString line = in.readLine().trimmed();
    if (line.isEmpty() || line.startsWith('#')) continue;

    const QStringList parts = line.split(',');
    if (parts.size() < 2) continue;

    bool        ok_x = false, ok_y = false, ok_z = false;
    const float x = parts[0].toFloat(&ok_x);
    const float y = parts[1].toFloat(&ok_y);
    const float z = parts.size() >= 3
                        ? std::clamp(parts[2].toFloat(&ok_z), 0.f, 1.f)
                        : 1.f;

    if (!ok_x || !ok_y) continue;

    loaded.push_back(
        {std::clamp(x, min_x_, max_x_), std::clamp(y, min_y_, max_y_), z});
  }

  if (!loaded.empty())
  {
    points_ = std::move(loaded);
    update();
    Q_EMIT points_changed();
    Q_EMIT drag_ended();
  }
}

void PointsCanvas::mouseMoveEvent(QMouseEvent *e)
{
  if (drag_idx_ >= 0)
  {
    const glm::vec2 v = canvas_to_value(e->pos());
    points_[drag_idx_].x = std::clamp(v.x, min_x_, max_x_);
    points_[drag_idx_].y = std::clamp(v.y, min_y_, max_y_);
    moved_during_drag_ = true;
    update();
    Q_EMIT points_changed();
  }
  else
  {
    const int prev = hovered_idx_;
    hovered_idx_ = hit_test(e->pos());
    setCursor(hovered_idx_ >= 0 ? Qt::SizeAllCursor : Qt::CrossCursor);
    if (hovered_idx_ != prev) update();
  }
}

void PointsCanvas::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    const int idx = hit_test(e->pos());
    if (idx >= 0)
    {
      drag_idx_ = idx;
      moved_during_drag_ = false;
    }
    else
    {
      // Add new point with z = 1
      const glm::vec2 v = canvas_to_value(e->pos());
      points_.push_back({v.x, v.y, 1.f});
      drag_idx_ = static_cast<int>(points_.size()) - 1;
      moved_during_drag_ = false;
      Q_EMIT points_changed();
      update();
    }
  }
  else if (e->button() == Qt::RightButton)
  {
    const int idx = hit_test(e->pos());
    if (idx >= 0)
    {
      points_.erase(points_.begin() + idx);
      hovered_idx_ = -1;
      drag_idx_ = -1;
      update();
      Q_EMIT points_changed();
      Q_EMIT drag_ended();
    }
  }
}

void PointsCanvas::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton && drag_idx_ >= 0)
  {
    if (moved_during_drag_) Q_EMIT drag_ended();
    drag_idx_ = -1;
    update();
  }
}

void PointsCanvas::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QRect r = canvas_rect();

  // Background
  p.fillRect(rect(), palette().color(QPalette::Base));

  // Grid
  {
    QPen gp(palette().color(QPalette::Mid), 1, Qt::DotLine);
    p.setPen(gp);
    constexpr int div = 4;
    for (int i = 1; i < div; ++i)
    {
      const int gx = r.left() + r.width() * i / div;
      const int gy = r.top() + r.height() * i / div;
      p.drawLine(gx, r.top(), gx, r.bottom());
      p.drawLine(r.left(), gy, r.right(), gy);
    }
  }

  // Border
  p.setPen(QPen(palette().color(QPalette::Mid), 1));
  p.setBrush(Qt::NoBrush);
  p.drawRect(r);

  // Points
  for (int i = 0; i < static_cast<int>(points_.size()); ++i)
  {
    const auto  &pt = points_[i];
    const QPoint cp = value_to_canvas(pt.x, pt.y);
    const bool   hov = (i == hovered_idx_);
    const bool   drg = (i == drag_idx_);

    const QColor fill = z_to_color(pt.z);

    p.setPen(
        QPen(drg || hov ? palette().color(QPalette::Text) : fill.darker(160),
             1));
    p.setBrush(fill);
    p.drawEllipse(cp,
                  int(POINT_R + (hov || drg ? 2 : 0)),
                  int(POINT_R + (hov || drg ? 2 : 0)));

    // Show z value on hover
    if (hov || drg)
    {
      p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
      p.setPen(palette().color(QPalette::Text));
      p.drawText(cp + QPoint(int(POINT_R) + 3, 4),
                 QString::number(double(pt.z), 'f', 2));
    }
  }

  // Point count
  p.setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
  p.setPen(palette().color(QPalette::PlaceholderText));
  p.drawText(r.adjusted(4, 0, 0, -3),
             Qt::AlignLeft | Qt::AlignBottom,
             QString("%1 pt%2")
                 .arg(points_.size())
                 .arg(points_.size() != 1 ? "s" : ""));

  // Legend: colour ramp strip bottom-right
  {
    const int strip_w = 60;
    const int strip_h = 6;
    const int sx = r.right() - strip_w - 2;
    const int sy = r.bottom() - strip_h - 3;

    for (int px = 0; px < strip_w; ++px)
    {
      const float t = float(px) / float(strip_w - 1);
      p.setPen(z_to_color(t));
      p.drawLine(sx + px, sy, sx + px, sy + strip_h);
    }
    p.setPen(QPen(palette().color(QPalette::Mid), 1));
    p.setBrush(Qt::NoBrush);
    p.drawRect(sx, sy, strip_w, strip_h);
  }
}

void PointsCanvas::randomize(int count)
{
  std::mt19937                          rng{std::random_device{}()};
  std::uniform_real_distribution<float> dx(min_x_, max_x_);
  std::uniform_real_distribution<float> dy(min_y_, max_y_);
  std::uniform_real_distribution<float> dz(0.f, 1.f);

  points_.clear();
  points_.reserve(count);
  for (int i = 0; i < count; ++i)
    points_.push_back({dx(rng), dy(rng), dz(rng)});

  update();
  Q_EMIT points_changed();
  Q_EMIT drag_ended();
}

QPoint PointsCanvas::value_to_canvas(float x, float y) const
{
  const QRect r = canvas_rect();
  const float tx = (x - min_x_) / (max_x_ - min_x_);
  const float ty = (y - min_y_) / (max_y_ - min_y_);
  return {r.left() + int(tx * r.width()),
          r.bottom() - int(ty * r.height())}; // Y flipped
}

void PointsCanvas::wheelEvent(QWheelEvent *e)
{
  const int idx = hit_test(e->position().toPoint());
  if (idx < 0) return;

  const float delta = (e->angleDelta().y() > 0 ? 1.f : -1.f) * z_step_;
  points_[idx].z = std::clamp(points_[idx].z + delta, 0.f, 1.f);

  update();
  Q_EMIT points_changed();
  Q_EMIT drag_ended(); // treat each scroll tick as a committed edit
  e->accept();
}

QColor PointsCanvas::z_to_color(float z) const
{
  // Simple blue(0) → cyan → green → yellow → red(1) heatmap.
  z = std::clamp(z, 0.f, 1.f);
  float r, g, b;
  if (z < 0.25f)
  {
    r = 0.f;
    g = z * 4.f;
    b = 1.f;
  }
  else if (z < 0.5f)
  {
    r = 0.f;
    g = 1.f;
    b = 1.f - (z - 0.25f) * 4.f;
  }
  else if (z < 0.75f)
  {
    r = (z - 0.5f) * 4.f;
    g = 1.f;
    b = 0.f;
  }
  else
  {
    r = 1.f;
    g = 1.f - (z - 0.75f) * 4.f;
    b = 0.f;
  }
  return QColor(int(r * 255), int(g * 255), int(b * 255));
}

} // namespace meta::qt
