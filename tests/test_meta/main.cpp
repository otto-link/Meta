#include <iostream>

#include "meta.hpp"

struct Vec2
{
  float x;
  float y;
};

namespace meta
{

// -----------------------------------------------------------------------------
// Custom type traits
// -----------------------------------------------------------------------------

template <> struct AttributeTraits<Vec2>
{
  static std::string to_string(const Vec2 &v)
  {
    return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
  }

  static nlohmann::json json_to(const Vec2 &v)
  {
    return {{"x", v.x}, {"y", v.y}};
  }

  static Vec2 json_from(const nlohmann::json &j)
  {
    return {j.at("x").get<float>(), j.at("y").get<float>()};
  }
};

META_DEFINE_TYPE_NAME(Vec2);

} // namespace meta

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main()
{
  using namespace meta;

  AttributeContainer container;

  // --- Basic attributes

  container.add("attr_float", 1.f);
  container.add("attr_vec2", Vec2{1.f, 0.f});
  container.add("comment", "some text");

#ifdef META_ENABLE_STD_TYPES
  container.add("path", std::filesystem::path("toto"));
#endif

  // --- Metadata example

  {
    auto *attr = container.add("float_with_metadata", -1.f);

    auto &meta = attr->metadata();
    meta.add("vmin", -10.f);
    meta.add("vmax", 10.f);
    meta.add("scale", "linear");
  }

  // --- Print attributes

  for (const auto &[name, attr] : container)
  {
    std::cout << name << " : " << attr->to_string() << '\n';
  }

  std::cout << "\nSerialized container:\n";
  std::cout << container.json_to().dump(4) << "\n";

  // ---------------------------------------------------------------------------
  // Deserialization test
  // ---------------------------------------------------------------------------

  AttributeContainer container2;

  meta::register_default_types();
  register_attribute_type<Vec2>("Vec2");

  container2.json_from(container.json_to());

  std::cout << "\nDeserialized container:\n";
  std::cout << container2.json_to().dump(4) << "\n";

  return 0;
}
