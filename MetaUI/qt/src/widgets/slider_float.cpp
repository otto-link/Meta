/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <algorithm>
#include <format>
#include <random>

#include <QApplication>
#include <QFontMetrics>
#include <QHoverEvent>
#include <QMenu>
#include <QPainter>

#include "meta_qt/widgets/helpers.hpp"
#include "meta_qt/widgets/slider_float.hpp"

namespace meta::qt
{

SliderFloat::SliderFloat(const std::string &label_,
                         float              value_init_,
                         float              vmin_,
                         float              vmax_,
                         bool               add_plus_minus_buttons_,
                         const std::string &value_format_,
                         QWidget           *parent)
    : QWidget(parent),
      value_init(value_init_),
      value(value_init_),
      vmin(vmin_),
      vmax(vmax_),
      add_plus_minus_buttons(add_plus_minus_buttons_),
      value_format(value_format_)
{
  this->label = helpers::truncate_string(label_, MAX_LABEL_LEN);

  this->setMouseTracking(true);
  this->setAttribute(Qt::WA_Hover);
  this->setContextMenuPolicy(Qt::CustomContextMenu);

  this->update_geometry();
  this->connect(this,
                &SliderFloat::edit_ended,
                [this]() { this->update_geometry(); });

  // Derive stylesheet colours from the current palette so we respect dark/light
  // themes.
  const QPalette &pal = this->palette();
  this->style_sheet = "background-color: " +
                      pal.color(QPalette::Base).name().toStdString() +
                      "; color: " +
                      pal.color(QPalette::Text).name().toStdString() +
                      "; border: 0px;"
                      " selection-background-color: #ABABAB;";

  this->value_edit = new QLineEdit(this);
  this->value_edit->setVisible(false);
  this->value_edit->setFixedHeight(this->height() - 2);
  this->value_edit->setAlignment(Qt::AlignCenter);
  this->value_edit->setStyleSheet(this->style_sheet.c_str());
  this->connect(this->value_edit,
                &QLineEdit::editingFinished,
                this,
                &SliderFloat::apply_text_edit_value);
}

void SliderFloat::apply_text_edit_value()
{
  bool  ok = false;
  float new_value = this->value_edit->text().toFloat(&ok);
  if (ok && this->set_value(new_value)) Q_EMIT this->edit_ended();

  this->value_edit->setVisible(false);
  this->update();
}

bool SliderFloat::event(QEvent *event)
{
  switch (event->type())
  {
  case QEvent::HoverEnter:
    this->is_hovered = true;
    this->update();
    this->setCursor(Qt::SizeHorCursor);
    break;

  case QEvent::HoverLeave:
    this->is_hovered = false;
    this->is_minus_hovered = false;
    this->is_plus_hovered = false;
    this->is_bar_hovered = false;
    this->update();
    this->setCursor(Qt::ArrowCursor);
    break;

  case QEvent::HoverMove:
  {
    auto        *hover = static_cast<QHoverEvent *>(event);
    const QPoint pos = hover->position().toPoint();
    this->is_minus_hovered = this->rect_minus.contains(pos);
    this->is_plus_hovered = this->rect_plus.contains(pos);
    this->is_bar_hovered = this->rect_bar.contains(pos);
    this->update();
    this->setCursor(this->is_bar_hovered ? Qt::SizeHorCursor : Qt::ArrowCursor);
    break;
  }

  default: break;
  }
  return QWidget::event(event);
}

float SliderFloat::get_value() const { return this->value; }

std::string SliderFloat::get_value_as_string() const
{
  try
  {
    return std::vformat(this->value_format, std::make_format_args(this->value));
  }
  catch (...)
  {
    return std::to_string(this->value);
  }
}

void SliderFloat::mouseDoubleClickEvent(QMouseEvent *)
{
  if (this->is_bar_hovered)
  {
    this->value_edit->setText(QString::number(this->value));
    this->value_edit->setGeometry(this->rect_bar.adjusted(1, 1, -1, -1));
    this->value_edit->setVisible(true);
    this->value_edit->setFocus();
    this->value_edit->selectAll();
    this->update();
  }
}

void SliderFloat::mouseMoveEvent(QMouseEvent *event)
{
  if (!this->is_dragging)
  {
    QWidget::mouseMoveEvent(event);
    return;
  }

  float      ppu;
  const bool is_bounded = this->vmin != -FLT_MAX && this->vmax != FLT_MAX;

  if (!is_bounded || this->vmin == this->vmax)
    ppu = PPU_F;
  else
    ppu = float(this->rect_bar.width()) / (this->vmax - this->vmin);

  const Qt::KeyboardModifiers mods = event->modifiers();
  this->force_edit_ended_emit = false;

  if ((mods & Qt::ControlModifier) && (mods & Qt::ShiftModifier))
    this->force_edit_ended_emit = true;
  else if (mods & Qt::ControlModifier)
    ppu *= PPU_MULT_FINE;
  else if (mods & Qt::ShiftModifier)
    ppu /= PPU_MULT_FINE;

  const int dx = event->position().toPoint().x() - this->pos_x_before_dragging;
  const float dv = float(dx) / ppu;
  this->set_value(this->value_before_dragging + dv);

  QWidget::mouseMoveEvent(event);
}

void SliderFloat::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    const bool  is_bounded = this->vmin != -FLT_MAX && this->vmax != FLT_MAX;
    const float delta = is_bounded
                            ? (this->vmax - this->vmin) / float(BUTTON_TICKS)
                            : 1.f;

    if (this->is_bar_hovered)
    {
      this->value_before_dragging = this->value;
      this->pos_x_before_dragging = event->position().toPoint().x();
      this->set_is_dragging(true);
    }
    else if (this->is_minus_hovered)
    {
      if (this->set_value(this->value - delta)) Q_EMIT this->edit_ended();
    }
    else if (this->is_plus_hovered)
    {
      if (this->set_value(this->value + delta)) Q_EMIT this->edit_ended();
    }
  }
}

void SliderFloat::mouseReleaseEvent(QMouseEvent *event)
{
  if (this->is_dragging && event->button() == Qt::LeftButton)
  {
    this->set_is_dragging(false);
    if (this->value != this->value_before_dragging) Q_EMIT this->edit_ended();
  }
}

void SliderFloat::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);

  const QPalette &pal = this->palette();
  const QColor    c_bg = pal.color(QPalette::Base);
  const QColor    c_fill = pal.color(QPalette::Highlight);
  const QColor    c_text = pal.color(QPalette::Text);
  const QColor    c_border = pal.color(QPalette::Mid);
  const QColor    c_hover = pal.color(QPalette::Highlight);

  // Background + border
  p.setBrush(c_bg);
  p.setPen(QPen(this->is_hovered ? c_hover : c_border,
                this->is_hovered ? HOVER_BORDER_WIDTH : BORDER_WIDTH));
  p.drawRoundedRect(this->rect(), RADIUS, RADIUS);

  // Value fill bar
  const bool is_bounded = this->vmin != -FLT_MAX && this->vmax != FLT_MAX;

  if (is_bounded && !this->value_edit->isVisible())
  {
    const float range = this->vmax - this->vmin;
    if (range > 0.f)
    {
      const float r = (this->value - this->vmin) / range;
      const int   rcut = int((1.f - r) * float(this->rect_bar.width()));

      p.setBrush(c_fill.darker(130));
      p.setPen(Qt::NoPen);

      if (this->add_plus_minus_buttons)
        p.drawRect(this->rect_bar.adjusted(1, 1, -rcut - 1, -1));
      else
        p.drawRoundedRect(this->rect_bar.adjusted(1, 1, -rcut - 1, -1),
                          RADIUS,
                          RADIUS);
    }
  }

  // +/- button separators
  if (this->add_plus_minus_buttons)
  {
    p.setPen(QPen(c_border, BORDER_WIDTH));
    p.drawLine(QPoint(this->rect_minus.right() + 1, this->rect().top()),
               QPoint(this->rect_minus.right() + 1, this->rect().bottom()));
    p.drawLine(QPoint(this->rect_plus.left() - 1, this->rect().top()),
               QPoint(this->rect_plus.left() - 1, this->rect().bottom()));
  }

  // Label (left) and value (right) inside the bar
  p.setBrush(c_text);
  p.setPen(c_text);

  const QRect label_rect = this->rect_bar.adjusted(this->base_dx,
                                                   0,
                                                   -this->base_dx,
                                                   0);
  p.drawText(label_rect,
             Qt::AlignLeft | Qt::AlignVCenter,
             QString::fromStdString(this->label));
  p.drawText(label_rect,
             Qt::AlignRight | Qt::AlignVCenter,
             QString::fromStdString(this->get_value_as_string()));

  // ◁/▶ arrows
  p.drawText(this->rect_minus,
             Qt::AlignCenter | Qt::AlignVCenter,
             this->is_minus_hovered ? "◀" : "◁");
  p.drawText(this->rect_plus,
             Qt::AlignCenter | Qt::AlignVCenter,
             this->is_plus_hovered ? "▶" : "▷");
}

void SliderFloat::resizeEvent(QResizeEvent *event)
{
  this->update_geometry();
  QWidget::resizeEvent(event);
}

bool SliderFloat::set_value(float new_value)
{
  new_value = std::clamp(new_value, this->vmin, this->vmax);

  if (new_value == this->value) return false;

  this->value = new_value;
  this->update();
  Q_EMIT this->value_changed();

  if (this->force_edit_ended_emit) Q_EMIT this->edit_ended();

  return true;
}

// ---------------------------------------------------------------------------
// Private slots / helpers
// ---------------------------------------------------------------------------

void SliderFloat::set_is_dragging(bool new_state)
{
  this->is_dragging = new_state;
  this->setCursor(new_state ? Qt::SizeHorCursor : Qt::ArrowCursor);
}

QSize SliderFloat::sizeHint() const
{
  return QSize(this->slider_width, this->base_dy);
}

void SliderFloat::update_geometry()
{
  QFontMetrics fm(this->font());
  this->base_dx = fm.horizontalAdvance(QString("M"));
  this->base_dy = fm.height() + PADDING_V;

  const int label_w = helpers::text_width(this, this->label);
  this->slider_width = label_w + PADDING_MIDDLE +
                       10 * fm.horizontalAdvance(QString("0")) +
                       6 * this->base_dx;

  this->slider_width_min = label_w + PADDING_MIDDLE +
                           fm.horizontalAdvance(QString::fromStdString(
                               this->get_value_as_string())) +
                           6 * this->base_dx;

  this->setMinimumWidth(this->slider_width_min);
  this->setMinimumHeight(this->sizeHint().height());
  this->setMaximumHeight(this->sizeHint().height());

  if (this->add_plus_minus_buttons)
  {
    this->rect_minus = this->rect();
    this->rect_minus.setWidth(2 * this->base_dx);

    this->rect_plus = this->rect().adjusted(this->rect().width() -
                                                2 * this->base_dx,
                                            0,
                                            0,
                                            0);
  }
  else
  {
    this->rect_minus = QRect();
    this->rect_plus = QRect();
  }

  const int gap = this->add_plus_minus_buttons ? 2 * this->base_dx : 0;
  this->rect_bar = this->rect().adjusted(gap, 0, -gap, 0);
}

} // namespace meta::qt
