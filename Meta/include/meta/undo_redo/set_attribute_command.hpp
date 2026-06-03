/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file set_attribute_command.hpp
 * @brief Undo/redo command that sets an Attribute value.
 * @copyright Copyright (c) 2026
 */
#pragma once

#include "meta/undo_redo/command.hpp"

namespace meta
{

/**
 * @brief Command that sets an Attribute value with undo/redo support.
 *
 * Stores both previous and new values and supports optional merging
 * for continuous edits (e.g. sliders or drag operations).
 */
template <typename T> class Attribute;

template <typename T> class SetAttributeCommand : public ICommand
{
public:
  /// @brief Creates a command changing an attribute value.
  SetAttributeCommand(Attribute<T> &attribute, const T &new_value)
      : _attribute(attribute),
        _old_value(attribute.value()),
        _new_value(new_value)
  {
  }

  /// @brief Restores the previous value.
  void undo() override { _attribute.value() = _old_value; }

  /// @brief Applies the new value.
  void redo() override { _attribute.value() = _new_value; }

  /// @brief Merges consecutive edits on the same attribute.
  bool merge_with(const ICommand &other) override
  {
    auto ptr = dynamic_cast<const SetAttributeCommand<T> *>(&other);

    if (!ptr)
    {
      return false;
    }

    if (&ptr->_attribute != &_attribute)
    {
      return false;
    }

    _new_value = ptr->_new_value;

    return true;
  }

private:
  Attribute<T> &_attribute;
  T             _old_value;
  T             _new_value;
};

} // namespace meta