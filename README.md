# Meta

## Extending Meta with a New Built-in Type (Core Source Changes)

To add support for a new type in Meta, follow these steps.

### 1. Register the Type Name

Register the type name in `Meta/include/meta/type/type_name.hpp` to associate the C++ type with a string identifier.

```cpp
namespace meta
{
template <>
struct TypeName<float>
{
    static constexpr std::string_view name = "float";
};
}
```

Alternatively, use the provided macro (outside the `meta` namespace):

```cpp
META_DEFINE_TYPE_NAME(float);
```

### 2. Define the Attribute Traits

In `Meta/include/meta/type/attribute_traits.hpp`, specialize `AttributeTraits<T>` if the generic implementation is not sufficient.

A specialization can provide:

- `to_string()` for string conversion.
- `json_to()` for serialization to `nlohmann::json`.
- `json_from()` for deserialization from `nlohmann::json`.

For example, for `glm::vec2`:

```cpp
template <>
struct AttributeTraits<glm::vec2>
{
    static std::string to_string(const glm::vec2& v)
    {
        return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
    }

    static nlohmann::json json_to(const glm::vec2& v)
    {
        return {{"x", v.x}, {"y", v.y}};
    }

    static glm::vec2 json_from(const nlohmann::json& j)
    {
        return {
            j.at("x").get<float>(),
            j.at("y").get<float>()
        };
    }
};
```

### 3. Register the Type in the Attribute Factory

To enable automatic instantiation during deserialization, register the type in the attribute factory.

For built-in types, add the registration in `Meta/src/serialization/attribute_factory.cpp` inside `register_builtin_types()`:

```cpp
META_REGISTER_ATTRIBUTE_TYPE(float);
```

Alternatively, register the type manually before it is used:

```cpp
meta::register_attribute_type<float>("float");
```

Once these three steps are complete, the type can be serialized, deserialized, and instantiated automatically by the Meta framework.