/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

// --- CLASS

void MetaWidget::closeEvent(QCloseEvent *event)
{
  Q_EMIT closed();
  QWidget::closeEvent(event);
}

const std::function<void()> &MetaWidget::get_sync_from_model() const
{
  return sync_from_model;
}

void MetaWidget::set_sync_from_model(std::function<void()> callback)
{
  sync_from_model = std::move(callback);
}

// --- FUNCTIONS

QLabel *make_error_widget(const AbstractAttribute *p_attr,
                          const std::string       &msg,
                          QWidget                 *parent)
{
  std::string full_msg = "[ERROR] " + p_attr->name();
  if (!msg.empty()) full_msg += ", what: " + msg;
  return new QLabel(full_msg.c_str(), parent);
}

MetaWidget *make_meta_widget_grid(QWidget *parent)
{
  auto *widget = new MetaWidget(parent);
  auto *layout = new QGridLayout(widget);
  layout->setSpacing(1);
  layout->setContentsMargins(0, 0, 0, 0);
  return widget;
}

MetaWidget *make_meta_widget_hbox(QWidget *parent)
{
  auto *widget = new MetaWidget(parent);
  auto *layout = new QHBoxLayout(widget);
  layout->setSpacing(1);
  layout->setContentsMargins(0, 0, 0, 0);
  return widget;
}

MetaWidget *make_meta_widget_vbox(QWidget *parent)
{
  auto *widget = new MetaWidget(parent);
  auto *layout = new QVBoxLayout(widget);
  layout->setSpacing(1);
  layout->setContentsMargins(0, 0, 0, 0);
  return widget;
}

} // namespace meta::qt
