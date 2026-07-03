/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/core/attribute_container.hpp"
#include "meta/metadata/keys.hpp"

namespace meta::presets
{

Attribute<float> &angle(AttributeContainer &c,
                        std::string_view    key,
                        std::string_view    label,
                        float               value)
{
  auto *a = c.add(std::string(key), value);
  a->value() = value;

  auto &m = a->metadata();

  m.add(keys::ui::widget_type, "SliderFloat");
  m.add(keys::ui::label, std::string(label));
  m.add(keys::ui::format, "{:.1f}°");
  m.add(keys::constraints::min, -180.f);
  m.add(keys::constraints::max, 180.f);
  m.add(keys::constraints::step, 0.1f);

  return *a;
}

} // namespace meta::presets
