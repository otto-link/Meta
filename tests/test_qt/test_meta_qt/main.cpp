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
                   &meta::qt::MetaWidget::edit_ended,
                   base,
                   [widget, label2, p_attr]()
                   {
                     label2->setText(QString::fromStdString(
                         "Started = " + p_attr->to_string()));
                   });

  QObject::connect(widget,
                   &meta::qt::MetaWidget::value_changed,
                   base,
                   [widget, label1, p_attr]()
                   {
                     label1->setText(QString::fromStdString(
                         " - Value = " + p_attr->to_string()));
                   });

  QObject::connect(widget,
                   &meta::qt::MetaWidget::edit_ended,
                   base,
                   [widget, label2, p_attr]()
                   {
                     label2->setText(QString::fromStdString(
                         "   + Ended = " + p_attr->to_string()));
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
  const bool base_int = true;
  const bool base_string = false;

#ifdef META_ENABLE_STD_TYPES
  const bool base_std_filesystem_path = false;
  const bool base_std_vector_float = false;
#endif

#ifdef META_ENABLE_GLM_TYPES
  const bool base_glm_ivec = false;
  const bool base_glm_vec = false;
#endif

  const bool base_groups = false;

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

    {
      auto *a = container.add("float_slider_custome", 0.f);
      a->metadata().add(meta::keys::ui::widget_type, "SliderFloat");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 3.f);
      a->metadata().add(meta::keys::constraints::step, 0.2f);
      a->metadata().add(meta::keys::ui::format, "{:.2f}");
      a->metadata().add("ui.plus_minus", true);
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

    {
      auto *a = container.add("int_slider_custome", 0);
      a->metadata().add(meta::keys::ui::widget_type, "SliderInt");
      a->metadata().add(meta::keys::constraints::min, -1);
      // a->metadata().add(meta::keys::constraints::max, INT_MAX);
      a->metadata().add(meta::keys::constraints::max, 10);
      a->metadata().add("ui.plus_minus", true);
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

    {
      auto *a = container.add("string_single_line", "");
      a->metadata().add(meta::keys::ui::widget_type, "SingleLineText");
      a->metadata().add("placeholder", "Text goes here...");
    }

    {
      auto *a = container.add("string_multiline", "");
      a->metadata().add(meta::keys::ui::widget_type, "MultilineText");
      a->metadata().add("ui.placeholder", "Text goes here...");
      a->metadata().add("ui.min_lines", 12);
    }

    {
      auto *a = container.add("string_editor", "");
      a->metadata().add(meta::keys::ui::widget_type, "CodeEditor");
      a->metadata().add("ui.min_lines", 24);
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

  // --- std

#ifdef META_ENABLE_STD_TYPES

  if (base_std_filesystem_path)
  {
    {
      auto *a = container.add("std::filesystem::path_open",
                              std::filesystem::path());
      a->metadata().add(meta::keys::ui::widget_type, "OpenFile");
      a->metadata().add("ui.start_dir", ".");
    }

    {
      auto *a = container.add("std::filesystem::path_save",
                              std::filesystem::path("./some_file"));
      a->metadata().add(meta::keys::ui::widget_type, "SaveFile");
    }

    {
      auto *a = container.add("std::filesystem::path_dir",
                              std::filesystem::path());
      a->metadata().add(meta::keys::ui::widget_type, "Directory");
    }
  }

  if (base_std_vector_float)
  {
    {
      std::vector<float> values = {0.f, 0.1f, 0.75f, 0.5f};
      container.add("std::vector::float", values);
    }
  }

#endif

  // --- glm::ivec2

#ifdef META_ENABLE_GLM_TYPES

  if (base_glm_ivec)
  {
    {
      container.add("glm::ivec2_free", glm::ivec2(16, 32));
    }

    {
      auto *a = container.add("glm::ivec2_constrained", glm::ivec2(16, 32));
      a->metadata().add(meta::keys::constraints::min, 16);
      a->metadata().add(meta::keys::constraints::max, int(std::pow(2, 16)));
      a->metadata().add(meta::keys::constraints::power_of_two, true);
      a->metadata().add(meta::keys::constraints::aspect_ratio, 4.f);
    }
  }

  if (base_glm_vec)
  {
    {
      container.add("glm::vec2_free", glm::vec2(16, 32));
    }

    {
      auto *a = container.add("glm::vec2_constrained", glm::vec2(16.f, 32.f));
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 64.f);
      a->metadata().add(meta::keys::constraints::step, 0.1f);
      a->metadata().add(meta::keys::ui::format, "{:.2f}");
    }

    {
      auto *a = container.add("glm::vec2_xy", glm::vec2(16.f, 32.f));
      a->metadata().add(meta::keys::ui::widget_type, "XYCanvas");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 64.f);
    }

    {
      auto *a = container.add("glm::vec2_range", glm::vec2(0.f, 1.f));
      a->metadata().add(meta::keys::ui::widget_type, "RangeBar");
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 2.f);
      a->metadata().add(meta::keys::constraints::step, 0.1f);
      a->metadata().add(meta::keys::ui::format, "{:.3f}");
    }

    {
      container.add("glm::vec3", glm::vec3(16.f, 32.f, 64.f));
    }

    {
      auto *a = container.add("glm::vec3_constrained",
                              glm::vec3(16.f, 32.f, 64.f));
      a->metadata().add(meta::keys::constraints::min, -1.f);
      a->metadata().add(meta::keys::constraints::max, 64.f);
      a->metadata().add(meta::keys::constraints::step, 0.1f);
      a->metadata().add(meta::keys::ui::format, "{:.1f}");
    }

    {
      auto *a = container.add("glm::vec3_color", glm::vec3(0.5f, 0.1f, 0.f));
      a->metadata().add(meta::keys::ui::widget_type, "ColorPicker");
    }

    {
      container.add("glm::vec4", glm::vec4(16.f, 32.f, 64.f, 128.f));
    }

    {
      auto *a = container.add("glm::vec4_color",
                              glm::vec4(0.5f, 0.1f, 0.f, 0.5f));
      a->metadata().add(meta::keys::ui::widget_type, "ColorPicker");
    }
  }

#endif

  // --- GUI

  QApplication app(argc, argv);

  if (true)
  {
    // UI
    bool add_border = false;

    for (const auto &[name, sp_attr] : container)
      make_debug_view(sp_attr.get(), add_border);
  }

  if (false)
  {
    meta::qt::MetaWidget *widget = meta::qt::render(
        container,
        meta::qt::CategoryPolicy::CP_MERGED);

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_started,
                     []() { std::cout << "+ edit_started\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::value_changed,
                     []() { std::cout << "  - value_changed\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_ended,
                     []() { std::cout << "    > edit_changed\n"; });

    widget->show();
  }

  meta::ContainerGroup group; // watch for lifetime...

  if (false)
  {
    // Create multiple "views" / contexts
    auto &node_settings = group.add("node_settings");
    auto &ui_settings = group.add("ui_settings");
    auto &debug_settings = group.add("debug_settings");

    // Fill node settings
    {
      auto *a = node_settings.add("threshold", 0.5f);
      a->metadata().add(meta::keys::constraints::min, 0.f);
      a->metadata().add(meta::keys::constraints::max, 5.f);
      a->metadata().add(meta::keys::ui::widget_type, "Slider");
      a->metadata().add(meta::keys::ui::category, "Base/Something/Category 2");
    }

    {
      auto *a = node_settings.add("iterations", 8);
      a->metadata().add(meta::keys::ui::category, "Base/Cat 1");
    }

    node_settings.add("active", true);

    // Fill UI settings
    ui_settings.add("theme", std::string("dark"));
    ui_settings.add("font_size", 14.f);
    ui_settings.add("show_grid", true);

    // Fill debug settings
    debug_settings.add("log_level", 2);
    debug_settings.add("wireframe", false);
    debug_settings.add("draw_bounds", true);

    meta::presets::seed(debug_settings, "seed", "Random Seed");

    // generate widget

    meta::qt::MetaWidget *widget = meta::qt::render(
        group,
        meta::qt::CategoryPolicy::CP_MERGED);

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_started,
                     []() { std::cout << "+ edit_started\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::value_changed,
                     []() { std::cout << "  - value_changed\n"; });

    QObject::connect(widget,
                     &meta::qt::MetaWidget::edit_ended,
                     []() { std::cout << "    > edit_changed\n"; });

    widget->show();
  }

  return app.exec();
}
