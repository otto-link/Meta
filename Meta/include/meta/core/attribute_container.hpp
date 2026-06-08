/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file attribute_container.hpp
 * @brief Container for named runtime attributes with lookup and JSON support.
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <nlohmann/json.hpp>

#include "meta/core/attribute.hpp"

#include <iostream>

namespace meta
{

/**
 * @brief Internal storage type for attributes.
 *
 * Maps attribute names to polymorphic attribute instances.
 */
using AttrContainerType =
    std::unordered_map<std::string, std::unique_ptr<AbstractAttribute>>;

/**
 * @brief Mutable iterator over attributes.
 */
using AttrIterator = AttrContainerType::iterator;

/**
 * @brief Const iterator over attributes.
 */
using ConstAttrIterator = AttrContainerType::const_iterator;

template <typename T>
concept StringLike = std::is_same_v<std::decay_t<T>, std::string> ||
                     std::is_same_v<std::decay_t<T>, const char *> ||
                     std::is_same_v<std::decay_t<T>, char *> ||
                     std::is_same_v<std::decay_t<T>, std::string_view>;

/**
 * @brief Runtime container of named attributes.
 *
 * Provides:
 * - ownership of attributes
 * - fast lookup by name
 * - iteration support
 * - JSON serialization / deserialization
 */
class AttributeContainer
{
public:
  /**
   * @brief Create and insert a new attribute.
   *
   * The value is forwarded and stored as `Attribute<std::decay_t<T>>`.
   *
   * @tparam T Input value type.
   * @param name Attribute identifier (must be unique).
   * @param value Initial value.
   * @return Pointer to the created attribute.
   *
   * @throws std::runtime_error if an attribute with the same name exists.
   */
  template <typename T>
    requires(!StringLike<T>)
  Attribute<std::decay_t<T>> *add(const std::string &name, T &&value)
  {
    using ValueType = std::decay_t<T>;

    auto attr = std::make_unique<Attribute<ValueType>>(name,
                                                       std::forward<T>(value));

    auto *ptr = attr.get();

    auto [it, inserted] = _attributes.try_emplace(name, std::move(attr));

    if (!inserted)
      throw std::runtime_error("Attribute already exists: " + name);

    return ptr;
  }

  // to avoid issues with "char const*"
  Attribute<std::decay_t<std::string>> *add(const std::string &name,
                                            std::string      &&value)
  {
    using ValueType = std::decay_t<std::string>;

    auto attr = std::make_unique<Attribute<ValueType>>(
        name,
        std::forward<std::string>(value));

    auto *ptr = attr.get();

    auto [it, inserted] = _attributes.try_emplace(name, std::move(attr));

    if (!inserted)
      throw std::runtime_error("Attribute already exists: " + name);

    return ptr;
  }

  /**
   * @brief Find attribute by name.
   * @param name Attribute identifier.
   * @return Pointer to attribute or nullptr if not found.
   */
  AbstractAttribute *find(const std::string &name);

  /**
   * @brief Find attribute by name (const version).
   */
  const AbstractAttribute *find(const std::string &name) const;

  /**
   * @brief Returns a pointer to the attribute value if it exists and has the
   * requested type.
   * @return Pointer to the value, or nullptr if the attribute is missing or has
   * an incompatible type.
   */
  template <typename T> T *try_value(const std::string &name)
  {
    auto *attr = find(name);

    if (!attr) return nullptr;

    auto *typed = attr->try_cast<Attribute<T>>();
    return typed ? &typed->value() : nullptr;
  }

  /**
   * @brief Returns a pointer to the attribute value if it exists and has the
   * requested type.
   * @return Pointer to the value, or nullptr if the attribute is missing or has
   * an incompatible type.
   */
  template <typename T> const T *try_value(const std::string &name) const
  {
    auto *attr = find(name);

    if (!attr) return nullptr;

    auto *typed = attr->try_cast<Attribute<T>>();
    return typed ? &typed->value() : nullptr;
  }

  /**
   * @brief Returns the attribute value.
   * @throws std::out_of_range if the attribute does not exist.
   * @throws std::runtime_error if the attribute type is incompatible.
   */
  template <typename T> T &value(const std::string &name)
  {
    if (auto *ptr = try_value<T>(name)) return *ptr;

    auto *attr = find(name);

    if (!attr) throw std::out_of_range("Attribute does not exist: " + name);

    throw std::runtime_error("Attribute '" + name + "' has type '" +
                             std::string(attr->type().name()) +
                             "', expected '" + std::string(typeid(T).name()) +
                             "'");
  }

  /**
   * @brief Returns the attribute value.
   * @throws std::out_of_range if the attribute does not exist.
   * @throws std::runtime_error if the attribute type is incompatible.
   */
  template <typename T> const T &value(const std::string &name) const
  {
    if (auto *ptr = try_value<T>(name)) return *ptr;

    auto *attr = find(name);

    if (!attr) throw std::out_of_range("Attribute does not exist: " + name);

    throw std::runtime_error("Attribute '" + name + "' has type '" +
                             std::string(attr->type().name()) +
                             "', expected '" + std::string(typeid(T).name()) +
                             "'");
  }

  // -------------------------------------------------------------------------
  // Iteration
  // -------------------------------------------------------------------------

  AttrIterator begin();
  AttrIterator end();

  ConstAttrIterator begin() const;
  ConstAttrIterator end() const;

  ConstAttrIterator cbegin() const;
  ConstAttrIterator cend() const;

  // -------------------------------------------------------------------------
  // Capacity
  // -------------------------------------------------------------------------

  /// Number of stored attributes.
  std::size_t size() const noexcept;

  /// Returns true if no attributes are stored.
  bool empty() const noexcept;

  // -------------------------------------------------------------------------
  // Lookup
  // -------------------------------------------------------------------------

  /// Returns true if an attribute exists.
  bool contains(const std::string &name) const;

  bool contains_all_keys(const std::vector<std::string> &keys);

  // -------------------------------------------------------------------------
  // Modification
  // -------------------------------------------------------------------------

  /// Removes all attributes.
  void clear();

  // -------------------------------------------------------------------------
  // Serialization
  // -------------------------------------------------------------------------

  /// Serialize all attributes to JSON.
  nlohmann::json json_to() const;

  /**
   * @brief Deserialize attributes from JSON.
   *
   * Existing attributes are updated; missing ones are created
   * using the attribute factory.
   */
  void json_from(const nlohmann::json &j);

private:
  AttrContainerType _attributes;
};

// -----------------------------------------------------------------------------
// Metadata helpers (external API)
// -----------------------------------------------------------------------------

/// Serialize metadata container to JSON.
nlohmann::json serialize_metadata(const AttributeContainer &m);

/// Deserialize metadata container from JSON.
void deserialize_metadata(AttributeContainer &m, const nlohmann::json &j);

} // namespace meta