/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/core/container_group.hpp"

namespace meta
{

AttributeContainer &ContainerGroup::add(const std::string &key)
{
  auto [it, inserted] = _containers.try_emplace(
      key,
      std::make_unique<AttributeContainer>());

  if (!inserted) throw std::runtime_error("Container already exists: " + key);

  if (!_current) _current = it->second.get();

  return *it->second;
}

bool ContainerGroup::contains(const std::string &key) const
{
  return _containers.contains(key);
}

AttributeContainer &ContainerGroup::current()
{
  if (!_current) throw std::runtime_error("No current container selected");

  return *_current;
}

const AttributeContainer &ContainerGroup::current() const
{
  if (!_current) throw std::runtime_error("No current container selected");

  return *_current;
}

bool ContainerGroup::erase(const std::string &key)
{
  auto it = _containers.find(key);

  if (it == _containers.end()) return false;

  if (_current == it->second.get()) _current = nullptr;

  _containers.erase(it);

  if (!_current && !_containers.empty())
    _current = _containers.begin()->second.get();

  return true;
}

AttributeContainer *ContainerGroup::find(const std::string &key)
{
  auto it = _containers.find(key);
  return it != _containers.end() ? it->second.get() : nullptr;
}

const AttributeContainer *ContainerGroup::find(const std::string &key) const
{
  auto it = _containers.find(key);
  return it != _containers.end() ? it->second.get() : nullptr;
}

void ContainerGroup::set_current(const std::string &key)
{
  auto *container = find(key);

  if (!container) throw std::runtime_error("Container does not exist: " + key);

  _current = container;
}

} // namespace meta
