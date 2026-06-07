#include <iostream>

#include <QApplication>
#include <QLabel>
#include <QVBoxLayout>

#include "meta.hpp"
#include "meta_qt.hpp"

// --- Qt helper

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

  // --- Bool

  if (true)
  {
    {
      auto *a = container.add("bool_toggle", true);
      a->metadata().add(meta::keys::ui::label, "Button Label");
      a->metadata().add(meta::keys::ui::widget_type, "Toggle");
    }

    {
      auto *a = container.add("bool_checkbox", true);
      a->metadata().add(meta::keys::ui::label, "Button Label");
      a->metadata().add(meta::keys::ui::widget_type, "Checkbox");
    }

    {
      auto *a = container.add("bool_binary_buttons", true);
      a->metadata().add(meta::keys::ui::widget_type, "BinaryButtons");
      a->metadata().add(meta::keys::ui::label, "Button Label");
      a->metadata().add(meta::keys::ui::label_true, "True V");
      a->metadata().add(meta::keys::ui::label_false, "False V");
    }
  }

  // --- Float

  if (false)
  {
    {
      auto *a = container.add("float_input", 0.f);
      a->metadata().add(meta::keys::ui::widget_type, "Input");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::ui::format, "{:.3f}");
    }

    {
      auto *a = container.add("float_slider", 0.f);
      a->metadata().add(meta::keys::ui::widget_type, "Slider");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 3.f);
      a->metadata().add(meta::keys::constraints::step, 0.2f);
      a->metadata().add(meta::keys::ui::format, "{:.2f}");
    }

    {
      auto *a = container.add("float_scrollbar", 0.f);
      a->metadata().add(meta::keys::ui::widget_type, "ScrollBar");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 3.f);
      a->metadata().add(meta::keys::constraints::step, 0.2f);
    }

    {
      auto *a = container.add("float_dial", 0.f);
      a->metadata().add(meta::keys::ui::widget_type, "Dial");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 3.f);
    }
  }

  // --- String

  if (true)
  {
    auto options = std::vector<std::string>{"Option A",
                                            "Option B",
                                            "Option C",
                                            "Option D"};

    {
      auto *a = container.add("string_combobox", "Option B");
      a->metadata().add(meta::keys::ui::widget_type, "ComboBox");
      a->metadata().add(meta::keys::constraints::allowed_values, options);
    }

    {
      auto *a = container.add("string_buttongrid", "Option B");
      a->metadata().add(meta::keys::ui::widget_type, "ButtonGrid");
      a->metadata().add(meta::keys::constraints::allowed_values, options);
    }
  }

  // --- GUI

  QApplication app(argc, argv);

  // UI
  bool add_border = false;

  for (const auto &[name, sp_attr] : container)
    make_debug_view(sp_attr.get(), add_border);

  return app.exec();
}
