/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/undo_redo/command_group.hpp"

namespace meta
{

void CommandGroup::add(std::unique_ptr<ICommand> command)
{
  _commands.push_back(std::move(command));
}

void CommandGroup::redo()
{
  for (auto &command : _commands)
    command->redo();
}

void CommandGroup::undo()
{
  for (auto it = _commands.rbegin(); it != _commands.rend(); ++it)
  {
    (*it)->undo();
  }
}

} // namespace meta
