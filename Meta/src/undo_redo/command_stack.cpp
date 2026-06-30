/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta/undo_redo/command_stack.hpp"

namespace meta
{

bool CommandStack::can_undo() const { return !undo_stack_.empty(); }

bool CommandStack::can_redo() const { return !redo_stack_.empty(); }

void CommandStack::clear()
{
  undo_stack_.clear();
  redo_stack_.clear();
}

void CommandStack::execute(std::unique_ptr<ICommand> command, bool allow_merge)
{
  if (!command) return;

  command->redo();

  if (allow_merge && !undo_stack_.empty())
  {
    if (undo_stack_.back()->merge_with(*command))
    {
      redo_stack_.clear();
      return;
    }
  }

  undo_stack_.push_back(std::move(command));
  redo_stack_.clear();
}

void CommandStack::undo()
{
  if (undo_stack_.empty()) return;

  auto command = std::move(undo_stack_.back());
  undo_stack_.pop_back();
  command->undo();
  redo_stack_.push_back(std::move(command));
}

void CommandStack::redo()
{
  if (redo_stack_.empty()) return;

  auto command = std::move(redo_stack_.back());
  redo_stack_.pop_back();
  command->redo();
  undo_stack_.push_back(std::move(command));
}

} // namespace meta
