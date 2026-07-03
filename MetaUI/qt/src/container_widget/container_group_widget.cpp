/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <QVBoxLayout>

#include "meta_qt/container_group_widget.hpp"

namespace meta::qt
{

ContainerGroupWidget::ContainerGroupWidget(meta::ContainerGroup  &group,
                                           ContainerRenderOptions options,
                                           QWidget               *parent)
    : MetaWidget(parent), group(group), options(options)
{
  auto *root = new QVBoxLayout(this);
  this->setLayout(root);

  // --- Selector

  combo = new QComboBox(this);
  root->addWidget(combo);

  stacked = new QStackedWidget(this);
  root->addWidget(stacked);

  // fill containers
  for (const auto &key : group.insertion_order())
  {
    combo->addItem(QString::fromStdString(key));

    // build widget ONCE
    auto *w = build_container_widget(key);
    pages[key] = w;
    stacked->addWidget(w);
  }

  // set initial
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
  auto *container = group.find(key);

  if (!container) return new QWidget(); // dummy

  MetaWidget *container_widget = meta::qt::render(*container, options);

  // pass-through signals
  connect(container_widget,
          &MetaWidget::edit_started,
          this,
          &MetaWidget::edit_started);

  connect(container_widget,
          &MetaWidget::value_changed,
          this,
          &MetaWidget::value_changed);

  connect(container_widget,
          &MetaWidget::edit_ended,
          this,
          &MetaWidget::edit_ended);

  return container_widget;
}

void ContainerGroupWidget::sync_stack()
{
  std::optional<std::string> current_name = group.current_container_name();

  if (!current_name) return;

  int index = combo->findText(QString::fromStdString(*current_name));
  if (index >= 0)
  {
    stacked->setCurrentIndex(index);
    combo->setCurrentIndex(index);
  }
}

// --- Function

MetaWidget *render(meta::ContainerGroup  &group,
                   ContainerRenderOptions options,
                   QWidget               *parent)
{
  auto *widget = new ContainerGroupWidget(group, options, parent);
  return widget;
}

} // namespace meta::qt
