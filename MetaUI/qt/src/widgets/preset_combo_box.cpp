/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

#include <algorithm>

#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "meta/macrologger.h"

#include "meta_qt/widgets/preset_combo_box.hpp"

namespace meta::qt
{

PresetComboBox::PresetComboBox(SnapshotManager *snapshot_manager_,
                               QWidget         *parent)
    : QWidget(parent), snapshot_manager(snapshot_manager_)
{
  this->combo = new QComboBox(this);

  auto *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(this->combo);

  this->connect(this->combo,
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                this,
                &PresetComboBox::on_index_changed);

  this->populate_combo();
}

void PresetComboBox::set_snapshot_provider(
    std::function<nlohmann::json()> provider)
{
  this->snapshot_provider = std::move(provider);
}

void PresetComboBox::refresh() { this->populate_combo(this->current_preset); }

void PresetComboBox::set_current_preset(const std::string &name)
{
  if (!this->snapshot_manager->has(name))
  {
    LOG_ERROR("PresetComboBox: no such preset '%s'", name.c_str());
    return;
  }
  this->populate_combo(name);
}

std::string PresetComboBox::get_current_preset() const
{
  return this->current_preset;
}

void PresetComboBox::on_index_changed(int index)
{
  if (index == PresetComboBox::save_action_index)
  {
    this->save_new_preset();
    return;
  }

  const std::string name = this->combo->itemText(index).toStdString();

  if (!this->snapshot_manager->has(name))
  {
    LOG_ERROR("PresetComboBox: selected preset '%s' no longer exists",
              name.c_str());
    this->populate_combo();
    return;
  }

  this->current_preset = name;
  Q_EMIT this->preset_selected(name, this->snapshot_manager->load(name));
}

void PresetComboBox::save_new_preset()
{
  bool    ok = false;
  QString text = QInputDialog::getText(this,
                                       tr("Save preset"),
                                       tr("Preset name:"),
                                       QLineEdit::Normal,
                                       QString(),
                                       &ok);

  if (!ok || text.trimmed().isEmpty())
  {
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
      this->populate_combo(this->current_preset);
      return;
    }
  }

  if (!this->snapshot_provider)
  {
    LOG_ERROR("PresetComboBox: no snapshot provider set, cannot save preset");
    this->populate_combo(this->current_preset);
    return;
  }

  this->snapshot_manager->save(name, this->snapshot_provider());
  this->current_preset = name;
  this->populate_combo(name);

  Q_EMIT this->preset_saved(name);
}

void PresetComboBox::populate_combo(const std::string &select_name)
{
  const QSignalBlocker blocker(this->combo);

  this->combo->clear();
  this->combo->addItem(tr("Save preset..."));

  std::vector<std::string> names = this->snapshot_manager->names();
  std::sort(names.begin(), names.end());

  // Pull "default" to the front, right after "Save preset...", if present.
  auto it = std::find(names.begin(), names.end(), "default");
  if (it != names.end())
  {
    names.erase(it);
    names.insert(names.begin(), "default");
  }

  for (const std::string &name : names)
    this->combo->addItem(QString::fromStdString(name));

  const std::string &target = select_name.empty() ? this->current_preset
                                                  : select_name;

  int index_to_select = PresetComboBox::save_action_index;
  if (!target.empty())
  {
    const int found = this->combo->findText(QString::fromStdString(target));
    if (found >= 0) index_to_select = found;
  }

  this->combo->setCurrentIndex(index_to_select);
  this->current_preset = index_to_select == PresetComboBox::save_action_index
                             ? std::string()
                             : this->combo->itemText(index_to_select)
                                   .toStdString();
}

} // namespace meta::qt
