/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <QVBoxLayout>

#include "meta_qt/container_group_widget.hpp"

namespace meta::qt
{

ContainerGroupWidget::ContainerGroupWidget(meta::ContainerGroup &group,
                                           CategoryPolicy        group_policy,
                                           const std::string &root_group_name,
                                           QWidget           *parent)
    : MetaWidget(parent),
      group(group),
      group_policy(group_policy),
      root_group_name(root_group_name)
{
  auto *root = new QVBoxLayout(this);
  this->setLayout(root);

  // --- Selector

  combo = new QComboBox(this);
  root->addWidget(combo);

  stacked = new QStackedWidget(this);
  root->addWidget(stacked);

  // fill containers
  for (auto &[key, _] : group.containers())
  {
    combo->addItem(QString::fromStdString(key));

    // build widget ONCE
    auto *w = build_container_widget(key);
    pages[key] = w;
    stacked->addWidget(w);
  }

  // set initial
  group.set_current(combo->currentText().toStdString());
  sync_stack();

  // --- Switching

  connect(combo,
          &QComboBox::currentTextChanged,
          this,
          [this](const QString &text)
          {
            this->group.set_current(text.toStdString());
            sync_stack();
          });
}

QWidget *ContainerGroupWidget::build_container_widget(const std::string &key)
{
  auto *container_widget = new QWidget();

  auto *container = group.find(key);
  if (!container) return container_widget;

  return meta::qt::render(*container, group_policy, root_group_name);
}

void ContainerGroupWidget::sync_stack()
{
  std::optional<std::string> current_name = group.current_container_name();

  if (!current_name) return;

  int index = combo->findText(QString::fromStdString(*current_name));
  if (index >= 0) stacked->setCurrentIndex(index);
}

// --- Function

MetaWidget *render(meta::ContainerGroup &group,
                   CategoryPolicy        group_policy,
                   const std::string    &root_group_name,
                   QWidget              *parent)
{
  auto *widget = new ContainerGroupWidget(group,
                                          group_policy,
                                          root_group_name,
                                          parent);
  return widget;
}

} // namespace meta::qt
