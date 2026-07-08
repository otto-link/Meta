/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta_qt/widgets/curve_canvas.hpp"

namespace meta::qt
{

CurveCanvas::CurveCanvas(std::vector<float> &samples,
                         int                 curve_size,
                         float               min_x,
                         float               max_x,
                         float               min_y,
                         float               max_y,
                         QWidget            *parent)
    : QWidget(parent),
      samples_(samples),
      curve_size_(curve_size),
      min_x_(min_x),
      max_x_(max_x),
      min_y_(min_y),
      max_y_(max_y)
{
  setMinimumSize(200, 140);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(160);
  setMouseTracking(true);

  // bootstrap control points from the current sample buffer,
  // or default to a straight diagonal if the buffer is empty / flat.
  init_control_points();
}

QPointF CurveCanvas::canvas_to_sample(const QPointF &pt, const QRect &r) const
{
  const float nx = float(pt.x() - r.left()) / float(r.width());
  const float ny = float(r.bottom() - pt.y()) / float(r.height());
  return {min_x_ + nx * (max_x_ - min_x_), min_y_ + ny * (max_y_ - min_y_)};
}

float CurveCanvas::catmull_rom(float p0, float p1, float p2, float p3, float t)
{
  const float t2 = t * t;
  const float t3 = t2 * t;
  return 0.5f * ((2.f * p1) + (-p0 + p2) * t +
                 (2.f * p0 - 5.f * p1 + 4.f * p2 - p3) * t2 +
                 (-p0 + 3.f * p1 - 3.f * p2 + p3) * t3);
}

int CurveCanvas::hit_test(const QPoint &pos, const QRect &r) const
{
  for (int i = 0; i < static_cast<int>(ctrl_.size()); ++i)
  {
    const QPointF cp = sample_to_canvas(ctrl_[i].x(), ctrl_[i].y(), r);
    if (QLineF(cp, QPointF(pos)).length() <= hit_r_) return i;
  }
  return -1;
}

void CurveCanvas::init_control_points()
{
  ctrl_.clear();

  // Case 1: empty / trivially flat buffer → default identity diagonal.
  const bool is_empty = (samples_.size() < 2);
  const bool is_flat = !is_empty && [&]()
  {
    const float v0 = samples_.front();
    for (float v : samples_)
      if (std::abs(v - v0) > 1e-5f) return false;
    return true;
  }();

  if (is_empty || is_flat)
  {
    constexpr int n_boot = 5;
    for (int i = 0; i < n_boot; ++i)
    {
      const float t = float(i) / float(n_boot - 1);
      ctrl_.push_back(
          {min_x_ + t * (max_x_ - min_x_), min_y_ + t * (max_y_ - min_y_)});
    }
    resample(); // buffer was trivial — overwrite is fine
    return;
  }

  // Case 2: meaningful buffer — extract control points from extrema so the
  // Catmull-Rom reconstruction is faithful and resample() is NOT called
  // (the existing buffer remains the ground truth until the user edits).
  //
  // Algorithm: always keep first + last sample, then insert local extrema
  // (sign-change of first difference), capped at max_ctrl_ points total.
  // When there are too many extrema, downsample uniformly among them.

  const int n = static_cast<int>(samples_.size());

  // Collect indices of local minima/maxima (excluding endpoints).
  std::vector<int> extrema;
  for (int i = 1; i < n - 1; ++i)
  {
    const float dl = samples_[i] - samples_[i - 1];
    const float dr = samples_[i + 1] - samples_[i];
    if (dl * dr < 0.f || (dl == 0.f && dr != 0.f)) extrema.push_back(i);
  }

  // Build the final index list: endpoints + up to (max_ctrl_-2) extrema.
  std::vector<int> indices;
  indices.push_back(0);

  const int max_inner = max_ctrl_ - 2;
  if (static_cast<int>(extrema.size()) <= max_inner)
  {
    for (int idx : extrema)
      indices.push_back(idx);
  }
  else
  {
    // Uniform subsample of extrema.
    for (int k = 0; k < max_inner; ++k)
    {
      const int ei = static_cast<int>(
          float(k) / float(max_inner - 1) * float(extrema.size() - 1) + 0.5f);
      indices.push_back(extrema[ei]);
    }
  }

  indices.push_back(n - 1);

  // Deduplicate (uniform subsample may repeat endpoints in edge cases).
  std::sort(indices.begin(), indices.end());
  indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

  for (int idx : indices)
  {
    const float t = float(idx) / float(n - 1);
    ctrl_.push_back({min_x_ + t * (max_x_ - min_x_),
                     std::clamp(samples_[idx], min_y_, max_y_)});

    qDebug() << ctrl_.back();
  }
  // Do NOT call resample() — samples_ is already correct.
}

void CurveCanvas::leaveEvent(QEvent *)
{
  hovered_idx_ = -1;
  update();
}

void CurveCanvas::mouseMoveEvent(QMouseEvent *e)
{
  const QRect r = padded_rect();

  if (drag_idx_ >= 0)
  {
    QPointF np = canvas_to_sample(QPointF(e->pos()) + drag_offset_, r);
    // Clamp to domain.
    np.setX(std::clamp(double(np.x()), double(min_x_), double(max_x_)));
    np.setY(std::clamp(double(np.y()), double(min_y_), double(max_y_)));
    // Prevent crossing neighbours on X.
    if (drag_idx_ > 0)
      np.setX(
          std::max(double(np.x()), double(ctrl_[drag_idx_ - 1].x()) + 1e-4));
    if (drag_idx_ < static_cast<int>(ctrl_.size()) - 1)
      np.setX(
          std::min(double(np.x()), double(ctrl_[drag_idx_ + 1].x()) - 1e-4));

    ctrl_[drag_idx_] = np;
    resample();
    Q_EMIT curve_changed();
    update();
  }
  else
  {
    const int prev_hover = hovered_idx_;
    hovered_idx_ = hit_test(e->pos(), r);
    if (hovered_idx_ != prev_hover)
    {
      setCursor(hovered_idx_ >= 0 ? Qt::SizeAllCursor : Qt::CrossCursor);
      update();
    }
  }
}

void CurveCanvas::mousePressEvent(QMouseEvent *e)
{
  const QRect r = padded_rect();
  const int   idx = hit_test(e->pos(), r);

  if (e->button() == Qt::LeftButton)
  {
    if (idx >= 0)
    {
      drag_idx_ = idx;
      drag_offset_ = sample_to_canvas(ctrl_[idx].x(), ctrl_[idx].y(), r) -
                     QPointF(e->pos());
    }
    else
    {
      // Add a new control point.
      const QPointF np = canvas_to_sample(QPointF(e->pos()), r);
      ctrl_.push_back(np);
      sort_ctrl();
      resample();
      Q_EMIT curve_changed();
      update();
    }
  }
  else if (e->button() == Qt::RightButton)
  {
    if (idx >= 0 && ctrl_.size() > 2)
    {
      ctrl_.erase(ctrl_.begin() + idx);
      resample();
      Q_EMIT curve_changed();
      update();
    }
  }
}

void CurveCanvas::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton && drag_idx_ >= 0)
  {
    sort_ctrl();
    drag_idx_ = -1;
    resample();
    Q_EMIT drag_ended();
    update();
  }
}

QRect CurveCanvas::padded_rect() const
{
  return rect().adjusted(pad_, pad_, -pad_, -pad_);
}

void CurveCanvas::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QRect r = rect().adjusted(pad_, pad_, -pad_, -pad_);

  // Background
  p.fillRect(rect(), palette().color(QPalette::Base));

  // Grid
  {
    QPen grid_pen(palette().color(QPalette::Mid), 1, Qt::DotLine);
    p.setPen(grid_pen);
    constexpr int divisions = 4;
    for (int i = 1; i < divisions; ++i)
    {
      int x = r.left() + r.width() * i / divisions;
      int y = r.top() + r.height() * i / divisions;
      p.drawLine(x, r.top(), x, r.bottom());
      p.drawLine(r.left(), y, r.right(), y);
    }
    // Border
    p.drawRect(r);
  }

  // Curve (drawn from resampled buffer)
  if (!samples_.empty())
  {
    QPen curve_pen(palette().color(QPalette::Highlight), 2);
    p.setPen(curve_pen);

    QPainterPath path;
    for (int i = 0; i < static_cast<int>(samples_.size()); ++i)
    {
      const QPointF pt = sample_to_canvas(
          min_x_ + (max_x_ - min_x_) * i / float(samples_.size() - 1),
          samples_[i],
          r);
      if (i == 0)
        path.moveTo(pt);
      else
        path.lineTo(pt);
    }
    p.drawPath(path);
  }

  // Control points
  for (int i = 0; i < static_cast<int>(ctrl_.size()); ++i)
  {
    const QPointF canvas_pt = sample_to_canvas(ctrl_[i].x(), ctrl_[i].y(), r);
    const bool    hovered = (i == hovered_idx_);
    const bool    dragged = (i == drag_idx_);

    p.setPen(QPen(palette().color(QPalette::Text), 1));
    p.setBrush(dragged   ? palette().color(QPalette::Highlight)
               : hovered ? palette().color(QPalette::Light)
                         : palette().color(QPalette::Button));
    p.drawEllipse(canvas_pt, point_r_, point_r_);
  }
}

void CurveCanvas::resample()
{
  if (ctrl_.size() < 2) return;

  samples_.assign(curve_size_, 0.f);

  const int n = static_cast<int>(ctrl_.size());

  for (int s = 0; s < curve_size_; ++s)
  {
    const float x = min_x_ +
                    (max_x_ - min_x_) * float(s) / float(curve_size_ - 1);

    // Find the segment [i, i+1] that contains x.
    int i = 0;
    while (i < n - 2 && ctrl_[i + 1].x() < x)
      ++i;

    // Phantom endpoints for Catmull-Rom at the boundaries.
    const float p0 = (i > 0) ? float(ctrl_[i - 1].y()) : float(ctrl_[i].y());
    const float p1 = float(ctrl_[i].y());
    const float p2 = float(ctrl_[i + 1].y());
    const float p3 = (i + 2 < n) ? float(ctrl_[i + 2].y())
                                 : float(ctrl_[i + 1].y());

    // Local t within the segment.
    const float seg_len = float(ctrl_[i + 1].x()) - float(ctrl_[i].x());
    const float t = (seg_len > 1e-6f) ? (x - float(ctrl_[i].x())) / seg_len
                                      : 0.f;

    samples_[s] = std::clamp(catmull_rom(p0, p1, p2, p3, t), min_y_, max_y_);
  }
}

void CurveCanvas::reset_to_value()
{
  init_control_points();
  update();
}

QPointF CurveCanvas::sample_to_canvas(float x, float y, const QRect &r) const
{
  const float nx = (x - min_x_) / (max_x_ - min_x_);
  const float ny = (y - min_y_) / (max_y_ - min_y_);
  return {r.left() + nx * r.width(), r.bottom() - ny * r.height()}; // Y flipped
}

void CurveCanvas::sort_ctrl()
{
  std::stable_sort(ctrl_.begin(),
                   ctrl_.end(),
                   [](const QPointF &a, const QPointF &b)
                   { return a.x() < b.x(); });
}

} // namespace meta::qt
