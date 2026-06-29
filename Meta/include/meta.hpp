/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once

#include "meta/core/abstract_attribute.hpp"
#include "meta/core/attribute.hpp"
#include "meta/core/attribute_container.hpp"
#include "meta/core/container_group.hpp"
#include "meta/core/meta_object.hpp"

#include "meta/type/attribute_traits.hpp"
#include "meta/type/type_name.hpp"

#include "meta/serialization/attribute_factory.hpp"

#include "meta/undo_redo/command.hpp"
#include "meta/undo_redo/command_group.hpp"
#include "meta/undo_redo/command_stack.hpp"
#include "meta/undo_redo/set_attribute_command.hpp"

#include "meta/metadata/keys.hpp"
#include "meta/presets/numeric.hpp"

#ifdef META_ENABLE_COLOR_GRADIENT_TYPES
#include "meta/ext/color_gradient/color_gradient.hpp"
#endif