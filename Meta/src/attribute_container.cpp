/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <cstddef>
#include <iostream>
#include <utility>

#include <nlohmann/json.hpp>

#include "meta/core/attribute_container.hpp"
#include "meta/serialization/attribute_factory.hpp"

namespace meta
{

AttrIterator AttributeContainer::begin() { return _attributes.begin(); }

ConstAttrIterator AttributeContainer::begin() const
{
  return _attributes.begin();
}

ConstAttrIterator AttributeContainer::cbegin() const
{
  return _attributes.cbegin();
}

ConstAttrIterator AttributeContainer::cend() const
{
  return _attributes.cend();
}

void AttributeContainer::clear() { _attributes.clear(); }

bool AttributeContainer::contains(const std::string &name) const
{
  return _attributes.contains(name);
}

bool AttributeContainer::contains_all_keys(const std::vector<std::string> &keys)
{
  for (const auto &key : keys)
    if (_attributes.find(key) == _attributes.end()) return false;

  return true;
}

bool AttributeContainer::empty() const noexcept { return _attributes.empty(); }

AttrIterator AttributeContainer::end() { return _attributes.end(); }

ConstAttrIterator AttributeContainer::end() const { return _attributes.end(); }

AbstractAttribute *AttributeContainer::find(const std::string &name)
{
  auto it = _attributes.find(name);
  return it == _attributes.end() ? nullptr : it->second.get();
}

const AbstractAttribute *AttributeContainer::find(const std::string &name) const
{
  auto it = _attributes.find(name);
  return it == _attributes.end() ? nullptr : it->second.get();
}

void AttributeContainer::json_from(const nlohmann::json &j)
{
  for (auto &[name, value] : j.items())
  {
    auto it = _attributes.find(name);

    if (it == _attributes.end())
    {
      auto new_attr = AttributeFactory::instance().create(value["type"], name);

      if (!new_attr)
      {
        std::cerr << "Unknown attribute type: " << value["type"] << "\n";
        continue;
      }

      auto [insert_it, inserted] = _attributes.emplace(name,
                                                       std::move(new_attr));

      it = insert_it;
    }

    it->second->json_from(value);
  }
}

nlohmann::json AttributeContainer::json_to() const
{
  nlohmann::json j;

  for (const auto &[name, attr] : _attributes)
    j[name] = attr->json_to();

  return j;
}

std::size_t AttributeContainer::size() const noexcept
{
  return _attributes.size();
}

// --- Functions

void deserialize_metadata(AttributeContainer &m, const nlohmann::json &j)
{
  m.json_from(j);
}

nlohmann::json serialize_metadata(const AttributeContainer &m)
{
  return m.json_to();
}

} // namespace meta
