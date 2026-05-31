#include "meta.hpp"

namespace meta
{

float almost_unit_identity(float x) { return (2.0f - x) * x * x; }

double almost_unit_identity(double x) { return (2.0 - x) * x * x; }

float threshold(float x, float x0, float x1)
{
  if (x < x0) return 0.0f;
  if (x < x1) return (x - x0) / (x1 - x0);
  return 1.0f;
}

double threshold(double x, double x0, double x1)
{
  if (x < x0) return 0.0;
  if (x < x1) return (x - x0) / (x1 - x0);
  return 1.0;
}

} // namespace meta
