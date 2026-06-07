#include <iostream>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

using namespace ftxui;

#include "meta.hpp"
#include "meta_tui.hpp"

int main()
{
  auto attr = meta::Attribute("download", false);
  attr.metadata().add("ui.label", std::string("Download Label"));
  // attr.metadata().add("ui.widget_type", std::string("Checkbox"));
  attr.metadata().add("ui.widget_type", std::string("Toggle"));

  auto attr_float = meta::Attribute("value", 0.f);
  attr_float.metadata().add("contraints.min", -1.f);
  attr_float.metadata().add("contraints.max", 3.f);
  attr_float.metadata().add("contraints.step", 0.2f);

  auto attr_str = meta::Attribute("Text", std::string("Some Text"));

  auto container = Container::Vertical({
      meta::tui::render(attr),
      meta::tui::render(attr_float),
      meta::tui::render(attr_str),
  });

  auto screen = ScreenInteractive::FitComponent();
  screen.Loop(container);

  std::cout << attr.to_string() << std::endl;
  std::cout << attr_float.to_string() << std::endl;
  std::cout << attr_str.to_string() << std::endl;

  return 0;
}
