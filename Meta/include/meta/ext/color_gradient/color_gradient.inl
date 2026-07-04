/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <string>

#include "meta/type/attribute_traits.hpp"
#include "meta/type/type_name.hpp"

// all registrations in one place for extensions

namespace meta
{

/// Typename for ColorGradient.
template <> struct TypeName<meta::ColorGradient>
{
  static constexpr std::string_view name = "meta::ColorGradient";
};

/// Traits specialization for ColorGradient serialization and formatting.
template <> struct AttributeTraits<ColorGradient>
{
  static std::string to_string(const ColorGradient &v)
  {
    std::ostringstream oss;

    for (auto &a : v.value())
    {
      oss << "(";
      oss << std::to_string(a.position) + ", ";
      oss << std::to_string(a.color[0]) + ", ";
      oss << std::to_string(a.color[1]) + ", ";
      oss << std::to_string(a.color[2]) + ", ";
      oss << std::to_string(a.color[3]);
      oss << ")\n";
    }

    return oss.str();
  }

  static nlohmann::json json_to(const ColorGradient &v) { return v.json_to(); }

  static ColorGradient json_from(const nlohmann::json &j)
  {
    ColorGradient v;
    v.json_from(j);
    return v;
  }
};

} // namespace meta