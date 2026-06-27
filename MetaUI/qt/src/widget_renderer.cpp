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

  if (p_attr->type() == typeid(int))
  {
    auto &attr = static_cast<Attribute<int> &>(*p_attr);
    return WidgetRenderer<int>::render(attr, parent);
  }

  if (p_attr->type() == typeid(std::string))
  {
    auto &attr = static_cast<Attribute<std::string> &>(*p_attr);
    return WidgetRenderer<std::string>::render(attr, parent);
  }

#ifdef META_ENABLE_GLM_TYPES
  if (p_attr->type() == typeid(glm::ivec2))
  {
    auto &attr = static_cast<Attribute<glm::ivec2> &>(*p_attr);
    return WidgetRenderer<glm::ivec2>::render(attr, parent);
  }

  if (p_attr->type() == typeid(glm::vec2))
  {
    auto &attr = static_cast<Attribute<glm::vec2> &>(*p_attr);
    return WidgetRenderer<glm::vec2>::render(attr, parent);
  }

  if (p_attr->type() == typeid(glm::vec3))
  {
    auto &attr = static_cast<Attribute<glm::vec3> &>(*p_attr);
    return WidgetRenderer<glm::vec3>::render(attr, parent);
  }
#endif

  return nullptr;
}

} // namespace meta::qt
