/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */

/**
 * @file command_group.hpp
 * @brief Composite command allowing multiple operations to be treated as one.
 * @copyright Copyright (c) 2026
 */
#pragma once

#include <memory>
#include <vector>

#include "meta/undo_redo/command.hpp"

namespace meta
{

/**
 * @brief Command that groups multiple commands into a single undo/redo step.
 */
class CommandGroup : public ICommand
{
public:
  /// @brief Adds a command to the group.
  void add(std::unique_ptr<ICommand> command);

  /// @brief Executes all commands in forward order.
  void redo() override;

  /// @brief Reverts all commands in reverse order.
  void undo() override;

private:
  std::vector<std::unique_ptr<ICommand>> commands_;
};

} // namespace meta