/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file snapshot_manager.hpp
 * @brief Named snapshot manager for storing and restoring serialized snapshots.
 * @copyright Copyright (c) 2026
 */

#pragma once
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace meta
{

class SnapshotManager
{
public:
  /**
   * @brief Save a snapshot under a given name.
   * @param name Snapshot identifier.
   * @param snapshot Serialized snapshot.
   */
  void save(const std::string &name, const nlohmann::json &snapshot);

  /**
   * @brief Check whether a snapshot exists.
   * @param name Snapshot identifier.
   * @return True if the snapshot exists.
   */
  bool has(const std::string &name) const;

  /**
   * @brief Retrieve a previously saved snapshot.
   * @param name Snapshot identifier.
   * @return Const reference to the stored snapshot.
   * @throws std::out_of_range if the snapshot does not exist.
   */
  const nlohmann::json &load(const std::string &name) const;

  /**
   * @brief Remove a saved snapshot.
   * @param name Snapshot identifier.
   */
  void erase(const std::string &name);

  /// Remove all saved snapshots.
  void clear();

  /**
   * @brief Get all saved snapshot names.
   * @return List of snapshot names.
   */
  std::vector<std::string> names() const;

private:
  std::unordered_map<std::string, nlohmann::json> snapshots_;
};

} // namespace meta
