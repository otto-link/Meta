/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QLabel>
#include <QWidget>

#include "meta/core/abstract_attribute.hpp"

namespace meta::qt
{

class MetaWidget : public QWidget
{
  Q_OBJECT

public:
  MetaWidget(QWidget *parent = nullptr) : QWidget(parent) {}

signals:
  void closed();
  void edit_started();
  void edit_ended();
  void value_changed();

protected:
  void closeEvent(QCloseEvent *event) override;
};

MetaWidget *make_meta_widget_grid(QWidget *parent = nullptr);
MetaWidget *make_meta_widget_hbox(QWidget *parent = nullptr);
MetaWidget *make_meta_widget_vbox(QWidget *parent = nullptr);

QLabel *make_error_widget(const AbstractAttribute *p_attr,
                          const std::string       &msg = "",
                          QWidget                 *parent = nullptr);

} // namespace meta::qt