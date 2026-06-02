#include <iomanip>
#include <iostream>
#include <memory>

#include "meta.hpp"

using namespace meta;

static void print_state(const Attribute<float> &attr, const std::string &label)
{
  std::cout << std::left << std::setw(38) << label << " -> " << attr.to_string()
            << "\n";
}

static void separator(const std::string &title)
{
  std::cout << "\n=== " << title << " ===\n";
}

int main()
{
  Attribute<float> attr("f", 1.f);
  CommandStack     stack;

  separator("Initial state");
  print_state(attr, "attr");

  separator("Basic execute / undo / redo");

  stack.execute(std::make_unique<SetAttributeCommand<float>>(attr, 2.f));
  print_state(attr, "after set 2");

  stack.execute(std::make_unique<SetAttributeCommand<float>>(attr, 3.f));
  print_state(attr, "after set 3");

  stack.undo();
  print_state(attr, "undo (back to 2)");

  stack.redo();
  print_state(attr, "redo (back to 3)");

  stack.redo(); // no-op
  print_state(attr, "redo (no-op)");

  stack.undo();
  stack.undo();
  stack.undo(); // no-op
  print_state(attr, "undo x3 (clamped) (back to 1)");

  separator("Merging test");

  bool allow_merge = true;

  stack.execute(std::make_unique<SetAttributeCommand<float>>(attr, 3.f),
                allow_merge);
  stack.execute(std::make_unique<SetAttributeCommand<float>>(attr, 4.f),
                allow_merge);
  stack.execute(std::make_unique<SetAttributeCommand<float>>(attr, 5.f),
                allow_merge);

  print_state(attr, "after merged sequence (3 -> 4 -> 5)");

  stack.undo();
  print_state(attr, "undo merged command (back to 1)");

  return 0;
}
