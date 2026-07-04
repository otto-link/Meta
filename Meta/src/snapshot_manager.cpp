/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/serialization/snapshot_manager.hpp"
#include "meta/logger.hpp"

namespace meta
{

void SnapshotManager::save(const std::string    &name,
                           const nlohmann::json &snapshot)
{
  Logger::log()->trace("SnapshotManager::save: '{}'", name);
  snapshots_[name] = snapshot;
}

bool SnapshotManager::has(const std::string &name) const
{
  Logger::log()->trace("SnapshotManager::has: '{}'", name);
  return snapshots_.contains(name);
}

const nlohmann::json &SnapshotManager::load(const std::string &name) const
{
  Logger::log()->trace("SnapshotManager::load: '{}'", name);
  return snapshots_.at(name);
}

void SnapshotManager::erase(const std::string &name)
{
  Logger::log()->trace("SnapshotManager::erase: '{}'", name);
  snapshots_.erase(name);
}

void SnapshotManager::clear()
{
  Logger::log()->trace("SnapshotManager::clear ({} snapshots)",
                       snapshots_.size());
  snapshots_.clear();
}

std::vector<std::string> SnapshotManager::names() const
{
  Logger::log()->trace("SnapshotManager::names: {} entries", snapshots_.size());

  std::vector<std::string> result;
  result.reserve(snapshots_.size());

  for (const auto &[name, _] : snapshots_)
    result.push_back(name);

  return result;
}

} // namespace meta
