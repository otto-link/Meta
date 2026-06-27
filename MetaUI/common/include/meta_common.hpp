/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <cfloat>
#include <limits>
#include <string>

#include "meta/core/attribute.hpp"
#include "meta/core/attribute_container.hpp"
#include "meta/metadata/keys.hpp"

#define META_DEFAULT_FORMAT "{}"
#define META_DEFAULT_CATEGORY ""
#define META_ROOT_CATEGORY "Settings"
#define META_DEFAULT_CATEGORY_POLICY "flat"

namespace meta::common
{

// --- Generic

template <typename T, typename V>
T try_get(const Attribute<V> &attr,
          const std::string  &key,
          const T            &default_value)
{
  auto *m = attr.metadata().find(key);
  if (!m) return default_value;
  return std::any_cast<T>(m->to_any());
}

template <typename T>
T try_get(const AbstractAttribute &attr,
          const std::string       &key,
          const T                 &default_value)
{
  auto *m = attr.metadata().find(key);
  if (!m) return default_value;
  return std::any_cast<T>(m->to_any());
}

int try_get_format_decimals(const std::string &format,
                            int                default_decimals = 2);

// --- Contraints

template <typename T> std::vector<T> allowed_values(const Attribute<T> &attr)
{
  return try_get<std::vector<T>>(attr,
                                 meta::keys::constraints::allowed_values,
                                 {});
}

template <typename V> float aspect_ratio(const Attribute<V> &attr)
{
  return try_get<float>(attr, meta::keys::constraints::aspect_ratio, 0.f);
}

template <typename T, typename V>
std::vector<std::pair<T, std::string>> enum_items(const Attribute<V> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::enum_items);
  if (!m) return {};

  return std::any_cast<std::vector<std::pair<T, std::string>>>(m->to_any());
}

inline std::string file_filter(const AbstractAttribute &attr)
{
  return try_get<std::string>(attr, meta::keys::constraints::file_filter, "*");
}

template <typename T> T min(const Attribute<T> &attr)
{
  return try_get<T>(attr,
                    meta::keys::constraints::min,
                    std::numeric_limits<T>::lowest());
}

template <typename T, typename V> T min(const Attribute<V> &attr)
{
  return try_get<T>(attr,
                    meta::keys::constraints::min,
                    std::numeric_limits<T>::lowest());
}

template <typename T> T max(const Attribute<T> &attr)
{
  return try_get<T>(attr,
                    meta::keys::constraints::max,
                    std::numeric_limits<T>::max());
}

template <typename T, typename V> T max(const Attribute<V> &attr)
{
  return try_get<T>(attr,
                    meta::keys::constraints::max,
                    std::numeric_limits<T>::max());
}

template <typename T, typename V> T power_of_two(const Attribute<V> &attr)
{
  return try_get<T>(attr,
                    meta::keys::constraints::power_of_two,
                    static_cast<T>(false));
}

template <typename T> T step(const Attribute<T> &attr)
{
  return try_get<T>(attr, meta::keys::constraints::step, static_cast<T>(1));
}

inline std::string start_dir(const AbstractAttribute &attr)
{
  return try_get<std::string>(attr, meta::keys::constraints::start_dir, "");
}

template <typename T, typename V> T step(const Attribute<V> &attr)
{
  return try_get<T>(attr, meta::keys::constraints::step, static_cast<T>(1));
}

// --- UI / Base widgets

template <typename T> std::string label(const Attribute<T> &attr)
{
  return try_get<std::string>(attr, meta::keys::ui::label, attr.name());
}

template <typename T> std::string format(const Attribute<T> &attr)
{
  return try_get<std::string>(attr,
                              meta::keys::ui::format,
                              META_DEFAULT_FORMAT);
}

template <typename T> std::string category(const Attribute<T> &attr)
{
  return try_get<std::string>(attr,
                              meta::keys::ui::category,
                              META_DEFAULT_CATEGORY);
}

inline std::string category(const AbstractAttribute &attr)
{
  return try_get<std::string>(attr,
                              meta::keys::ui::category,
                              META_DEFAULT_CATEGORY);
}

template <typename T> std::string widget_type(const Attribute<T> &attr)
{
  return try_get<std::string>(attr, meta::keys::ui::widget_type, std::string{});
}

} // namespace meta::common
