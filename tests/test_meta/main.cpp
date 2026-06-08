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

} // namespace meta

META_DEFINE_TYPE_NAME(Vec2);

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main()
{
  using namespace meta;

  AttributeContainer container;

  // --- Basic attributes

  container.add("vec2", Vec2{1.f, 0.f}); // custom

  container.add("float", 1.f);
  container.add("double", 1.0);
  container.add("int", 1);
  container.add("bool", true);
  container.add("std::string", "some text");

  container.add("uint8_t", (uint8_t)8);
  container.add("uint16_t", (uint16_t)16);
  container.add("uint32_t", (uint32_t)32);
  container.add("uint64_t", (uint64_t)64);

#ifdef META_ENABLE_STD_TYPES
  container.add("std::filesystem::path", std::filesystem::path("some_path"));

  container.add("std::vector<std::string>", std::vector<std::string>{"a", "b"});
  container.add("std::vector<bool>", std::vector<bool>{true, false});

  {
    std::vector<std::pair<int, std::string>> options = {{0, "Linear"},
                                                        {1, "Cubic"},
                                                        {2, "Bezier"}};
    container.add("std::vector<std::pair<int, std::string>>", options);
  }
#endif

#ifdef META_ENABLE_GLM_TYPES
  container.add("glm::vec2", glm::vec2(0.f, 1.f));
  container.add("glm::vec3", glm::vec3(0.f, 1.f, 2.f));
  container.add("glm::vec4", glm::vec4(0.f, 1.f, 2.f, 3.f));

  container.add("glm::ivec2", glm::ivec2(0, 1));
  container.add("glm::ivec3", glm::ivec3(0, 1, 2));
  container.add("glm::ivec4", glm::ivec4(0, 1, 2, 3));
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

  // --- Pass-through

  std::cout << "\nPass through, container level accessor(s):\n";
  std::cout << "float value: " << container.value<float>("float") << "\n";

  // ---------------------------------------------------------------------------
  // Deserialization test
  // ---------------------------------------------------------------------------

  AttributeContainer container2;

  // "basic" types
  meta::register_default_types();

  // any specific class needs to be registered to the attribute
  // factory to allow deserialization
  register_attribute_type<Vec2>("Vec2");

  container2.json_from(container.json_to());

  std::cout << "\nDeserialized container:\n";
  std::cout << container2.json_to().dump(4) << "\n";

  return 0;
}
