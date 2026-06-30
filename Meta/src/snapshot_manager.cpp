/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/serialization/snapshot_manager.hpp"

namespace meta
{

void SnapshotManager::save(const std::string    &name,
                           const nlohmann::json &snapshot)
{
  snapshots_[name] = snapshot;
}

bool SnapshotManager::has(const std::string &name) const
{
  return snapshots_.contains(name);
}

const nlohmann::json &SnapshotManager::load(const std::string &name) const
{
  return snapshots_.at(name);
}

void SnapshotManager::erase(const std::string &name) { snapshots_.erase(name); }

void SnapshotManager::clear() { snapshots_.clear(); }

std::vector<std::string> SnapshotManager::names() const
{
  std::vector<std::string> result;
  result.reserve(snapshots_.size());

  for (const auto &[name, _] : snapshots_)
    result.push_back(name);

  return result;
}

} // namespace meta
