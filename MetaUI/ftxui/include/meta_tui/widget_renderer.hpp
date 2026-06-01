/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

namespace meta::tui
{

template <typename T> struct WidgetRenderer
{
  static ftxui::Component render(Attribute<T> &attr)
  {
    return ftxui::text("Unsupported type: " + attr.name());
  }
};

// helper
template <typename T> ftxui::Component render(Attribute<T> &attr)
{
  return WidgetRenderer<T>::render(attr);
}
// ---

template <> struct WidgetRenderer<bool>
{
  static ftxui::Component render(Attribute<bool> &attr)
  {
    bool             &value = attr.value();
    const std::string widget_type = meta::common::widget_type(attr);

    if (widget_type == "Toggle")
    {
      auto entries = std::make_shared<std::vector<std::string>>(
          std::vector<std::string>{"True", "False"});
      auto selected = std::make_shared<int>(value ? 0 : 1);

      Component toggle = ftxui::Toggle(entries.get(), selected.get());

      // wrap in a renderer that captures shared_ptrs and syncs back to value
      return ftxui::Renderer(toggle,
                             [entries,
                              selected,
                              &value,
                              toggle,
                              label = meta::common::label(attr)]()
                             {
                               value = (*selected == 0);
                               return ftxui::hbox({
                                   ftxui::text(label + " "),
                                   toggle->Render(),
                               });
                             });
    }
    else
    {
      return ftxui::Checkbox(meta::common::label(attr) + " ", &value);
    }
  }
};

template <> struct WidgetRenderer<float>
{
  static ftxui::Component render(Attribute<float> &attr)
  {
    float &value = attr.value();
    return ftxui::Slider(meta::common::label(attr) + " ",
                         &value,
                         meta::common::min(attr),
                         meta::common::max(attr),
                         meta::common::step(attr));
  }
};

} // namespace meta::tui

#ifdef META_ENABLE_STD_TYPES
#include "meta_tui/widget_renderer_stl.inl"
#endif
