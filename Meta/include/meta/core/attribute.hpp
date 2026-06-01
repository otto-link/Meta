/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file attribute.hpp
 * @brief Typed runtime attribute with reflection, serialization, and metadata.
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <any>
#include <string>
#include <typeinfo>

#include <nlohmann/json.hpp>

#include "meta/core/abstract_attribute.hpp"
#include "meta/type/attribute_traits.hpp"
#include "meta/type/type_name.hpp"

namespace meta
{

// -----------------------------------------------------------------------------
// Forward declarations (metadata coupling avoidance)
// -----------------------------------------------------------------------------

class AttributeContainer;

/**
 * @brief Serialize attribute metadata to JSON.
 */
nlohmann::json serialize_metadata(const AttributeContainer &m);

/**
 * @brief Deserialize attribute metadata from JSON.
 */
void deserialize_metadata(AttributeContainer &m, const nlohmann::json &j);

// -----------------------------------------------------------------------------
// Attribute
// -----------------------------------------------------------------------------

/**
 * @brief Typed runtime attribute with reflection support.
 *
 * Stores a strongly typed value with runtime introspection, conversion,
 * and optional metadata.
 *
 * @tparam T Stored value type.
 */
template <typename T> class Attribute : public AbstractAttribute
{
public:
  /**
   * @brief Construct an attribute with a name and initial value.
   *
   * @param name Attribute identifier.
   * @param value Initial stored value.
   */
  Attribute(std::string name, T value)
      : _name(std::move(name)), _value(std::move(value))
  {
  }

  /**
   * @brief Get attribute name.
   */
  const std::string &name() const override { return _name; }

  /**
   * @brief Get runtime type information of stored value.
   */
  std::type_index type() const override { return typeid(T); }

  /**
   * @brief Get mutable pointer to stored value.
   */
  void *raw_ptr() override { return &_value; }

  /**
   * @brief Get const pointer to stored value.
   */
  const void *raw_ptr() const override { return &_value; }

  /**
   * @brief Assign value from type-erased container.
   *
   * @param value Input value wrapped in std::any.
   * @return true if type matches and assignment succeeded.
   */
  bool set_from_any(const std::any &value) override
  {
    if (value.type() != typeid(T)) return false;

    _value = std::any_cast<T>(value);
    return true;
  }

  /**
   * @brief Convert value to std::any.
   */
  std::any to_any() const override { return _value; }

  /**
   * @brief Access mutable value.
   */
  T &value() { return _value; }

  /**
   * @brief Access const value.
   */
  const T &value() const { return _value; }

  /**
   * @brief Convert value to human-readable string.
   */
  std::string to_string() const override
  {
    return AttributeTraits<T>::to_string(_value);
  }

  /**
   * @brief Serialize attribute to JSON.
   *
   * Includes:
   * - type identifier
   * - value serialization
   * - metadata container
   */
  nlohmann::json json_to() const override
  {
    return {{"type", TypeName<T>::name},
            {"value", AttributeTraits<T>::json_to(_value)},
            {"metadata", serialize_metadata(metadata())}};
  }

  /**
   * @brief Deserialize attribute from JSON.
   *
   * Restores:
   * - value
   * - metadata container
   */
  void json_from(const nlohmann::json &j) override
  {
    _value = AttributeTraits<T>::json_from(j.at("value"));
    deserialize_metadata(metadata(), j.at("metadata"));
  }

private:
  std::string _name;
  T           _value;
};

} // namespace meta