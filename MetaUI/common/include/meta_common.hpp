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
#define META_DEFAULT_MIN std::numeric_limits<float>::lowest()
#define META_DEFAULT_MAX std::numeric_limits<float>::max()
#define META_DEFAULT_STEP 0.1

namespace meta::keys::constraints
{

inline constexpr char allowed_values[] = "constraints.allowed_values";
inline constexpr char min[] = "constraints.min";
inline constexpr char max[] = "constraints.max";
inline constexpr char step[] = "constraints.step";

} // namespace meta::keys::constraints

namespace meta::keys::ui
{

inline constexpr char format[] = "ui.format";
inline constexpr char label[] = "ui.label";
inline constexpr char label_true[] = "ui.label_true";
inline constexpr char label_false[] = "ui.label_false";
inline constexpr char widget_type[] = "ui.widget_type";

} // namespace meta::keys::ui

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

int try_get_format_decimals(const std::string &format,
                            int                default_decimals = 2);

// --- Contraints

template <typename T> T min(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::min);
  if (!m) return META_DEFAULT_MIN;
  return std::any_cast<T>(m->to_any());
}

template <typename T> std::vector<T> allowed_values(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::allowed_values);
  if (!m) return {};
  return std::any_cast<std::vector<T>>(m->to_any());
}
template <typename T> T max(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::max);
  if (!m) return META_DEFAULT_MAX;
  return std::any_cast<T>(m->to_any());
}

template <typename T> T step(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::constraints::step);
  if (!m) return META_DEFAULT_STEP;
  return std::any_cast<T>(m->to_any());
}

// --- UI

template <typename T> std::string label(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::ui::label);
  if (!m) return attr.name();
  return std::any_cast<std::string>(m->to_any());
}

template <typename T> std::string format(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::ui::format);
  if (!m) return META_DEFAULT_FORMAT;
  return std::any_cast<std::string>(m->to_any());
}

template <typename T> std::string widget_type(const Attribute<T> &attr)
{
  auto *m = attr.metadata().find(meta::keys::ui::widget_type);
  if (!m) return "UNDEFINED";
  return std::any_cast<std::string>(m->to_any());
}

} // namespace meta::common
