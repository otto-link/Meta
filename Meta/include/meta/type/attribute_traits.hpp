/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file attribute_traits.hpp
 * @brief Generic traits for attribute value conversion and serialization.
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <string>

#include <nlohmann/json.hpp>

namespace meta
{

/**
 * @brief Default traits for attribute value conversion.
 *
 * Provides generic conversions between:
 * - C++ values
 * - std::string representation
 * - JSON serialization
 *
 * Specializations should be provided for complex or non-trivial types.
 *
 * @tparam T Value type handled by the attribute system.
 */
template <typename T> struct AttributeTraits
{
  /**
   * @brief Convert value to human-readable string.
   */
  static std::string to_string(const T &value) { return std::to_string(value); }

  /**
   * @brief Convert value to JSON representation.
   */
  static nlohmann::json json_to(const T &value) { return value; }

  /**
   * @brief Convert JSON back to value.
   */
  static T json_from(const nlohmann::json &j) { return j.get<T>(); }
};

} // namespace meta

// -----------------------------------------------------------------------------
// Specializations / extensions
// -----------------------------------------------------------------------------

#include "meta/type/attribute_traits_base.inl"

#ifdef META_ENABLE_STD_TYPES
#include "meta/type/attribute_traits_std.inl"
#endif

#ifdef META_ENABLE_GLM_TYPES
#include "meta/type/attribute_traits_glm.inl"
#endif