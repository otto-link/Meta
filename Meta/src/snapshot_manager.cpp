/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/serialization/snapshot_manager.hpp"

namespace meta
{

void SnapshotManager::save(const std::string    &name,
                           const nlohmann::json &snapshot)
{
  _snapshots[name] = snapshot;
}

bool SnapshotManager::has(const std::string &name) const
{
  return _snapshots.contains(name);
}

const nlohmann::json &SnapshotManager::load(const std::string &name) const
{
  return _snapshots.at(name);
}

void SnapshotManager::erase(const std::string &name) { _snapshots.erase(name); }

void SnapshotManager::clear() { _snapshots.clear(); }

std::vector<std::string> SnapshotManager::names() const
{
  std::vector<std::string> result;
  result.reserve(_snapshots.size());

  for (const auto &[name, _] : _snapshots)
    result.push_back(name);

  return result;
}

} // namespace meta
