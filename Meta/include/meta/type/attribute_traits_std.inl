/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file attribute_traits.inl
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <format>
#include <string>

#include <nlohmann/json.hpp>

namespace meta
{

template <> struct AttributeTraits<std::string>
{
  static std::string to_string(const std::string &v) { return v; }

  static nlohmann::json json_to(const std::string &v) { return v; }

  static std::string json_from(const nlohmann::json &j)
  {
    return j.get<std::string>();
  }
};

template <> struct AttributeTraits<std::vector<std::string>>
{
  static std::string to_string(const std::vector<std::string> &v)
  {
    std::string result;
    for (size_t i = 0; i < v.size(); ++i)
    {
      result += v[i];
      if (i + 1 < v.size()) result += ", ";
    }
    return result;
  }

  static nlohmann::json json_to(const std::vector<std::string> &v)
  {
    return nlohmann::json(v);
  }

  static std::vector<std::string> json_from(const nlohmann::json &j)
  {
    return j.get<std::vector<std::string>>();
  }
};

} // namespace meta