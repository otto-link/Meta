/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QString>
#include <algorithm>
#include <cmath>
namespace meta::qt
{
// Ordinary values stay compact; small nonzero values must never read as zero.
inline QString display_float(float value, int decimals = 2)
{
  const double magnitude = std::abs(double(value));
  if (!std::isfinite(value)) return QString::number(value);
  decimals = std::clamp(decimals, 0, 8);
  if (magnitude == 0 || magnitude >= std::pow(10., -decimals))
    return QString::number(value == 0 ? 0 : value, 'f', decimals);
  if (magnitude < 1e-10) return QString::number(value, 'g', 6);
  const int precision = std::min(12,
                                 int(std::ceil(-std::log10(magnitude))) + 5);
  QString   text = QString::number(value, 'f', precision);
  while (text.endsWith('0') && text.size() - text.indexOf('.') - 1 > decimals)
    text.chop(1);
  return text;
}
} // namespace meta::qt
