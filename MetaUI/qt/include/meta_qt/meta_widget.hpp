/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
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

MetaWidget *make_meta_widget_grid(QWidget *parent = nullptr);
MetaWidget *make_meta_widget_hbox(QWidget *parent = nullptr);
MetaWidget *make_meta_widget_vbox(QWidget *parent = nullptr);

} // namespace meta::qt