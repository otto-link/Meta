/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <algorithm>

#include <QAbstractItemView>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "meta/logger.hpp"

#include "meta_qt/widgets/preset_combo_box.hpp"

namespace meta::qt
{

PresetComboBox::PresetComboBox(SnapshotManager *snapshot_manager_,
                               QWidget         *parent)
    : QWidget(parent), snapshot_manager(snapshot_manager_)
{
  Logger::log()->trace("PresetComboBox::PresetComboBox");

  this->combo = new QComboBox(this);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->combo);

  this->connect(this->combo,
                QOverload<int>::of(&QComboBox::activated),
                this,
                &PresetComboBox::on_index_activated);

  // --- Context menu for deletion

  this->combo->view()->setContextMenuPolicy(Qt::CustomContextMenu);

  this->connect(this->combo->view(),
                &QWidget::customContextMenuRequested,
                this,
                &PresetComboBox::on_context_menu_requested);

  // update this widget if the underlying data have been modified
  // elsewhere
  connection_ = snapshot_manager->snapshots_modified.subscribe(
      [this]() { this->refresh(); });

  this->populate_combo();
}

void PresetComboBox::set_snapshot_provider(
    std::function<nlohmann::json()> provider)
{
  Logger::log()->trace("PresetComboBox::set_snapshot_provider");

  this->snapshot_provider = std::move(provider);
}

void PresetComboBox::refresh()
{
  Logger::log()->trace("PresetComboBox::refresh");

  this->populate_combo(this->current_preset);
}

void PresetComboBox::set_current_preset(const std::string &name)
{
  Logger::log()->trace("PresetComboBox::set_current_preset: '{}'", name);

  if (!this->snapshot_manager->has(name))
  {
    Logger::log()->error("PresetComboBox: no such preset '{}'", name);
    return;
  }

  this->populate_combo(name);
}

std::string PresetComboBox::get_current_preset() const
{
  return this->current_preset;
}

void PresetComboBox::on_index_activated(int index)
{
  Logger::log()->trace("PresetComboBox::on_index_activated: index={}", index);

  if (index == PresetComboBox::save_action_index)
  {
    this->save_new_preset();
    return;
  }

  const std::string name = this->combo->itemText(index).toStdString();

  if (!this->snapshot_manager->has(name))
  {
    Logger::log()->error("PresetComboBox::on_index_activated: selected preset "
                         "'{}' no longer exists",
                         name);
    this->populate_combo();
    return;
  }

  Logger::log()->trace(
      "PresetComboBox::on_index_activated: selected preset '{}'",
      name);

  this->current_preset = name;
  Q_EMIT this->preset_selected(name, this->snapshot_manager->load(name));
}

void PresetComboBox::save_new_preset()
{
  Logger::log()->trace("PresetComboBox::save_new_preset");

  bool    ok = false;
  QString text = QInputDialog::getText(this,
                                       tr("Save preset"),
                                       tr("Preset name:"),
                                       QLineEdit::Normal,
                                       QString(),
                                       &ok);

  if (!ok || text.trimmed().isEmpty())
  {
    Logger::log()->trace("PresetComboBox::save_new_preset: cancelled");

    this->populate_combo(this->current_preset);
    return;
  }

  const std::string name = text.trimmed().toStdString();

  if (this->snapshot_manager->has(name))
  {
    const auto ret = QMessageBox::question(
        this,
        tr("Overwrite preset"),
        tr("A preset named \"%1\" already exists. Overwrite it?")
            .arg(QString::fromStdString(name)));

    if (ret != QMessageBox::Yes)
    {
      Logger::log()->trace(
          "PresetComboBox::save_new_preset: overwrite cancelled for '{}'",
          name);

      this->populate_combo(this->current_preset);
      return;
    }
  }

  if (!this->snapshot_provider)
  {
    Logger::log()->error("PresetComboBox::save_new_preset: no snapshot "
                         "provider set, cannot save preset");
    this->populate_combo(this->current_preset);
    return;
  }

  this->snapshot_manager->save(name, this->snapshot_provider());

  Logger::log()->trace("PresetComboBox::save_new_preset: saved '{}'", name);

  this->current_preset = name;
  this->populate_combo(name);

  Q_EMIT this->preset_saved(name);
}

void PresetComboBox::on_context_menu_requested(const QPoint &pos)
{
  Logger::log()->trace("PresetComboBox::on_context_menu_requested");

  const int index = this->combo->view()->indexAt(pos).row();
  if (index < 0) return;

  const std::string name = this->combo->itemText(index).toStdString();

  // --- ignore special entries
  if (name.empty() || name == "──────────" ||
      index == PresetComboBox::save_action_index)
    return;

  QMenu menu(this);

  QAction *delete_action = menu.addAction(tr("Delete preset"));

  QAction *selected = menu.exec(this->combo->mapToGlobal(pos));

  if (selected == delete_action)
  {
    Logger::log()->trace(
        "PresetComboBox::on_context_menu_requested: delete '{}'",
        name);

    this->delete_preset(name);
  }
}

void PresetComboBox::delete_preset(const std::string &name)
{
  Logger::log()->trace("PresetComboBox::delete_preset: '{}'", name);

  if (name == "default")
  {
    QMessageBox::warning(this,
                         tr("Cannot delete preset"),
                         tr("The default preset cannot be deleted."));
    return;
  }

  if (!this->snapshot_manager->has(name)) return;

  const auto ret = QMessageBox::question(
      this,
      tr("Delete preset"),
      tr("Are you sure you want to delete \"%1\"?")
          .arg(QString::fromStdString(name)));

  if (ret != QMessageBox::Yes)
  {
    Logger::log()->trace("PresetComboBox::delete_preset: cancelled '{}'", name);
    return;
  }

  this->snapshot_manager->erase(name);

  Logger::log()->trace("PresetComboBox::delete_preset: deleted preset '{}'",
                       name);

  if (this->current_preset == name) this->current_preset.clear();

  this->populate_combo(this->current_preset);

  Q_EMIT this->preset_deleted(name);
}

void PresetComboBox::populate_combo(const std::string &select_name)
{
  Logger::log()->trace("PresetComboBox::populate_combo: select='{}'",
                       select_name);

  const QSignalBlocker blocker(this->combo);

  this->combo->clear();

  // --- Save preset entry
  this->combo->addItem(tr("[Save preset...]"));

  std::vector<std::string> names = this->snapshot_manager->names();
  std::sort(names.begin(), names.end());

  // Pull "default" to front
  auto it = std::find(names.begin(), names.end(), "default");
  if (it != names.end())
  {
    names.erase(it);
    names.insert(names.begin(), "default");
  }

  int default_index = -1;

  for (const std::string &name : names)
  {
    if (name == "default") default_index = this->combo->count();

    this->combo->addItem(QString::fromStdString(name));
  }

  // --- separator
  const int sep_index = default_index >= 0 ? default_index + 1
                                           : this->combo->count();

  this->combo->insertItem(sep_index, "──────────");
  this->combo->setItemData(sep_index, 0, Qt::UserRole - 1);

  // --- selection logic
  const std::string &target = select_name.empty() ? this->current_preset
                                                  : select_name;

  int index_to_select = PresetComboBox::save_action_index;

  if (!target.empty())
  {
    const int found = this->combo->findText(QString::fromStdString(target));

    if (found >= 0) index_to_select = found;
  }

  this->combo->setCurrentIndex(index_to_select);

  this->current_preset = (index_to_select == PresetComboBox::save_action_index)
                             ? std::string()
                             : this->combo->itemText(index_to_select)
                                   .toStdString();
}

} // namespace meta::qt
