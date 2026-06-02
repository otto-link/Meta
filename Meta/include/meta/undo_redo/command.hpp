/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file command.hpp
 * @brief Base interface for undo/redo commands.
 * @copyright Copyright (c) 2026
 */
#pragma once

namespace meta
{

/**
 * @brief Abstract interface for reversible operations.
 *
 * Commands implement undo/redo logic and can optionally support merging
 * with consecutive commands (e.g. continuous edits).
 */
class ICommand
{
public:
  virtual ~ICommand() = default;

  /// @brief Reverts the command effect.
  virtual void undo() = 0;

  /// @brief Applies the command effect.
  virtual void redo() = 0;

  /**
   * @brief Attempts to merge with another command.
   * @return true if merge succeeded, false otherwise.
   */
  virtual bool merge_with(const ICommand &other)
  {
    (void)other;
    return false;
  }
};

} // namespace meta