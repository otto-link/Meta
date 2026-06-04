/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QLabel>
#include <QLayout>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

template <typename T> struct WidgetRenderer
{
  static MetaWidget *render(Attribute<T> &attr, QWidget *parent)
  {
    std::string msg;
    msg.reserve(128);
    msg += "Unsupported type: ";
    msg += TypeName<T>::name;
    msg += ", ";
    msg += attr.name();

    MetaWidget *widget = make_meta_widget_hbox(parent);
    auto       *layout = widget->layout();

    QLabel *label = new QLabel(msg.c_str(), widget);
    layout->addWidget(label);

    return widget;
  }
};

// helper
template <typename T>
MetaWidget *render(Attribute<T> &attr, QWidget *parent = nullptr)
{
  return WidgetRenderer<T>::render(attr, parent);
}

MetaWidget *render(AbstractAttribute *p_attr, QWidget *parent = nullptr);

} // namespace meta::qt

#include "widget_renderer_inl/bool.inl"
#include "widget_renderer_inl/float.inl"