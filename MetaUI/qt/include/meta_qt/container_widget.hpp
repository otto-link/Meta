/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QVBoxLayout>

#include "meta/core/abstract_attribute.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

enum ContainerGroupPolicy
{
  CGP_FLAT,
  CGP_TREE,
  CGP_MERGED,
  CGP_SMART
};

struct GroupNode
{
  std::string name;

  std::vector<meta::AbstractAttribute *> attributes;

  std::map<std::string, std::unique_ptr<GroupNode>> children;
};

void insert_attribute(GroupNode               &root,
                      const std::string       &path,
                      meta::AbstractAttribute *p_attr);

std::string compute_flattened_path(GroupNode *node);

void render_flat(GroupNode &node, QVBoxLayout *layout);

void render_group(GroupNode &node, QVBoxLayout *parent_layout);

void render_group_merged(GroupNode &node, QVBoxLayout *parent_layout);

MetaWidget *render(
    meta::AttributeContainer &container,
    ContainerGroupPolicy      group_policy = ContainerGroupPolicy::CGP_SMART,
    const std::string        &root_group_name = "",
    QWidget                  *parent = nullptr);

} // namespace meta::qt