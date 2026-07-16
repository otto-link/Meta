/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "meta/core/attribute.hpp"

#ifdef META_ENABLE_GLM_TYPES
#include <glm/glm.hpp>
#endif

#ifdef META_ENABLE_COLOR_GRADIENT_TYPES
#include "meta/ext/color_gradient/color_gradient.hpp"
#endif

namespace meta::presets
{

Attribute<float> &slider_float(AttributeContainer &c,
                               std::string_view    key,
                               std::string_view    label,
                               float               value,
                               float               vmin,
                               float               vmax,
                               std::string_view    format = "{:.3f}",
                               bool                log_scale = false);

Attribute<int> &slider_int(AttributeContainer &c,
                           std::string_view    key,
                           std::string_view    label,
                           int                 value,
                           int                 vmin,
                           int                 vmax,
                           std::string_view    format = "{}");

Attribute<bool> &checkbox(AttributeContainer &c,
                          std::string_view    key,
                          std::string_view    label,
                          bool                value);

Attribute<bool> &binary_buttons(AttributeContainer &c,
                                std::string_view    key,
                                std::string_view    label,
                                std::string_view    label_true,
                                std::string_view    label_false,
                                bool                value);

Attribute<int> &enum_choice(AttributeContainer                            &c,
                            std::string_view                              key,
                            std::string_view                              label,
                            const std::vector<std::pair<int, std::string>> &items,
                            int                                            value);

#ifdef META_ENABLE_GLM_TYPES

Attribute<glm::vec2> &wavenumber(AttributeContainer &c,
                                 std::string_view    key,
                                 std::string_view    label,
                                 glm::vec2           value,
                                 float               vmin,
                                 float               vmax,
                                 bool                link_xy,
                                 std::string_view    format = "{:.2f}");

Attribute<glm::vec2> &range(AttributeContainer &c,
                            std::string_view    key,
                            std::string_view    label,
                            glm::vec2           value,
                            float               vmin,
                            float               vmax,
                            bool                is_active,
                            std::string_view    format = "{:.3f}");

Attribute<glm::vec2> &xy(AttributeContainer &c,
                         std::string_view    key,
                         std::string_view    label,
                         glm::vec2           value,
                         float               xmin,
                         float               xmax,
                         float               ymin,
                         float               ymax);

Attribute<std::vector<glm::vec3>> &points(AttributeContainer &c,
                                          std::string_view    key,
                                          std::string_view    label,
                                          std::vector<glm::vec3> value = {});

Attribute<glm::vec4> &color(AttributeContainer &c,
                            std::string_view    key,
                            std::string_view    label,
                            glm::vec4           value);

#endif // META_ENABLE_GLM_TYPES

#ifdef META_ENABLE_COLOR_GRADIENT_TYPES

Attribute<ColorGradient> &color_gradient(AttributeContainer &c,
                                         std::string_view    key,
                                         std::string_view    label,
                                         ColorGradient       value = {});

#endif // META_ENABLE_COLOR_GRADIENT_TYPES

Attribute<std::filesystem::path> &file(AttributeContainer   &c,
                                       std::string_view       key,
                                       std::string_view       label,
                                       std::filesystem::path  value,
                                       std::string_view       filter,
                                       bool                   for_saving);

Attribute<std::string> &text(AttributeContainer &c,
                             std::string_view    key,
                             std::string_view    label,
                             std::string         value,
                             bool                read_only = false);

Attribute<std::string> &string_choice(AttributeContainer            &c,
                                      std::string_view                key,
                                      std::string_view                label,
                                      const std::vector<std::string> &choices,
                                      std::string                     value,
                                      bool                             use_combo = true);

Attribute<std::vector<float>> &curve(AttributeContainer &c,
                                     std::string_view    key,
                                     std::string_view    label,
                                     std::vector<float>  value,
                                     float               vmin,
                                     float               vmax);

} // namespace meta::presets
