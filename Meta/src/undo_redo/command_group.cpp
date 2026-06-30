/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/undo_redo/command_group.hpp"

namespace meta
{

void CommandGroup::add(std::unique_ptr<ICommand> command)
{
  commands_.push_back(std::move(command));
}

void CommandGroup::redo()
{
  for (auto &command : commands_)
    command->redo();
}

void CommandGroup::undo()
{
  for (auto it = commands_.rbegin(); it != commands_.rend(); ++it)
  {
    (*it)->undo();
  }
}

} // namespace meta
