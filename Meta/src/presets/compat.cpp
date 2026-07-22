/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/core/attribute_container.hpp"
#include "meta/metadata/keys.hpp"

#ifdef META_ENABLE_COLOR_GRADIENT_TYPES
#include "meta/ext/color_gradient/color_gradient.hpp"
#endif

namespace meta::presets
{

Attribute<float> &slider_float(AttributeContainer &c, std::string_view key,
                               std::string_view label, float value, float vmin, float vmax,
                               std::string_view format, bool log_scale)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "SliderFloat");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::format, std::string(format));
  m.add(keys::constraints::min, vmin);
  m.add(keys::constraints::max, vmax);
  if (log_scale)
    m.add(keys::ui::log_scale, true); // read by MetaUI float.inl:36
  return *a;
}

Attribute<int> &slider_int(AttributeContainer &c, std::string_view key, std::string_view label,
                           int value, int vmin, int vmax, std::string_view format)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "SliderInt");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::format, std::string(format));
  m.add(keys::constraints::min, vmin);
  m.add(keys::constraints::max, vmax);
  return *a;
}

Attribute<bool> &checkbox(AttributeContainer &c, std::string_view key, std::string_view label,
                          bool value)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "Checkbox");
  m.add(keys::ui::label, std::string(label));
  return *a;
}

Attribute<bool> &binary_buttons(AttributeContainer &c, std::string_view key,
                                std::string_view label, std::string_view label_true,
                                std::string_view label_false, bool value)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "BinaryButtons");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::label_true, std::string(label_true));
  m.add(keys::ui::label_false, std::string(label_false));
  return *a;
}

Attribute<int> &enum_choice(AttributeContainer &c, std::string_view key, std::string_view label,
                            const std::vector<std::pair<int, std::string>> &items, int value)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "EnumComboBox");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::constraints::enum_items, items);
  return *a;
}

#ifdef META_ENABLE_GLM_TYPES

Attribute<glm::vec2> &wavenumber(AttributeContainer &c, std::string_view key,
                                 std::string_view label, glm::vec2 value, float vmin, float vmax,
                                 bool link_xy, std::string_view format)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "LinkedSliders");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::format, std::string(format));
  m.add(keys::ui::locked_xy, link_xy); // read by MetaUI glm_vec2.inl:30
  m.add(keys::constraints::min, vmin);
  m.add(keys::constraints::max, vmax);
  return *a;
}

Attribute<glm::vec2> &range(AttributeContainer &c, std::string_view key, std::string_view label,
                            glm::vec2 value, float vmin, float vmax, bool is_active,
                            std::string_view format)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "RangeBar");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::format, std::string(format));
  m.add(keys::constraints::min, vmin);
  m.add(keys::constraints::max, vmax);
  // NOTE: MetaUI's RangeBar (glm_vec2.inl) currently derives "active" from a
  // value sentinel ({-1, 0} == disabled), not from metadata keys. These two
  // keys are forward-looking metadata for Task 2, which is expected to teach
  // the renderer to read them instead of/in addition to the sentinel.
  m.add(keys::ui::has_active_toggle, true); // Task 2 renders the checkbox
  m.add(keys::ui::active, is_active);
  return *a;
}

Attribute<glm::vec2> &xy(AttributeContainer &c, std::string_view key, std::string_view label,
                         glm::vec2 value, float xmin, float xmax, float ymin, float ymax)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "XYCanvas");
  m.add(keys::ui::label, std::string(label));
  // MetaUI's XYCanvas (glm_vec2.inl) reads a single shared bound pair via
  // meta::common::min<float>/max<float>, i.e. keys::constraints::min/max,
  // and applies it to both axes -- it does not currently read per-axis
  // bound keys. We set the shared bounds from the x-axis range so the
  // widget is functional today, and additionally stash the full per-axis
  // range under ad-hoc keys for a future asymmetric-bounds XYCanvas.
  m.add(keys::constraints::min, xmin);
  m.add(keys::constraints::max, xmax);
  m.add(keys::ui::min_x, xmin); // not yet read by XYCanvas renderer
  m.add(keys::ui::max_x, xmax); // not yet read by XYCanvas renderer
  m.add(keys::ui::min_y, ymin); // not yet read by XYCanvas renderer
  m.add(keys::ui::max_y, ymax); // not yet read by XYCanvas renderer
  return *a;
}

Attribute<std::vector<glm::vec3>> &points(AttributeContainer &c, std::string_view key,
                                          std::string_view label, std::vector<glm::vec3> value)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "PointsEditor");
  m.add(keys::ui::label, std::string(label));
  return *a;
}

Attribute<glm::vec4> &color(AttributeContainer &c, std::string_view key, std::string_view label,
                            glm::vec4 value)
{
  auto *a = c.add(std::string(key), value);
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "ColorPicker");
  m.add(keys::ui::label, std::string(label));
  return *a;
}

#endif // META_ENABLE_GLM_TYPES

#ifdef META_ENABLE_COLOR_GRADIENT_TYPES

Attribute<ColorGradient> &color_gradient(AttributeContainer &c, std::string_view key,
                                         std::string_view label, ColorGradient value)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "GradientEditor");
  m.add(keys::ui::label, std::string(label));
  return *a;
}

#endif // META_ENABLE_COLOR_GRADIENT_TYPES

Attribute<std::filesystem::path> &file(AttributeContainer &c, std::string_view key,
                                       std::string_view label, std::filesystem::path value,
                                       std::string_view filter, bool for_saving)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, for_saving ? "SaveFile" : "OpenFile");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::constraints::file_filter, std::string(filter));
  return *a;
}

Attribute<std::string> &text(AttributeContainer &c, std::string_view key, std::string_view label,
                             std::string value, bool read_only)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "SingleLineText");
  m.add(keys::ui::label, std::string(label));
  if (read_only)
    // NOTE: MetaUI's SingleLineText renderer (std_string.inl) does not
    // currently read a "ui.read_only" key -- keeping the metadata anyway
    // per the migration plan, for a future renderer update.
    m.add(keys::ui::read_only, true);
  return *a;
}

Attribute<std::string> &string_choice(AttributeContainer &c, std::string_view key,
                                      std::string_view label,
                                      const std::vector<std::string> &choices, std::string value,
                                      bool use_combo)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, use_combo ? "ComboBox" : "ButtonGrid");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::constraints::allowed_values, choices);
  return *a;
}

Attribute<std::vector<float>> &curve(AttributeContainer &c, std::string_view key,
                                     std::string_view label, std::vector<float> value,
                                     float vmin, float vmax)
{
  auto *a = c.add(std::string(key), std::move(value));
  auto &m = a->metadata();
  m.add(keys::ui::widget_type, "CurveEditor");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::min_y, vmin);
  m.add(keys::ui::max_y, vmax);
  return *a;
}

} // namespace meta::presets
