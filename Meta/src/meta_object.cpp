/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/core/meta_object.hpp"
#include "meta/core/attribute_container.hpp"

namespace meta
{

MetaObject::MetaObject() : _metadata(std::make_unique<AttributeContainer>()) {}

AttributeContainer &MetaObject::metadata() { return *_metadata; }

const AttributeContainer &MetaObject::metadata() const { return *_metadata; }

} // namespace meta
