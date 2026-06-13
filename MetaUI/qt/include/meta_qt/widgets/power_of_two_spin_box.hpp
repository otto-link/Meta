/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QSpinBox>

namespace meta::qt
{

class PowerOfTwoSpinBox : public QSpinBox
{
public:
  using QSpinBox::QSpinBox;

protected:
  void        stepBy(int steps) override;
  StepEnabled stepEnabled() const override;
  QString     textFromValue(int value) const override;
  int         valueFromText(const QString &text) const override;
};

} // namespace meta::qt