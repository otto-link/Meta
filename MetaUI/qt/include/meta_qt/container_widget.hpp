/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QVBoxLayout>

#include "meta/core/abstract_attribute.hpp"

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

struct CategoryNode
{
  std::string name;

  std::vector<meta::AbstractAttribute *> attributes;

  std::map<std::string, std::unique_ptr<CategoryNode>> children;
};

void insert_attribute(CategoryNode            &root,
                      const std::string       &path,
                      meta::AbstractAttribute *p_attr);

std::string compute_flattened_path(CategoryNode *node);

void render_flat(CategoryNode              &node,
                 QVBoxLayout               *layout,
                 std::vector<MetaWidget *> &collected_widgets);

void render_group(CategoryNode              &node,
                  QVBoxLayout               *parent_layout,
                  std::vector<MetaWidget *> &collected_widgets);

void render_group_merged(CategoryNode              &node,
                         QVBoxLayout               *parent_layout,
                         std::vector<MetaWidget *> &collected_widgets);

MetaWidget *render(meta::AttributeContainer &container,
                   CategoryPolicy     group_policy = CategoryPolicy::CP_SMART,
                   const std::string &root_group_name = "",
                   QWidget           *parent = nullptr);

} // namespace meta::qt