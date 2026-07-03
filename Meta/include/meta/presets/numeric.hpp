/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include "meta/core/attribute.hpp"

namespace meta::presets
{

Attribute<float> &angle(AttributeContainer &c,
                        std::string_view    key,
                        std::string_view    label,
                        float               value = 0);

Attribute<int> &seed(AttributeContainer &c,
                     std::string_view    key,
                     std::string_view    label,
                     int                 value = 0);

} // namespace meta::presets