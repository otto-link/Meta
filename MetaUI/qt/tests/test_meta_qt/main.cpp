#include <iostream>

#include <QApplication>

#include "meta.hpp"
#include "meta_qt.hpp"

int main(int argc, char *argv[])
{

  // --- Core / Non-GUI

  auto attr = meta::Attribute("download", true);
  attr.metadata().add("ui.label", std::string("Download Label"));
  // attr.metadata().add("ui.widget_type", std::string("Checkbox"));
  attr.metadata().add("ui.widget_type", std::string("Toggle"));

  auto attr_float = meta::Attribute("value", 0.f);
  attr_float.metadata().add("ui.min", -1.f);
  attr_float.metadata().add("ui.max", 3.f);
  attr_float.metadata().add("ui.step", 0.2f);

  auto attr_str = meta::Attribute("Text", std::string("Some Text"));

  // --- GUI

  QApplication app(argc, argv);

  QWidget *widget = meta::qt::render(attr);
  widget->show();

  return app.exec();

  std::cout << attr.to_string() << std::endl;
  std::cout << attr_float.to_string() << std::endl;
  std::cout << attr_str.to_string() << std::endl;
}
