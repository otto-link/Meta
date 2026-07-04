/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <regex>

#include <QVBoxLayout>

#include "meta/core/abstract_attribute.hpp"
#include "meta/serialization/snapshot_manager.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

enum CategoryPolicy
{
  CP_FLAT,
  CP_TREE,
  CP_MERGED,
  CP_SMART
};

struct ContainerRenderOptions
{
  CategoryPolicy            category_policy = CategoryPolicy::CP_SMART;
  std::string               root_category_name = "";
  std::vector<std::string>  insertion_order = {};
  std::optional<std::regex> collapse_regex = std::nullopt;
};

struct CategoryNode
{
  std::string                                          name;
  std::vector<meta::AbstractAttribute *>               attributes;
  std::map<std::string, std::unique_ptr<CategoryNode>> children;
};

void insert_attribute(CategoryNode            &root,
                      const std::string       &path,
                      meta::AbstractAttribute *p_attr);

std::string compute_flattened_path(CategoryNode *node);

void render_flat(CategoryNode              &node,
                 QVBoxLayout               *layout,
                 std::vector<MetaWidget *> &collected_widgets);

void render_category(meta::AttributeContainer  &container,
                     CategoryNode              &node,
                     QVBoxLayout               *parent_layout,
                     std::vector<MetaWidget *> &collected_widgets);

void render_group_merged(meta::AttributeContainer  &container,
                         CategoryNode              &node,
                         QVBoxLayout               *parent_layout,
                         std::vector<MetaWidget *> &collected_widgets);

MetaWidget *render(meta::AttributeContainer &container,
                   ContainerRenderOptions    options = ContainerRenderOptions{},
                   SnapshotManager          *p_snapshot_manager = nullptr,
                   QWidget                  *parent = nullptr);

} // namespace meta::qt
