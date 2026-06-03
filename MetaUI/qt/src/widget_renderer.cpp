/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta_qt/widget_renderer.hpp"

namespace meta::qt
{

MetaWidget *render(AbstractAttribute *p_attr, QWidget *parent)
{
  if (!p_attr) return nullptr;

  if (p_attr->type() == typeid(bool))
  {
    auto &attr = static_cast<Attribute<bool> &>(*p_attr);
    return WidgetRenderer<bool>::render(attr, parent);
  }

  if (p_attr->type() == typeid(float))
  {
    auto &attr = static_cast<Attribute<float> &>(*p_attr);
    return WidgetRenderer<float>::render(attr, parent);
  }

  return nullptr;
}

} // namespace meta::qt
