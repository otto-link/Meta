#include <iostream>

#include <glm/glm.hpp>

#include "meta.hpp"

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main()
{
  using namespace meta;

  AttributeContainer container;
  container.add("glm::vec2", glm::vec2(0.f, 1.f));
  container.add("glm::vec3", glm::vec3(0.f, 1.f, 2.f));
  container.add("glm::vec4", glm::vec4(0.f, 1.f, 2.f, 3.f));

  container.add("glm::ivec2", glm::ivec2(0, 1));
  container.add("glm::ivec3", glm::ivec3(0, 1, 2));
  container.add("glm::ivec4", glm::ivec4(0, 1, 2, 3));

  std::cout << "\nSerialized container:\n";
  std::cout << "---------------------\n";
  std::cout << container.json_to().dump(4) << "\n";

  std::cout << "\nString export:\n";
  std::cout << "--------------\n";
  for (const auto &[name, attr] : container)
    std::cout << " " << name << ": " << attr->to_string() << "\n";

  return 0;
}
