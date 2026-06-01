#include <iostream>

#include "meta.hpp"

// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------

int main()
{
  using namespace meta;

  AttributeContainer container;

  // --- Basic attributes
  container.add("attr_float", 1.f);

  // --- Metadata example
  {
    auto *attr = container.add("float_with_metadata", -1.f);

    auto &meta = attr->metadata();
    meta.add("vmin", -10.f);
    meta.add("vmax", 10.f);
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

  container2.json_from(container.json_to());

  std::cout << "\nDeserialized container:\n";
  std::cout << container2.json_to().dump(4) << "\n";

  return 0;
}
