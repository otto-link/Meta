/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta_common.hpp"

#include "meta_qt/collapsible_section.hpp"
#include "meta_qt/container_widget.hpp"
#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widget_renderer.hpp"

namespace meta::qt
{

void insert_attribute(GroupNode         &root,
                      const std::string &path,
                      AbstractAttribute *attr)
{
  GroupNode *node = &root;

  std::stringstream ss(path);
  std::string       part;

  while (std::getline(ss, part, '/'))
  {
    auto &child = node->children[part];

    if (!child)
    {
      child = std::make_unique<GroupNode>();
      child->name = part;
    }

    node = child.get();
  }

  node->attributes.push_back(attr);
}

void render_group(GroupNode &node, QVBoxLayout *parent_layout)
{
  QVBoxLayout *current_layout = parent_layout;

  if (!node.name.empty())
  {
    auto *section = new CollapsibleSection(node.name.c_str());
    parent_layout->addWidget(section);

    current_layout = section->content_layout;
  }

  // --- Render attributes in this group

  for (auto *p_attr : node.attributes)
  {
    QWidget *w = meta::qt::render(p_attr);
    current_layout->addWidget(w);
  }

  // --- Render children groups

  for (auto &[name, child] : node.children)
    render_group(*child, current_layout);
}

MetaWidget *render(AttributeContainer &container,
                   QWidget            *parent,
                   const std::string  &root_group_name)
{
  // --- Build node tree

  GroupNode root;

  if (root_group_name.empty()) root.name = META_ROOT_GROUP;

  for (const auto &[name, sp_attr] : container)
  {
    auto             *attr = sp_attr.get();
    const std::string group = meta::common::group(*sp_attr);

    insert_attribute(root, group, attr);
  }

  // --- Render

  MetaWidget *widget = make_meta_widget_vbox(parent);
  auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

  render_group(root, layout);
  return widget;
}

} // namespace meta::qt
