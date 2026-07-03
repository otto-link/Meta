/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include <functional>
#include <string>

#include <QComboBox>
#include <QWidget>

#include <nlohmann/json.hpp>

#include "meta/serialization/snapshot_manager.hpp"

namespace meta::qt
{

/**
 * @brief Combo box backed by a SnapshotManager: offers a "Save preset..."
 * action item followed by the list of saved preset names ("default" pinned
 * first if present, remaining names sorted alphabetically).
 */
class PresetComboBox : public QWidget
{
  Q_OBJECT

public:
  explicit PresetComboBox(SnapshotManager *snapshot_manager,
                          QWidget         *parent = nullptr);

  /**
   * @brief Set the callback invoked to capture the current state when the
   * user saves a new preset. Must be set before a save can succeed.
   */
  void set_snapshot_provider(std::function<nlohmann::json()> provider);

  /// Re-read the preset list from the snapshot manager and rebuild the combo.
  void refresh();

  /// Programmatically select a preset by name, if it exists.
  void set_current_preset(const std::string &name);

  /// Name of the currently selected preset (empty if none / "Save preset..."
  /// shown).
  std::string get_current_preset() const;

signals:
  /// Emitted when the user picks an existing preset from the list.
  void preset_selected(std::string name, nlohmann::json snapshot);

  /// Emitted after a new preset has been successfully saved.
  void preset_saved(std::string name);

private slots:
  void on_index_changed(int index);

private:
  void save_new_preset();
  void populate_combo(const std::string &select_name = {});

  SnapshotManager                *snapshot_manager = nullptr;
  QComboBox                      *combo = nullptr;
  std::function<nlohmann::json()> snapshot_provider;
  std::string                     current_preset;

  static constexpr int save_action_index = 0;
};

} // namespace meta::qt