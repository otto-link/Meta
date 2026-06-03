/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <cfloat>
#include <string>

#include "meta/core/attribute.hpp"
#include "meta/core/attribute_container.hpp"

namespace meta::common
{

// --- Generic

template <typename T>
std::string try_get_string(const Attribute<T> &attr,
                           const std::string  &key,
                           const std::string  &default_value = "")
{
  auto *m = attr.metadata().find(key);
  if (!m) return default_value;
  return std::any_cast<std::string>(m->to_any());
}

// --- Specialized

template <typename T> std::string label(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find("ui.label");
  if (!m) return attr.name();
  return std::any_cast<std::string>(m->to_any());
}

template <typename T> float min(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find("ui.min");
  if (!m) return -FLT_MAX;
  return std::any_cast<float>(m->to_any());
}

template <typename T> float max(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find("ui.max");
  if (!m) return FLT_MAX;
  return std::any_cast<float>(m->to_any());
}

template <typename T> float step(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find("ui.step");
  if (!m) return 1.f;
  return std::any_cast<float>(m->to_any());
}

template <typename T> std::string widget_type(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find("ui.widget_type");
  if (!m) return attr.name();
  return std::any_cast<std::string>(m->to_any());
}

} // namespace meta::common
