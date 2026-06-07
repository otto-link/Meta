/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <cfloat>
#include <limits>
#include <string>

#include "meta/core/attribute.hpp"
#include "meta/core/attribute_container.hpp"

#define META_DEFAULT_FORMAT "{}"
#define META_DEFAULT_GROUP ""
#define META_ROOT_GROUP "Settings"
#define META_DEFAULT_GROUP_POLICY "flat"

namespace meta::keys::constraints
{

inline constexpr char allowed_values[] = "constraints.allowed_values";
inline constexpr char enum_items[] = "constraints.enum_items";
inline constexpr char min[] = "constraints.min";
inline constexpr char max[] = "constraints.max";
inline constexpr char step[] = "constraints.step";

} // namespace meta::keys::constraints

namespace meta::keys::ui
{

inline constexpr char format[] = "ui.format";
inline constexpr char group[] = "ui.group";
inline constexpr char label[] = "ui.label";
inline constexpr char label_true[] = "ui.label_true";
inline constexpr char label_false[] = "ui.label_false";
inline constexpr char widget_type[] = "ui.widget_type";

} // namespace meta::keys::ui

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

template <typename T, typename V>
std::vector<std::pair<T, std::string>> enum_items(const Attribute<V> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::enum_items);
  if (!m) return {};

  return std::any_cast<std::vector<std::pair<T, std::string>>>(m->to_any());
}

template <typename T> T min(const Attribute<T> &attr)
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

template <typename T> T step(const Attribute<T> &attr)
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

template <typename T> std::string group(const Attribute<T> &attr)
{
  return try_get<std::string>(attr, meta::keys::ui::group, META_DEFAULT_GROUP);
}

inline std::string group(const AbstractAttribute &attr)
{
  return try_get<std::string>(attr, meta::keys::ui::group, META_DEFAULT_GROUP);
}

template <typename T> std::string widget_type(const Attribute<T> &attr)
{
  return try_get<std::string>(attr, meta::keys::ui::widget_type, "UNDEFINED");
}

} // namespace meta::common
