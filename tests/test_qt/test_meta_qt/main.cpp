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
  auto *label1 = new QLabel(
      QString::fromStdString("Value = " + p_attr->to_string()));
  auto *label2 = new QLabel(
      QString::fromStdString("Ended = " + p_attr->to_string()));

  auto *layout = new QVBoxLayout();
  layout->addWidget(widget);
  layout->addWidget(label1);
  layout->addWidget(label2);
  base->setLayout(layout);

  QObject::connect(widget,
                   &meta::qt::MetaWidget::value_changed,
                   base,
                   [widget, label1, p_attr]() {
                     label1->setText(QString::fromStdString(
                         "Value = " + p_attr->to_string()));
                   });

  QObject::connect(widget,
                   &meta::qt::MetaWidget::edit_ended,
                   base,
                   [widget, label2, p_attr]() {
                     label2->setText(QString::fromStdString(
                         "Ended = " + p_attr->to_string()));
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

  const bool base_bool = false;
  const bool base_float = false;
  const bool base_int = false;
  const bool base_string = false;

  const bool base_groups = true;

  // --- Bool

  if (base_bool)
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

  if (base_float)
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

  // --- Int

  if (base_int)
  {
    {
      auto *a = container.add("int_enumcombobox", 0);

      a->metadata().add(meta::keys::ui::widget_type, "EnumComboBox");

      std::vector<std::pair<int, std::string>> options = {{0, "Linear"},
                                                          {1, "Cubic"},
                                                          {2, "Bezier"}};
      a->metadata().add(meta::keys::constraints::enum_items, options);
    }

    {
      auto *a = container.add("int_input", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Input");
    }

    {
      auto *a = container.add("int_slider", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Slider");
      a->metadata().add(meta::keys::constraints::min, -1);
      a->metadata().add(meta::keys::constraints::max, 3);
    }

    {
      auto *a = container.add("int_scrollbar", 0);
      a->metadata().add(meta::keys::ui::widget_type, "ScrollBar");
      a->metadata().add(meta::keys::constraints::min, -1);
      a->metadata().add(meta::keys::constraints::max, 3);
    }

    {
      auto *a = container.add("int_dial", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Dial");
      a->metadata().add(meta::keys::constraints::min, -1);
      a->metadata().add(meta::keys::constraints::max, 3);
    }
  }

  // --- String

  if (base_string)
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

  // --- Groups

  if (base_groups)
  {
    {
      auto *a = container.add("a", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Input");
      a->metadata().add(meta::keys::ui::category, "Base/Other");
    }

    {
      auto *a = container.add("b", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Input");
      a->metadata().add(meta::keys::ui::category, "Base");
    }

    {
      auto *a = container.add("c", 0.1f);
      a->metadata().add(meta::keys::ui::widget_type, "Slider");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 3.f);
      a->metadata().add(meta::keys::ui::category, "Base/Other/SubTitle");
    }

    {
      auto *a = container.add("d", 0);
      a->metadata().add(meta::keys::ui::widget_type, "Input");
      a->metadata().add(meta::keys::ui::category, "Base/Something/Category");
    }

    {
      auto options = std::vector<std::string>{"Option A",
                                              "Option B",
                                              "Option C",
                                              "Option D"};

      auto *a = container.add("string_combobox", "Option B");
      a->metadata().add(meta::keys::ui::widget_type, "ButtonGrid");
      a->metadata().add(meta::keys::constraints::allowed_values, options);
      a->metadata().add(meta::keys::ui::category, "Base/Something/Category 2");
    }
  }

  // --- GUI

  QApplication app(argc, argv);

  if (false)
  {
    // UI
    bool add_border = false;

    for (const auto &[name, sp_attr] : container)
      make_debug_view(sp_attr.get(), add_border);
  }

  if (true)
  {
    meta::qt::MetaWidget *widget = meta::qt::render(
        container,
        meta::qt::CategoryPolicy::CP_MERGED);

    QObject::connect(widget,
                     &meta::qt::MetaWidget::value_changed,
                     []() { std::cout << "value_changed\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_ended,
                     []() { std::cout << "  > edit_changed\n"; });

    widget->show();
  }

  meta::ContainerGroup group; // watch for lifetime...

  if (true)
  {
    // Create multiple "views" / contexts
    auto &node_settings = group.add("node_settings");
    auto &ui_settings = group.add("ui_settings");
    auto &debug_settings = group.add("debug_settings");

    // Fill node settings
    node_settings.add("threshold", 0.5f);
    node_settings.add("iterations", 8);
    node_settings.add("active", true);

    // Fill UI settings
    ui_settings.add("theme", std::string("dark"));
    ui_settings.add("font_size", 14.f);
    ui_settings.add("show_grid", true);

    // Fill debug settings
    debug_settings.add("log_level", 2);
    debug_settings.add("wireframe", false);
    debug_settings.add("draw_bounds", true);

    meta::qt::MetaWidget *widget = meta::qt::render(
        group,
        meta::qt::CategoryPolicy::CP_MERGED);

    QObject::connect(widget,
                     &meta::qt::MetaWidget::value_changed,
                     []() { std::cout << "value_changed\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_ended,
                     []() { std::cout << "  > edit_changed\n"; });

    widget->show();
  }

  return app.exec();
}
