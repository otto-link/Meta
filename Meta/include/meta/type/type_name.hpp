/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file type_registry.hpp
 * @brief Runtime type registry and factory for attribute instantiation.
 *
 * Provides a central mechanism to:
 * - register attribute types by string identifier
 * - create attributes at runtime from serialized type names
 * - support deserialization of heterogeneous attribute containers
 *
 * This system is a key component of the reflection layer, enabling
 * dynamic reconstruction of typed attributes without compile-time knowledge
 * of their concrete types.
 *
 * @copyright Copyright (c) 2026
 */
#pragma once
#include <string>
#include <string_view>
#include <type_traits>

/**
 * @brief Define a TypeName specialization for a given type.
 *
 * Creates a compile-time mapping between a C++ type and its string name.
 *
 * @param TYPE C++ type to register.
 */
#define META_DEFINE_TYPE_NAME(...)                                             \
  namespace meta                                                               \
  {                                                                            \
  template <> struct TypeName<__VA_ARGS__>                                     \
  {                                                                            \
    static constexpr std::string_view name = #__VA_ARGS__;                     \
  };                                                                           \
  }

namespace meta
{

// -----------------------------------------------------------------------------
// Primary template (intentionally undefined for unsupported types)
// -----------------------------------------------------------------------------

/**
 * @brief Primary TypeName template (unspecialized).
 *
 * This template must be specialized for each supported type.
 * Using it without specialization triggers a compile-time error
 * via the fallback implementation.
 *
 * @tparam T Type to be mapped to a string identifier.
 */
template <typename T> struct TypeName;

// -----------------------------------------------------------------------------
// Helper concept (optional but useful)
// -----------------------------------------------------------------------------

/**
 * @brief Compile-time check for valid TypeName specialization.
 *
 * Evaluates to true if TypeName<T>::name exists.
 * Used for validating type registration in the reflection system.
 */
template <typename T>
inline constexpr bool has_type_name_v = requires { TypeName<T>::name; };

// -----------------------------------------------------------------------------
// Fallback (optional): catches missing specializations at compile time
// -----------------------------------------------------------------------------

/**
 * @brief Fallback TypeName specialization (compile-time guard).
 *
 * Triggers a static assertion if a type is used without specialization.
 * Prevents silent runtime errors in the reflection system.
 */
template <typename T> struct TypeName
{
  static_assert(sizeof(T) == 0,
                "TypeName<T> is not specialized for this type.");
};

} // namespace meta

// -----------------------------------------------------------------------------
// Built-in types
// -----------------------------------------------------------------------------

// /!\ also need to be registered in attribute_factory.cpp for deserialization

META_DEFINE_TYPE_NAME(float);
META_DEFINE_TYPE_NAME(double);
META_DEFINE_TYPE_NAME(int);
META_DEFINE_TYPE_NAME(bool);

META_DEFINE_TYPE_NAME(uint8_t);
META_DEFINE_TYPE_NAME(uint16_t);
META_DEFINE_TYPE_NAME(uint32_t);
META_DEFINE_TYPE_NAME(uint64_t);

META_DEFINE_TYPE_NAME(std::string); // basic need...

#ifdef META_ENABLE_STD_TYPES
#include "meta/type/type_name_std.inl"
#endif

#ifdef META_ENABLE_GLM_TYPES
#include "meta/type/type_name_glm.inl"
#endif
