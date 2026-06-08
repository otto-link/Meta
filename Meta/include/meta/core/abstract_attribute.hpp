/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file abstract_attribute.hpp
 * @brief Base interface for all reflected attributes.
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <any>
#include <string>
#include <typeindex>

#include <nlohmann/json.hpp>

#include "meta/core/meta_object.hpp"
#include "meta/macrologger.h"

namespace meta
{

/**
 * @brief Base class for all attribute types stored in the reflection system.
 *
 * Provides a uniform interface for:
 * - runtime type erasure
 * - generic value access
 * - string and JSON serialization
 * - dynamic modification via std::any
 *
 * All concrete attributes are expected to derive from this class.
 */
class AbstractAttribute : public MetaObject
{
public:
  virtual ~AbstractAttribute() = default;

  /**
   * @brief Returns the attribute name.
   */
  virtual const std::string &name() const = 0;

  /**
   * @brief Returns the C++ type identifier of the stored value.
   */
  virtual std::type_index type() const = 0;

  /**
   * @brief Returns a mutable pointer to the underlying value.
   */
  virtual void *raw_ptr() = 0;

  /**
   * @brief Returns a const pointer to the underlying value.
   */
  virtual const void *raw_ptr() const = 0;

  /**
   * @brief Sets the value from a type-erased std::any.
   * @return true if the type matches and assignment succeeded.
   */
  virtual bool set_from_any(const std::any &value) = 0;

  /**
   * @brief Returns the value as a type-erased std::any.
   */
  virtual std::any to_any() const = 0;

  /**
   * @brief Returns a human-readable string representation.
   */
  virtual std::string to_string() const = 0;

  /**
   * @brief Serializes the attribute to JSON.
   */
  virtual nlohmann::json json_to() const = 0;

  /**
   * @brief Deserializes the attribute from JSON.
   */
  virtual void json_from(const nlohmann::json &j) = 0;

  /**
   * @brief Attempts to cast this attribute to the specified derived type.
   * @return Pointer to the requested type, or nullptr if the cast fails.
   */
  template <class T = void> T *try_cast()
  {
    T *ptr = dynamic_cast<T *>(this);
    if (ptr)
      return ptr;
    else
    {
      LOG_ERROR("in AbstractAttribute::get_ref, trying to get an attribute "
                "type which is not "
                "compatible with the current instance. Get type is: %s, "
                "current type is: %s",
                typeid(T).name(),
                type().name());
      return nullptr;
    }
  }

  /**
   * @brief Attempts to cast this attribute to the specified derived type.
   * @return Pointer to the requested type, or nullptr if the cast fails.
   */
  template <class T = void> const T *try_cast() const
  {
    T *ptr = dynamic_cast<T *>(this);
    if (ptr)
      return ptr;
    else
    {
      LOG_ERROR("in AbstractAttribute::get_ref, trying to get an attribute "
                "type which is not "
                "compatible with the current instance. Get type is: %s, "
                "current type is: %s",
                typeid(T).name(),
                type().name());
      return nullptr;
    }
  }
};

} // namespace meta