/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/macrologger.h"

#include "meta_common.hpp"

#include "meta_qt/collapsible_section.hpp"
#include "meta_qt/container_widget.hpp"
#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widget_renderer.hpp"

namespace meta::qt
{

std::string compute_flattened_path(GroupNode *node)
{
  std::string path;

  while (node)
  {
    if (!path.empty() && !node->name.empty()) path += "/";

    path += node->name;

    // stop if not a pure chain
    if (node->attributes.size() > 0) break;

    if (node->children.size() != 1) break;

    node = node->children.begin()->second.get();
  }

  return path;
}

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

void render_flat(GroupNode                 &node,
                 QVBoxLayout               *layout,
                 std::vector<MetaWidget *> &collected_widgets)
{
  // render attributes from this node
  for (auto *p_attr : node.attributes)
  {
    MetaWidget *w = meta::qt::render(p_attr);
    layout->addWidget(w);
    collected_widgets.push_back(w);
  }

  // recurse into children
  for (auto &[name, child] : node.children)
    render_flat(*child, layout, collected_widgets);
}

void render_group(GroupNode                 &node,
                  QVBoxLayout               *parent_layout,
                  std::vector<MetaWidget *> &collected_widgets)
{
  QVBoxLayout *current_layout = parent_layout;

  if (!node.name.empty())
  {
    auto *section = new CollapsibleSection(node.name.c_str());
    parent_layout->addWidget(section);

    current_layout = section->content_layout;
  }

  // --- render attributes in this group
  for (auto *p_attr : node.attributes)
  {
    MetaWidget *w = meta::qt::render(p_attr);
    current_layout->addWidget(w);
    collected_widgets.push_back(w);
  }

  // --- render children groups
  for (auto &[name, child] : node.children)
    render_group(*child, current_layout, collected_widgets);
}

void render_group_merged(GroupNode                 &node,
                         QVBoxLayout               *parent_layout,
                         std::vector<MetaWidget *> &collected_widgets)
{
  GroupNode               *current = &node;
  std::vector<GroupNode *> chain;

  // build flatten chain correctly
  while (current)
  {
    chain.push_back(current);
    if (current->children.size() != 1) break;
    current = current->children.begin()->second.get();
  }

  // build title
  std::string title;
  for (auto *n : chain)
  {
    if (!n->name.empty())
    {
      if (!title.empty()) title += "/";
      title += n->name;
    }
  }

  auto *section = new CollapsibleSection(title.c_str());
  parent_layout->addWidget(section);

  QVBoxLayout *layout = section->content_layout;

  // render attributes

  for (auto *n : chain)
    for (auto *p_attr : n->attributes)
    {
      MetaWidget *w = meta::qt::render(p_attr);
      layout->addWidget(w);
      collected_widgets.push_back(w);
    }

  // recurse ONLY from last node
  GroupNode *last = chain.back();

  for (auto &[name, child] : last->children)
    render_group_merged(*child, layout, collected_widgets);
}

MetaWidget *render(AttributeContainer  &container,
                   ContainerGroupPolicy group_policy,
                   const std::string   &root_group_name,
                   QWidget             *parent)
{
  // --- Build node tree

  GroupNode root;

  if (root_group_name.empty()) root.name = META_ROOT_GROUP;

  bool has_no_groups = true;

  for (const auto &[name, sp_attr] : container)
  {
    auto             *attr = sp_attr.get();
    const std::string group = meta::common::group(*sp_attr);
    insert_attribute(root, group, attr);

    has_no_groups &= group.empty();
  }

  // --- Render

  MetaWidget *container_widget = make_meta_widget_vbox(parent);
  auto       *layout = static_cast<QVBoxLayout *>(container_widget->layout());

  std::vector<MetaWidget *> collected_widgets;

  switch (group_policy)
  {
  case ContainerGroupPolicy::CGP_TREE:
  {
    render_group(root, layout, collected_widgets);
  }
  break;

  case ContainerGroupPolicy::CGP_MERGED:
  {
    render_group_merged(root, layout, collected_widgets);
  }
  break;

  case ContainerGroupPolicy::CGP_SMART:
  {
    // switch to flat view if no group is defined
    if (has_no_groups)
      render_flat(root, layout, collected_widgets);
    else
      render_group_merged(root, layout, collected_widgets);
  }
  break;

  case ContainerGroupPolicy::CGP_FLAT:
  default:
  {
    render_flat(root, layout, collected_widgets);
  }
  }

  // --- Pass-through signals

  for (MetaWidget *w : collected_widgets)
  {
    QObject::connect(w,
                     &MetaWidget::value_changed,
                     container_widget,
                     &MetaWidget::value_changed);

    QObject::connect(w,
                     &MetaWidget::edit_ended,
                     container_widget,
                     &MetaWidget::edit_ended);
  }

  //--- Output

  return container_widget;
}

} // namespace meta::qt
