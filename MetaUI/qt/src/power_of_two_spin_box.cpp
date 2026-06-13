/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include "meta_qt/widgets/power_of_two_spin_box.hpp"

namespace meta::qt
{

int helper_nearest_power_of_two(int v)
{
  if (v <= 1) return 1;

  int p = 1;
  while (p < v)
    p <<= 1;

  return p;
}

QAbstractSpinBox::StepEnabled PowerOfTwoSpinBox::stepEnabled() const
{
  return QSpinBox::stepEnabled();
}

void PowerOfTwoSpinBox::stepBy(int steps)
{
  int v = std::max(1, value());

  while (steps > 0)
  {
    if (v <= maximum() / 2)
      v <<= 1;
    else
      v = maximum();

    --steps;
  }

  while (steps < 0)
  {
    if (v > 1)
      v >>= 1;
    else
      v = 1;

    ++steps;
  }

  setValue(std::clamp(v, minimum(), maximum()));
}

QString PowerOfTwoSpinBox::textFromValue(int value) const
{
  return QString::number(value);
}

int PowerOfTwoSpinBox::valueFromText(const QString &text) const
{
  return helper_nearest_power_of_two(text.toInt());
}

} // namespace meta::qt
