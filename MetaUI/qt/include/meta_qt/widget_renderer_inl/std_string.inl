/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QButtonGroup>
#include <QComboBox>
#include <QHBoxLayout>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<std::string>
{
  static MetaWidget *render(Attribute<std::string> &attr, QWidget *parent)
  {
    std::string       widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::vector<std::string> options = meta::common::allowed_values(attr);

    std::string &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QHBoxLayout *>(widget->layout());

    if (!label_txt.empty())
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);
    }

    if (widget_type.empty()) widget_type = "ComboBox"; // TODO Input

    if (widget_type == "ComboBox")
    {
      auto *combo = new QComboBox(widget);
      layout->addWidget(combo);

      int current_index = -1;

      for (size_t i = 0; i < options.size(); ++i)
      {
        combo->addItem(QString::fromStdString(options[i]));

        if (options[i] == value) current_index = static_cast<int>(i);
      }

      if (current_index >= 0) combo->setCurrentIndex(current_index);

      QObject::connect(combo,
                       &QComboBox::currentTextChanged,
                       widget,
                       [&value, widget](const QString &text)
                       {
                         value = text.toStdString();
                         Q_EMIT widget->value_changed();
                         Q_EMIT widget->edit_ended();
                       });
    }
    else if (widget_type == "ButtonGrid")
    {
      // --- Optional columns override

      int max_cols = 5;
      if (attr.metadata().contains("ui.columns"))
      {
        auto *c = attr.metadata().find("ui.columns");
        max_cols = std::any_cast<int>(c->to_any());
      }

      const int n = static_cast<int>(options.size());
      int ncols = std::min(max_cols, static_cast<int>(std::ceil(std::sqrt(n))));

      // --- Grid + button group

      auto *grid = new QGridLayout();
      auto *group = new QButtonGroup(widget);

      bool exclusive = true;
      if (attr.metadata().contains("ui.exclusive"))
      {
        auto *m2 = attr.metadata().find("ui.exclusive");
        exclusive = std::any_cast<bool>(m2->to_any());
      }

      group->setExclusive(exclusive);

      // --- Build buttons

      for (int i = 0; i < n; ++i)
      {
        const std::string &choice = options[i];

        auto *btn = new QPushButton(QString::fromStdString(choice), widget);
        btn->setCheckable(true);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        if (choice == value) btn->setChecked(true);

        group->addButton(btn);
        grid->addWidget(btn, i / ncols, i % ncols);
      }

      QObject::connect(
          group,
          QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
          widget,
          [&value, widget](QAbstractButton *button)
          {
            value = button->text().toStdString();
            Q_EMIT widget->value_changed();
            Q_EMIT widget->edit_ended();
          });

      layout->addLayout(grid);
    }
    else
    {
      layout->addWidget(
          make_error_widget(&attr, "unsupported widget type", widget));
    }

    return widget;
  }
};

} // namespace meta::qt
