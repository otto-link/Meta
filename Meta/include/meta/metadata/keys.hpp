/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

namespace meta::keys::constraints
{

inline constexpr char allowed_values[] = "constraints.allowed_values";
inline constexpr char aspect_ratio[] = "constraints.aspect_ratio";
inline constexpr char enum_items[] = "constraints.enum_items";
inline constexpr char file_filter[] = "constraints.file_filter";
inline constexpr char min[] = "constraints.min";
inline constexpr char max[] = "constraints.max";
inline constexpr char power_of_two[] = "constraints.power_of_two";
inline constexpr char step[] = "constraints.step";

} // namespace meta::keys::constraints

namespace meta::keys::ui
{

inline constexpr char format[] = "ui.format";
inline constexpr char category[] = "ui.category";
inline constexpr char label[] = "ui.label";
inline constexpr char label_true[] = "ui.label_true";
inline constexpr char label_false[] = "ui.label_false";
inline constexpr char widget_type[] = "ui.widget_type";

} // namespace meta::keys::ui