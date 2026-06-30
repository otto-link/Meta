/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QToolButton>
#include <QVBoxLayout>

namespace meta::qt
{

class CollapsibleSection : public QWidget
{
  Q_OBJECT

public:
  explicit CollapsibleSection(const QString &title, QWidget *parent = nullptr);

  void set_expanded(bool new_state);

  QVBoxLayout *content_layout;

private:
  QToolButton *toggle_button;
  QWidget     *content;
};

} // namespace meta::qt