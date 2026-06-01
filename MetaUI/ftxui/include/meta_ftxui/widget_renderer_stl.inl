/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

namespace meta::tui
{

template <> struct WidgetRenderer<std::string>
{
  static ftxui::Component render(Attribute<std::string> &attr)
  {
    std::string &value = attr.value();
    InputOption  options;
    options.multiline = false;
    return ftxui::Input(&value, attr.name() + " ", options);
  }
};

} // namespace meta::tui