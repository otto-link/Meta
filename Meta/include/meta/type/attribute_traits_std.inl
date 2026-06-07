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
//  Generic vector traits
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

template <typename T, typename U>
struct AttributeTraits<std::vector<std::pair<T, U>>>
{
  using value_type = std::vector<std::pair<T, U>>;

  static std::string to_string(const value_type &v)
  {
    std::ostringstream oss;

    for (size_t i = 0; i < v.size(); ++i)
    {
      oss << "(" << AttributeTraits<T>::to_string(v[i].first) << ": "
          << AttributeTraits<U>::to_string(v[i].second) << ")";

      if (i + 1 < v.size()) oss << ", ";
    }

    return oss.str();
  }

  static nlohmann::json json_to(const value_type &v)
  {
    nlohmann::json j = nlohmann::json::array();

    for (const auto &p : v)
    {
      j.push_back({{"value", p.first}, {"label", p.second}});
    }

    return j;
  }

  static value_type json_from(const nlohmann::json &j)
  {
    value_type result;

    if (!j.is_array()) return result;

    for (const auto &el : j)
    {
      if (!el.contains("value") || !el.contains("label")) continue;

      result.emplace_back(el["value"].get<T>(), el["label"].get<U>());
    }

    return result;
  }
};

// ---------------------------
//  Generic map trait
// ---------------------------

template <typename K, typename V> struct AttributeTraits<std::map<K, V>>
{
  static std::string to_string(const std::map<K, V> &m)
  {
    std::string result;

    for (auto it = m.begin(); it != m.end(); ++it)
    {
      result += AttributeTraits<K>::to_string(it->first);
      result += ": ";
      result += AttributeTraits<V>::to_string(it->second);

      auto next = it;
      ++next;
      if (next != m.end()) result += ", ";
    }

    return result;
  }

  static nlohmann::json json_to(const std::map<K, V> &m)
  {
    return m; // nlohmann supports map directly
  }

  static std::map<K, V> json_from(const nlohmann::json &j)
  {
    return j.get<std::map<K, V>>();
  }
};

// ---------------------------
//  Generic map trait
// ---------------------------

template <typename K, typename V>
struct AttributeTraits<std::unordered_map<K, V>>
{
  static std::string to_string(const std::unordered_map<K, V> &m)
  {
    std::string result;

    for (auto it = m.begin(); it != m.end(); ++it)
    {
      result += AttributeTraits<K>::to_string(it->first);
      result += ": ";
      result += AttributeTraits<V>::to_string(it->second);

      auto next = it;
      ++next;
      if (next != m.end()) result += ", ";
    }

    return result;
  }

  static nlohmann::json json_to(const std::unordered_map<K, V> &m) { return m; }

  static std::unordered_map<K, V> json_from(const nlohmann::json &j)
  {
    return j.get<std::unordered_map<K, V>>();
  }
};

} // namespace meta