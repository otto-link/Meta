/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file command_stack.hpp
 * @brief Undo/redo command stack managing reversible operations.
 * @copyright Copyright (c) 2026 Otto Link
 */
#pragma once

#include <memory>
#include <vector>

#include "meta/undo_redo/command.hpp"

namespace meta
{

/**
 * @brief Simple undo/redo stack based on ICommand.
 *
 * Stores executed commands and allows stepping backward
 * (undo) and forward (redo). Supports optional command
 * merging for continuous interactions (e.g. sliders).
 */
class CommandStack
{
public:
  /// @brief Returns true if an undo operation is possible.
  bool can_undo() const;

  /// @brief Returns true if a redo operation is possible.
  bool can_redo() const;

  /// @brief Clears both undo and redo history.
  void clear();

  /**
   * @brief Executes a command and stores it in the undo stack.
   * @param command Command to execute.
   * @param allow_merge If true, allows merging with previous command.
   */
  void execute(std::unique_ptr<ICommand> command, bool allow_merge = false);

  /// @brief Undoes the last executed command.
  void undo();

  /// @brief Redoes the last undone command.
  void redo();

private:
  std::vector<std::unique_ptr<ICommand>> _undo_stack;
  std::vector<std::unique_ptr<ICommand>> _redo_stack;
};

} // namespace meta