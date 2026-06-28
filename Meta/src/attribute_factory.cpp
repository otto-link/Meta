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
  META_REGISTER_ATTRIBUTE_TYPE(int);
  META_REGISTER_ATTRIBUTE_TYPE(float);
  META_REGISTER_ATTRIBUTE_TYPE(double);
  META_REGISTER_ATTRIBUTE_TYPE(bool);

  META_REGISTER_ATTRIBUTE_TYPE(uint8_t);
  META_REGISTER_ATTRIBUTE_TYPE(uint16_t);
  META_REGISTER_ATTRIBUTE_TYPE(uint32_t);
  META_REGISTER_ATTRIBUTE_TYPE(uint64_t);

  META_REGISTER_ATTRIBUTE_TYPE(std::string);
  META_REGISTER_ATTRIBUTE_TYPE(std::filesystem::path);

  META_REGISTER_ATTRIBUTE_TYPE(std::vector<float>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<double>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<int>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<bool>);

  META_REGISTER_ATTRIBUTE_TYPE(std::vector<std::string>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<std::pair<int, std::string>>);

#ifdef META_ENABLE_GLM_TYPES
  META_REGISTER_ATTRIBUTE_TYPE(glm::vec2);
  META_REGISTER_ATTRIBUTE_TYPE(glm::vec3);
  META_REGISTER_ATTRIBUTE_TYPE(glm::vec4);

  META_REGISTER_ATTRIBUTE_TYPE(glm::ivec2);
  META_REGISTER_ATTRIBUTE_TYPE(glm::ivec3);
  META_REGISTER_ATTRIBUTE_TYPE(glm::ivec4);

  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::vec2>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::vec3>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::vec4>);
  
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::ivec2>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::ivec3>);
  META_REGISTER_ATTRIBUTE_TYPE(std::vector<glm::ivec4>);
#endif
}

} // namespace meta
