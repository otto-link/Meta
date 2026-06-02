/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/undo_redo/command_stack.hpp"

namespace meta
{

bool CommandStack::can_undo() const { return !_undo_stack.empty(); }

bool CommandStack::can_redo() const { return !_redo_stack.empty(); }

void CommandStack::clear()
{
  _undo_stack.clear();
  _redo_stack.clear();
}

void CommandStack::execute(std::unique_ptr<ICommand> command, bool allow_merge)
{
  if (!command) return;

  command->redo();

  if (allow_merge && !_undo_stack.empty())
  {
    if (_undo_stack.back()->merge_with(*command))
    {
      _redo_stack.clear();
      return;
    }
  }

  _undo_stack.push_back(std::move(command));
  _redo_stack.clear();
}

void CommandStack::undo()
{
  if (_undo_stack.empty()) return;

  auto command = std::move(_undo_stack.back());
  _undo_stack.pop_back();
  command->undo();
  _redo_stack.push_back(std::move(command));
}

void CommandStack::redo()
{
  if (_redo_stack.empty()) return;

  auto command = std::move(_redo_stack.back());
  _redo_stack.pop_back();
  command->redo();
  _undo_stack.push_back(std::move(command));
}

} // namespace meta
