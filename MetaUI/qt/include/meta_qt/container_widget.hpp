/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QVBoxLayout>

#include "meta/core/abstract_attribute.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

struct GroupNode
{
  std::string name;

  std::vector<meta::AbstractAttribute *> attributes;

  std::map<std::string, std::unique_ptr<GroupNode>> children;
};

void insert_attribute(GroupNode               &root,
                      const std::string       &path,
                      meta::AbstractAttribute *p_attr);

void render_group(GroupNode &node, QVBoxLayout *parent_layout);

MetaWidget *render(meta::AttributeContainer &container,
                   QWidget                  *parent = nullptr,
                   const std::string        &root_group_name = "");

} // namespace meta::qt