/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <algorithm>
#include <cmath>

#include <QColorDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>

#include "meta/logger.hpp"
#include "meta_qt/widgets/gradient_picker.hpp"

namespace meta::qt
{

// ---------------------------------------------------------------------------
// Colour conversion
// ---------------------------------------------------------------------------

QColor GradientPicker::to_qcolor(const std::array<float, 4> &c)
{
  return QColor(int(std::clamp(c[0], 0.f, 1.f) * 255.f),
                int(std::clamp(c[1], 0.f, 1.f) * 255.f),
                int(std::clamp(c[2], 0.f, 1.f) * 255.f),
                int(std::clamp(c[3], 0.f, 1.f) * 255.f));
}

std::array<float, 4> GradientPicker::from_qcolor(const QColor &c)
{
  return {float(c.redF()),
          float(c.greenF()),
          float(c.blueF()),
          float(c.alphaF())};
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GradientPicker::GradientPicker(std::vector<Stop>         &stops,
                               const std::vector<Preset> &presets,
                               QWidget                   *parent)
    : QWidget(parent), stops_(stops), presets_(presets)
{
  // The widget is purely custom-painted for the bar; the preset strip and
  // shuffle button live as child widgets laid out below it.
  // We use a manual layout: bar is painted in paintEvent, children are
  // positioned in resizeEvent via a QVBoxLayout on a transparent container.

  auto *vbox = new QVBoxLayout(this);
  vbox->setContentsMargins(0, 0, 0, 0);
  vbox->setSpacing(4);

  // Spacer for the painted bar area
  vbox->addSpacing(PAD + BAR_H + STOP_R * 2 + 6);

  // Preset strip (scroll area)
  auto *scroll = new QScrollArea(this);
  int   h_scrollbar = scroll->horizontalScrollBar()->sizeHint().height();
  scroll->setFixedHeight(h_scrollbar + STRIP_H + 2);
  scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scroll->setFrameShape(QFrame::NoFrame);
  scroll->setWidgetResizable(false);

  preset_strip_ = new QWidget();
  preset_strip_->setFixedHeight(STRIP_H);
  scroll->setWidget(preset_strip_);
  vbox->addWidget(scroll);

  // Shuffle button
  if (false)
  {
    auto *shuffle_btn = new QPushButton(QObject::tr("Shuffle colors"), this);
    shuffle_btn->setFixedHeight(22);
    vbox->addWidget(shuffle_btn);

    connect(shuffle_btn,
            &QPushButton::clicked,
            this,
            [this]()
            {
              // Randomise colours, keep positions.
              std::srand(static_cast<unsigned>(std::time(nullptr)));
              for (auto &s : stops_)
              {
                s.color = {float(std::rand()) / RAND_MAX,
                           float(std::rand()) / RAND_MAX,
                           float(std::rand()) / RAND_MAX,
                           1.f};
              }
              update();
              Q_EMIT value_changed();
              Q_EMIT edit_ended();
            });
  }

  rebuild_preset_strip();
}

// ---------------------------------------------------------------------------
// Preset strip
// ---------------------------------------------------------------------------

void GradientPicker::set_presets(const std::vector<Preset> &presets)
{
  presets_ = presets;
  rebuild_preset_strip();
}

void GradientPicker::rebuild_preset_strip()
{
  // Delete all existing children of preset_strip_
  qDeleteAll(
      preset_strip_->findChildren<QWidget *>(QString(),
                                             Qt::FindDirectChildrenOnly));

  auto *row = new QHBoxLayout(preset_strip_);
  row->setContentsMargins(2, 2, 2, 2);
  row->setSpacing(4);

  for (const auto &preset : presets_)
  {
    // Each swatch: gradient preview + name label stacked in a button-like
    // widget
    auto *swatch = new QWidget(preset_strip_);
    swatch->setFixedSize(SWATCH_W, STRIP_H - 4);
    swatch->setCursor(Qt::PointingHandCursor);
    swatch->setToolTip(QString::fromStdString(preset.name));

    // Paint the gradient inside the swatch via a custom paint filter
    // We use a label with a pixmap for simplicity (no subclass needed).
    QPixmap pix(SWATCH_W, STRIP_H - 4);
    {
      QPainter pp(&pix);
      pp.setRenderHint(QPainter::Antialiasing);

      QLinearGradient grad(0, 0, pix.width(), 0);
      for (const auto &s : preset.stops)
        grad.setColorAt(double(s.position), to_qcolor(s.color));

      pp.fillRect(pix.rect(), grad);

      // Name overlay
      pp.setPen(Qt::white);
      pp.setFont(QFont(pp.font().family(), 7));
      pp.drawText(pix.rect().adjusted(2, 0, -2, 0),
                  Qt::AlignBottom | Qt::AlignHCenter,
                  QString::fromStdString(preset.name));

      // Border
      pp.setPen(QPen(QColor(80, 80, 80), 1));
      pp.setBrush(Qt::NoBrush);
      pp.drawRoundedRect(pix.rect().adjusted(0, 0, -1, -1), RADIUS, RADIUS);
    }

    auto *lbl = new QLabel(swatch);
    lbl->setPixmap(pix);
    lbl->setGeometry(0, 0, SWATCH_W, STRIP_H - 4);
    lbl->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Click handler via event filter on swatch
    const std::vector<Stop> preset_stops = preset.stops;
    swatch->installEventFilter(this);
    swatch->setProperty("preset_stops_idx",
                        static_cast<int>(&preset - &presets_[0]));

    // We wire click via a mouse press event filter on each swatch.
    // Simpler: use a QPushButton with a custom flat style.
    // Rework: replace swatch QWidget with a flat QPushButton.
    // (done below — remove the QWidget approach)
    swatch->deleteLater();

    auto *btn = new QPushButton(preset_strip_);
    btn->setFixedSize(SWATCH_W, STRIP_H - 4);
    btn->setFlat(true);
    btn->setIcon(QIcon(pix));
    btn->setIconSize(QSize(SWATCH_W, STRIP_H - 4));
    btn->setToolTip(QString::fromStdString(preset.name));
    btn->setCursor(Qt::PointingHandCursor);

    connect(btn,
            &QPushButton::clicked,
            this,
            [this, preset_stops]()
            {
              stops_ = preset_stops;
              sort_stops();
              update();
              Q_EMIT value_changed();
              Q_EMIT edit_ended();
            });

    row->addWidget(btn);
  }

  row->addStretch();
  preset_strip_->setFixedWidth(
      std::max(10, int(presets_.size()) * (SWATCH_W + 4) + 8));
}

// ---------------------------------------------------------------------------
// Paint
// ---------------------------------------------------------------------------

void GradientPicker::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QRectF    br = bar_rect();
  const QPalette &pal = palette();

  // Checkerboard (alpha hint)
  if (false)
  {
    constexpr int cs = 6;
    const QColor  light(210, 210, 210), dark(160, 160, 160);
    for (int y = int(br.top()); y < int(br.bottom()); y += cs)
      for (int x = int(br.left()); x < int(br.right()); x += cs)
      {
        const bool lit = ((x / cs) + (y / cs)) % 2 == 0;
        p.fillRect(x, y, cs, cs, lit ? light : dark);
      }
  }

  // Gradient bar
  {
    QLinearGradient grad(br.topLeft(), br.topRight());
    for (const auto &s : stops_)
      grad.setColorAt(double(s.position), to_qcolor(s.color));

    p.setBrush(grad);
    p.setPen(QPen(pal.color(QPalette::Mid), 1));
    p.drawRoundedRect(br, RADIUS, RADIUS);
  }

  // Stop handles
  for (int i = 0; i < static_cast<int>(stops_.size()); ++i)
  {
    const QRectF r = stop_rect(stops_[i]);
    const bool   sel = (i == selected_idx_);

    // Small triangle pointing up from bar bottom to handle
    const float cx = float(r.center().x());
    const float ty = float(br.bottom());
    QPolygonF   tri;
    tri << QPointF(cx - 4, ty + 8) << QPointF(cx + 4, ty + 8)
        << QPointF(cx, ty + 1);
    p.setPen(Qt::NoPen);
    p.setBrush(pal.color(sel ? QPalette::Highlight : QPalette::Button));
    p.drawPolygon(tri);

    // Colour disc
    p.setBrush(to_qcolor(stops_[i].color));
    p.setPen(
        QPen(sel ? pal.color(QPalette::Highlight) : pal.color(QPalette::Dark),
             sel ? 2 : 1));
    p.drawEllipse(r);
  }
}

// ---------------------------------------------------------------------------
// Mouse
// ---------------------------------------------------------------------------

void GradientPicker::mouseDoubleClickEvent(QMouseEvent *e)
{
  const int idx = hit_test(e->pos());

  if (idx >= 0)
  {
    // Edit existing stop colour
    const QColor picked = QColorDialog::getColor(
        to_qcolor(stops_[idx].color),
        this,
        QString(),
        QColorDialog::ShowAlphaChannel | QColorDialog::DontUseNativeDialog);

    if (picked.isValid())
    {
      stops_[idx].color = from_qcolor(picked);
      update();
      Q_EMIT value_changed();
      Q_EMIT edit_ended();
    }
  }
  else if (bar_rect().contains(QPointF(e->pos())))
  {
    // Add new stop
    const double pos = std::clamp((e->pos().x() - bar_rect().left()) /
                                      bar_rect().width(),
                                  0.0,
                                  1.0);

    constexpr double eps = 1e-3;
    const bool       too_close = std::any_of(
        stops_.begin(),
        stops_.end(),
        [&](const Stop &s)
        { return std::abs(double(s.position) - pos) < eps; });

    if (!too_close)
    {
      stops_.push_back({float(pos), {1.f, 1.f, 1.f, 1.f}});
      sort_stops();
      selected_idx_ = hit_test(e->pos());
      update();
      Q_EMIT value_changed();
      Q_EMIT edit_ended();
    }
  }
}

void GradientPicker::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    selected_idx_ = hit_test(e->pos());
    dragging_ = selected_idx_ >= 0;
    update();
  }
}

void GradientPicker::mouseMoveEvent(QMouseEvent *e)
{
  if (!dragging_ || selected_idx_ < 0) return;

  const QRectF br = bar_rect();
  double pos = std::clamp((e->pos().x() - br.left()) / br.width(), 0.0, 1.0);

  // Maintain minimum gap to avoid coincident positions.
  constexpr double eps = 1e-3;
  for (int i = 0; i < static_cast<int>(stops_.size()); ++i)
  {
    if (i == selected_idx_) continue;
    if (std::abs(double(stops_[i].position) - pos) < eps)
      pos = pos < double(stops_[i].position) ? double(stops_[i].position) - eps
                                             : double(stops_[i].position) + eps;
    pos = std::clamp(pos, 0.0, 1.0);
  }

  stops_[selected_idx_].position = float(pos);
  sort_stops();
  update();
  Q_EMIT value_changed();
}

void GradientPicker::mouseReleaseEvent(QMouseEvent *)
{
  if (dragging_)
  {
    dragging_ = false;
    Q_EMIT edit_ended();
  }
}

void GradientPicker::contextMenuEvent(QContextMenuEvent *e)
{
  const int idx = hit_test(e->pos());
  if (idx < 0) return;

  // Only offer removal when at least 3 stops (keep minimum 2).
  if (static_cast<int>(stops_.size()) <= 2) return;

  QMenu    menu(this);
  QAction *rm = menu.addAction(QObject::tr("Remove stop"));

  if (menu.exec(e->globalPos()) == rm)
  {
    stops_.erase(stops_.begin() + idx);
    if (selected_idx_ == idx)
      selected_idx_ = -1;
    else if (selected_idx_ > idx)
      --selected_idx_;
    update();
    Q_EMIT value_changed();
    Q_EMIT edit_ended();
  }
}

QSize GradientPicker::sizeHint() const
{
  return {300, PAD + BAR_H + STOP_R * 2 + 6 + 4 + STRIP_H + 4 + 22 + 4};
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

QRectF GradientPicker::bar_rect() const
{
  return QRectF(PAD, PAD, width() - 2 * PAD, BAR_H);
}

QRectF GradientPicker::stop_rect(const Stop &s) const
{
  const QRectF br = bar_rect();
  const double cx = br.left() + double(s.position) * br.width();
  const double cy = br.bottom() + 3 + STOP_R;
  return QRectF(cx - STOP_R, cy - STOP_R, STOP_R * 2, STOP_R * 2);
}

int GradientPicker::hit_test(const QPoint &pos) const
{
  for (int i = 0; i < static_cast<int>(stops_.size()); ++i)
    if (stop_rect(stops_[i]).adjusted(-2, -2, 2, 2).contains(QPointF(pos)))
      return i;
  return -1;
}

void GradientPicker::sort_stops()
{
  std::sort(stops_.begin(),
            stops_.end(),
            [](const Stop &a, const Stop &b)
            { return a.position < b.position; });
}

} // namespace meta::qt
