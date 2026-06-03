#include <iostream>

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

#include "meta.hpp"
#include "meta_qt.hpp"

QWidget *make_debug_view(meta::AbstractAttribute *p_attr,
                         bool                     add_border = false)
{
  auto *base = new QWidget();

  auto *widget = meta::qt::render(p_attr);
  auto *label = new QLabel(
      QString::fromStdString("Value = " + p_attr->to_string()));

  auto *layout = new QVBoxLayout();
  layout->addWidget(widget);
  layout->addWidget(label);
  base->setLayout(layout);

  QObject::connect(widget,
                   &meta::qt::MetaWidget::value_changed,
                   base,
                   [widget, label, p_attr]() {
                     label->setText(QString::fromStdString(
                         "Value = " + p_attr->to_string()));
                   });

  if (add_border)
  {
    base->setStyleSheet("background: red;");
    widget->setStyleSheet("background: green;");
  }

  base->show();

  return base;
}

int main(int argc, char *argv[])
{

  // --- Core / Non-GUI

  meta::AttributeContainer container;

  {
    auto *a = container.add("download_checkbox", true);
    a->metadata().add("ui.label", std::string("Download Label"));
    a->metadata().add("ui.widget_type", std::string("Checkbox"));
  }

  {
    auto *a = container.add("download_toggle", true);
    a->metadata().add("ui.widget_type", std::string("BinaryButtons"));
    a->metadata().add("ui.label_true", std::string("True V"));
    a->metadata().add("ui.label_false", std::string("False V"));
  }

  {
    auto *a = container.add("value", 0.f);
    a->metadata().add("ui.min", -1.f);
    a->metadata().add("ui.max", 3.f);
    a->metadata().add("ui.step", 0.2f);
  }

  // --- GUI

  QApplication app(argc, argv);

  // UI
  bool add_border = false;

  for (const auto &[name, sp_attr] : container)
    make_debug_view(sp_attr.get(), add_border);

  return app.exec();
}
