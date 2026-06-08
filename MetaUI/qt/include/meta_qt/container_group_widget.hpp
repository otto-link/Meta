/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QComboBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "meta/core/container_group.hpp"

#include "meta_qt/container_widget.hpp"
#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

class ContainerGroupWidget : public MetaWidget
{
public:
  ContainerGroupWidget(meta::ContainerGroup &group,
                       CategoryPolicy group_policy = CategoryPolicy::CP_SMART,
                       const std::string &root_group_name = "",
                       QWidget           *parent = nullptr)
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

private:
  QWidget *build_container_widget(const std::string &key)
  {
    auto *container_widget = new QWidget();

    auto *container = group.find(key);
    if (!container) return container_widget;

    return meta::qt::render(*container, group_policy, root_group_name);
  }

  void sync_stack()
  {
    std::optional<std::string> current_name = group.current_container_name();

    if (!current_name) return;

    int index = combo->findText(QString::fromStdString(*current_name));
    if (index >= 0) stacked->setCurrentIndex(index);
  }

private:
  meta::ContainerGroup &group;
  CategoryPolicy        group_policy;
  std::string           root_group_name;

  QComboBox      *combo = nullptr;
  QStackedWidget *stacked = nullptr;

  std::unordered_map<std::string, QWidget *> pages;
};

inline MetaWidget *render(
    meta::ContainerGroup &group,
    CategoryPolicy        group_policy = CategoryPolicy::CP_SMART,
    const std::string    &root_group_name = "",
    QWidget              *parent = nullptr)
{
  auto *widget = new ContainerGroupWidget(group,
                                          group_policy,
                                          root_group_name,
                                          parent);
  return widget;
}

} // namespace meta::qt