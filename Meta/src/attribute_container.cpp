/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <cstddef>
#include <iostream>
#include <utility>

#include <nlohmann/json.hpp>

#include "meta/core/attribute_container.hpp"
#include "meta/macrologger.h"
#include "meta/serialization/attribute_factory.hpp"

namespace meta
{

AttrIterator AttributeContainer::begin() { return attributes_.begin(); }

ConstAttrIterator AttributeContainer::begin() const
{
  return attributes_.begin();
}

ConstAttrIterator AttributeContainer::cbegin() const
{
  return attributes_.cbegin();
}

ConstAttrIterator AttributeContainer::cend() const
{
  return attributes_.cend();
}

void AttributeContainer::clear()
{
  attributes_.clear();
  compact_insertion_order();
}

void AttributeContainer::compact_insertion_order()
{
  insertion_order_.erase(std::remove_if(insertion_order_.begin(),
                                        insertion_order_.end(),
                                        [this](const std::string &name) {
                                          return !attributes_.contains(name);
                                        }),
                         insertion_order_.end());
}

bool AttributeContainer::contains(const std::string &name) const
{
  return attributes_.contains(name);
}

bool AttributeContainer::contains_all_keys(const std::vector<std::string> &keys)
{
  for (const auto &key : keys)
    if (attributes_.find(key) == attributes_.end()) return false;

  return true;
}

bool AttributeContainer::empty() const noexcept { return attributes_.empty(); }

AttrIterator AttributeContainer::end() { return attributes_.end(); }

ConstAttrIterator AttributeContainer::end() const { return attributes_.end(); }

AbstractAttribute *AttributeContainer::find(const std::string &name)
{
  auto it = attributes_.find(name);
  return it == attributes_.end() ? nullptr : it->second.get();
}

const AbstractAttribute *AttributeContainer::find(const std::string &name) const
{
  auto it = attributes_.find(name);
  return it == attributes_.end() ? nullptr : it->second.get();
}

const std::vector<std::string> &AttributeContainer::insertion_order() const
{
  return insertion_order_;
}

void AttributeContainer::json_from(const nlohmann::json &j)
{
  for (auto &[name, value] : j.items())
  {
    auto it = attributes_.find(name);

    if (it == attributes_.end())
    {
      const std::string attr_type = value.at("type");
      auto new_attr = AttributeFactory::instance().create(attr_type, name);

      if (!new_attr)
      {
        LOG_ERROR("Unknown attribute type: %s", attr_type.c_str());
        continue;
      }

      auto [insert_it, inserted] = attributes_.emplace(name,
                                                       std::move(new_attr));

      it = insert_it;
    }

    it->second->json_from(value);
  }
}

nlohmann::json AttributeContainer::json_to() const
{
  nlohmann::json j;

  for (const auto &[name, attr] : attributes_)
    j[name] = attr->json_to();

  return j;
}

std::size_t AttributeContainer::size() const noexcept
{
  return attributes_.size();
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
