/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <string>

namespace meta::common
{

int try_get_format_decimals(const std::string &format, int default_decimals)
{
  // try to infer precision from format strings such as "{:.3f}"
  int decimals = default_decimals;

  const auto pos1 = format.find("{:.");
  const auto pos2 = format.find('f');

  if (pos1 != std::string::npos && pos2 != std::string::npos && pos2 > pos1 + 3)
  {
    try
    {
      decimals = std::stoi(format.substr(pos1 + 3, pos2 - (pos1 + 3)));
    }
    catch (...)
    {
    }
  }

  return decimals;
}

} // namespace meta::common
