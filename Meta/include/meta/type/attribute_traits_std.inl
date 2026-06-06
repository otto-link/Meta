/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file attribute_traits_std.inl
 * @brief AttributeTraits specializations for standard library string types.
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace meta
{

/**
 * @brief Traits specialization for std::filesystem::path serialization and
 * formatting.
 */
template <> struct AttributeTraits<std::filesystem::path>
{
  static std::string to_string(const std::filesystem::path &v)
  {
    return v.string();
  }

  static nlohmann::json json_to(const std::filesystem::path &v) { return v; }

  static std::filesystem::path json_from(const nlohmann::json &j)
  {
    return j.get<std::filesystem::path>();
  }
};

// ---------------------------
//  Generic vector trait
// ---------------------------

template <typename T> struct AttributeTraits<std::vector<T>>
{
  static std::string to_string(const std::vector<T> &v)
  {
    std::ostringstream oss;

    for (size_t i = 0; i < v.size(); ++i)
    {
      oss << AttributeTraits<T>::to_string(v[i]);
      if (i + 1 < v.size()) oss << ", ";
    }

    return oss.str();
  }

  static nlohmann::json json_to(const std::vector<T> &v)
  {
    return nlohmann::json(v);
  }

  static std::vector<T> json_from(const nlohmann::json &j)
  {
    return j.get<std::vector<T>>();
  }
};

} // namespace meta