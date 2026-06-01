/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/serialization/attribute_factory.hpp"
#include "meta/core/abstract_attribute.hpp"
#include "meta/core/attribute_container.hpp"

namespace meta
{

std::unique_ptr<AbstractAttribute> AttributeFactory::create(
    const std::string &name,
    const std::string &attr_name) const
{
  auto it = _registry.find(name);
  if (it == _registry.end()) return nullptr;

  return it->second(attr_name);
}

void register_default_types()
{
  register_attribute_type<int>("int");
  register_attribute_type<float>("float");
  register_attribute_type<double>("double");
  register_attribute_type<bool>("bool");

#ifdef META_ENABLE_STD_TYPES
  register_attribute_type<std::string>("std::string");
#endif
}

} // namespace meta
