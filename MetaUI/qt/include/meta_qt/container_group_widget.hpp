/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <regex>

#include <QComboBox>
#include <QStackedWidget>

#include "meta/core/container_group.hpp"

#include "meta_qt/container_widget.hpp"
#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

/// Widget that displays and edits a ContainerGroup using a stacked UI.
class ContainerGroupWidget : public MetaWidget
{
public:
  /// Construct a ContainerGroupWidget.
  ContainerGroupWidget(
      meta::ContainerGroup  &group,
      ContainerRenderOptions options = ContainerRenderOptions{},
      QWidget               *parent = nullptr);

public slots:
  /// Synchronize the contained MetaWidgets from their model.
  void on_sync_meta_widgets_from_model();

private:
  /// Build a widget for a single container entry.
  QWidget *build_container_widget(const std::string &key);

  /// Synchronize combo box selection with stacked widget page.
  void sync_stack();

private:
  meta::ContainerGroup  &group;   /// Underlying container group
  ContainerRenderOptions options; /// Rendering options

  QComboBox      *combo = nullptr;   /// Selector for container keys
  QStackedWidget *stacked = nullptr; /// Stacked pages for each container

  std::unordered_map<std::string, QWidget *> pages; /// Cached page widgets
};

/// Render a ContainerGroup into a MetaWidget.
MetaWidget *render(meta::ContainerGroup  &group,
                   ContainerRenderOptions options = ContainerRenderOptions{},
                   QWidget               *parent = nullptr);

} // namespace meta::qt