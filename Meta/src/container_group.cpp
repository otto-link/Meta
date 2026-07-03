/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/core/container_group.hpp"

namespace meta
{

AttributeContainer &ContainerGroup::add(const std::string &key)
{
  auto [it, inserted] = containers_.try_emplace(
      key,
      std::make_unique<AttributeContainer>());

  if (!inserted) throw std::runtime_error("Container already exists: " + key);

  if (!current_) current_ = it->second.get();

  // keep track of insertion order
  insertion_order_.push_back(key);

  return *it->second;
}

void ContainerGroup::compact_insertion_order()
{
  insertion_order_.erase(std::remove_if(insertion_order_.begin(),
                                        insertion_order_.end(),
                                        [this](const std::string &name) {
                                          return !containers_.contains(name);
                                        }),
                         insertion_order_.end());
}

const std::unordered_map<std::string, std::unique_ptr<AttributeContainer>> &
ContainerGroup::containers() const
{
  return containers_;
}

bool ContainerGroup::contains(const std::string &key) const
{
  return containers_.contains(key);
}

AttributeContainer &ContainerGroup::current()
{
  if (!current_) throw std::runtime_error("No current container selected");

  return *current_;
}

const AttributeContainer &ContainerGroup::current() const
{
  if (!current_) throw std::runtime_error("No current container selected");

  return *current_;
}

std::optional<std::string> ContainerGroup::current_container_name() const
{
  if (!current_) return std::nullopt;

  for (const auto &[key, container] : containers_)
    if (container.get() == current_) return key;

  return std::nullopt;
}

bool ContainerGroup::erase(const std::string &key)
{
  auto it = containers_.find(key);

  if (it == containers_.end()) return false;

  if (current_ == it->second.get()) current_ = nullptr;

  containers_.erase(it);

  if (!current_ && !containers_.empty())
    current_ = containers_.begin()->second.get();

  compact_insertion_order();

  return true;
}

AttributeContainer *ContainerGroup::find(const std::string &key)
{
  auto it = containers_.find(key);
  return it != containers_.end() ? it->second.get() : nullptr;
}

const AttributeContainer *ContainerGroup::find(const std::string &key) const
{
  auto it = containers_.find(key);
  return it != containers_.end() ? it->second.get() : nullptr;
}

const std::vector<std::string> &ContainerGroup::insertion_order() const
{
  return insertion_order_;
}

void ContainerGroup::set_current(const std::string &key)
{
  auto *container = find(key);

  if (!container) throw std::runtime_error("Container does not exist: " + key);

  current_ = container;
}

} // namespace meta
