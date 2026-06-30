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

class ContainerGroupWidget : public MetaWidget
{
public:
  ContainerGroupWidget(
      meta::ContainerGroup  &group,
      ContainerRenderOptions options = ContainerRenderOptions{},
      QWidget               *parent = nullptr);

private:
  QWidget *build_container_widget(const std::string &key);

  void sync_stack();

private:
  meta::ContainerGroup  &group;
  ContainerRenderOptions options;

  QComboBox      *combo = nullptr;
  QStackedWidget *stacked = nullptr;

  std::unordered_map<std::string, QWidget *> pages;
};

// wrapper
MetaWidget *render(meta::ContainerGroup  &group,
                   ContainerRenderOptions options = ContainerRenderOptions{},
                   QWidget               *parent = nullptr);

} // namespace meta::qt