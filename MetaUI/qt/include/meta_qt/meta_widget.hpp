/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QHBoxLayout>
#include <QWidget>

namespace meta::qt
{

class MetaWidget : public QWidget
{
  Q_OBJECT

public:
  MetaWidget(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void value_changed();
};

inline MetaWidget *make_meta_widget(QWidget *parent = nullptr)
{
  auto *widget = new MetaWidget(parent);
  auto *layout = new QHBoxLayout(widget);
  layout->setSpacing(1);
  layout->setContentsMargins(0, 0, 0, 0);
  return widget;
}

} // namespace meta::qt