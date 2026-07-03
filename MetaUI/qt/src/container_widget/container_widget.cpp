/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <optional>
#include <regex>

#include "meta/macrologger.h"
#include "meta_common.hpp"

#include "meta_qt/container_widget.hpp"
#include "meta_qt/meta_widget.hpp"
#include "meta_qt/widget_renderer.hpp"
#include "meta_qt/widgets/collapsible_section.hpp"

namespace meta::qt
{

std::string compute_flattened_path(CategoryNode *node)
{
  std::string path;

  while (node)
  {
    if (!path.empty() && !node->name.empty()) path += "/";

    path += node->name;

    if (node->attributes.size() > 0) break;
    if (node->children.size() != 1) break;

    node = node->children.begin()->second.get();
  }

  return path;
}

void insert_attribute(CategoryNode      &root,
                      const std::string &path,
                      AbstractAttribute *attr)
{
  CategoryNode *node = &root;

  std::stringstream ss(path);
  std::string       part;

  while (std::getline(ss, part, '/'))
  {
    auto &child = node->children[part];

    if (!child)
    {
      child = std::make_unique<CategoryNode>();
      child->name = part;
    }

    node = child.get();
  }

  node->attributes.push_back(attr);
}

void render_flat(CategoryNode              &node,
                 QVBoxLayout               *layout,
                 std::vector<MetaWidget *> &collected_widgets)
{
  for (auto *p_attr : node.attributes)
  {
    MetaWidget *w = meta::qt::render(p_attr);
    layout->addWidget(w);
    collected_widgets.push_back(w);
  }

  for (auto &[name, child] : node.children)
    render_flat(*child, layout, collected_widgets);
}

void render_category(meta::AttributeContainer  &container,
                     CategoryNode              &node,
                     QVBoxLayout               *parent_layout,
                     std::vector<MetaWidget *> &collected_widgets)
{
  QVBoxLayout *current_layout = parent_layout;

  if (!node.name.empty())
  {
    const std::string title = node.name;

    auto *section = new CollapsibleSection(title.c_str());
    parent_layout->addWidget(section);

    // UI state management
    {
      auto *state = container.try_add(meta::keys::ui::state, true);
      state->metadata().try_add(meta::keys::ui::widget_type, "None");

      // apply stored state if available
      if (state->metadata().contains(title))
      {
        bool current_state = state->metadata().value<bool>(title);
        section->set_expanded(current_state);
      }

      QObject::connect(
          section,
          &CollapsibleSection::expanded_state_changed,
          [&container, title](bool new_state)
          {
            // add or get existing (dummy attribute used as a
            // metadata container)
            auto *state = container.try_add(meta::keys::ui::state, true);
            state->metadata().try_add(title, new_state)->value() = new_state;
          });
    }

    current_layout = section->content_layout;
  }

  for (auto *p_attr : node.attributes)
  {
    MetaWidget *w = meta::qt::render(p_attr);

    if (w) // avoid 'None' type widgets
    {
      current_layout->addWidget(w);
      collected_widgets.push_back(w);
    }
  }

  for (auto &[name, child] : node.children)
    render_category(container, *child, current_layout, collected_widgets);
}

void render_category_merged(meta::AttributeContainer        &container,
                            CategoryNode                    &node,
                            QVBoxLayout                     *parent_layout,
                            std::vector<MetaWidget *>       &collected_widgets,
                            const std::optional<std::regex> &collapse_regex)
{
  CategoryNode               *current = &node;
  std::vector<CategoryNode *> chain;

  while (current)
  {
    chain.push_back(current);
    if (current->children.size() != 1) break;
    current = current->children.begin()->second.get();
  }

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

  if (collapse_regex && std::regex_search(title, *collapse_regex))
    section->set_expanded(false);

  // UI state management
  {
    auto *state = container.try_add(meta::keys::ui::state, true);
    state->metadata().try_add(meta::keys::ui::widget_type, "None");

    // apply stored state if available
    if (state->metadata().contains(title))
    {
      bool current_state = state->metadata().value<bool>(title);
      section->set_expanded(current_state);
    }

    QObject::connect(
        section,
        &CollapsibleSection::expanded_state_changed,
        [&container, title](bool new_state)
        {
          // add or get existing (dummy attribute used as a
          // metadata container)
          auto *state = container.try_add(meta::keys::ui::state, true);
          state->metadata().try_add(title, new_state)->value() = new_state;
        });
  }

  parent_layout->addWidget(section);

  QVBoxLayout *layout = section->content_layout;

  for (auto *n : chain)
    for (auto *p_attr : n->attributes)
    {
      MetaWidget *w = meta::qt::render(p_attr);

      if (w) // 'None' widget is possible
      {
        layout->addWidget(w);
        collected_widgets.push_back(w);
      }
    }

  CategoryNode *last = chain.back();

  for (auto &[name, child] : last->children)
    render_category_merged(container,
                           *child,
                           layout,
                           collected_widgets,
                           collapse_regex);
}

MetaWidget *render(AttributeContainer    &container,
                   ContainerRenderOptions options,
                   QWidget               *parent)
{
  CategoryNode root;

  if (options.root_category_name.empty()) root.name = META_ROOT_CATEGORY;

  bool has_no_categorys = true;

  const std::vector<std::string> &order = options.insertion_order.empty()
                                              ? container.insertion_order()
                                              : options.insertion_order;

  for (const auto &name : order)
  {
    auto *attr = container.find(name);

    if (!attr)
    {
      LOG_ERROR("render: attribute '%s' not found in container, skipping",
                name.c_str());
      continue;
    }

    const std::string category = meta::common::category(*attr);
    insert_attribute(root, category, attr);
    has_no_categorys &= category.empty();
  }

  MetaWidget *container_widget = make_meta_widget_vbox(parent);
  auto       *layout = static_cast<QVBoxLayout *>(container_widget->layout());

  std::vector<MetaWidget *> collected_widgets;

  switch (options.category_policy)
  {
  case CategoryPolicy::CP_TREE:
    render_category(container, root, layout, collected_widgets);
    break;

  case CategoryPolicy::CP_MERGED:
    render_category_merged(container,
                           root,
                           layout,
                           collected_widgets,
                           options.collapse_regex);
    break;

  case CategoryPolicy::CP_SMART:
    if (has_no_categorys)
      render_flat(root, layout, collected_widgets);
    else
      render_category_merged(container,
                             root,
                             layout,
                             collected_widgets,
                             options.collapse_regex);
    break;

  case CategoryPolicy::CP_FLAT:
  default: render_flat(root, layout, collected_widgets); break;
  }

  for (MetaWidget *w : collected_widgets)
  {
    QObject::connect(w,
                     &MetaWidget::edit_started,
                     container_widget,
                     &MetaWidget::edit_started);

    QObject::connect(w,
                     &MetaWidget::value_changed,
                     container_widget,
                     &MetaWidget::value_changed);

    QObject::connect(w,
                     &MetaWidget::edit_ended,
                     container_widget,
                     &MetaWidget::edit_ended);
  }

  return container_widget;
}

} // namespace meta::qt
