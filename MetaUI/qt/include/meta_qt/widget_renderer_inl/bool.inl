/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<bool>
{
  static MetaWidget *render(Attribute<bool> &attr, QWidget *parent)
  {
    const std::string widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    bool             &value = attr.value();

    MetaWidget *widget = make_meta_widget_grid(parent);
    auto       *layout = dynamic_cast<QGridLayout *>(widget->layout());

    if (widget_type == "Toggle")
    {
      // --- TOGGLE

      auto *button = new QPushButton(label_txt.c_str(), widget);
      layout->addWidget(button, 0, 0);

      button->setCheckable(true);
      button->setChecked(value);

      widget->connect(button,
                      &QPushButton::toggled,
                      widget,
                      [widget, &value](bool checked)
                      {
                        value = checked;
                        Q_EMIT widget->value_changed();
                      });
    }
    else if (widget_type == "BinaryButtons")
    {
      // --- BINARY BUTTONS

      const std::string label_true = meta::common::try_get_string(
          attr,
          "ui.label_true",
          "True");
      const std::string label_false = meta::common::try_get_string(
          attr,
          "ui.label_false",
          "False");

      auto *label = new QLabel(label_txt.c_str(), widget);
      auto *button_true = new QPushButton(label_true.c_str(), widget);
      auto *button_false = new QPushButton(label_false.c_str(), widget);

      layout->addWidget(label, 0, 0, 1, 2);
      layout->addWidget(button_true, 1, 0);
      layout->addWidget(button_false, 1, 1);

      // make the buttons checkable
      button_true->setCheckable(true);
      button_false->setCheckable(true);

      // set the initial state of the buttons based on the attribute value
      button_true->setChecked(value);
      button_false->setChecked(!value);

      // connect the buttons' clicked signals to update the state
      widget->connect(button_true,
                      &QPushButton::clicked,
                      widget,
                      [widget, button_true, button_false, &value]()
                      {
                        if (button_true->isChecked())
                        {
                          button_false->setChecked(false);
                          value = true;
                          Q_EMIT widget->value_changed();
                        }
                        else
                        {
                          // ensure at least one button is always checked
                          button_true->setChecked(true);
                        }
                      });

      widget->connect(button_false,
                      &QPushButton::clicked,
                      widget,
                      [widget, button_true, button_false, &value]()
                      {
                        if (button_false->isChecked())
                        {
                          button_true->setChecked(false);
                          value = false;
                          Q_EMIT widget->value_changed();
                        }
                        else
                        {
                          button_false->setChecked(true);
                        }
                      });
    }
    else if (widget_type == "Checkbox")
    {
      // --- CHECKBOX

      QCheckBox *checkbox = new QCheckBox(label_txt.c_str(), widget);
      layout->addWidget(checkbox, 0, 0);

      checkbox->setChecked(value);
      checkbox->connect(checkbox,
                        &QCheckBox::toggled,
                        checkbox,
                        [widget, checkbox, &value](bool checked)
                        {
                          value = checked;
                          Q_EMIT widget->value_changed();
                        });
    }
    else
    {
      layout->addWidget(
          make_error_widget(&attr, "unsupported widget type", widget),
          0,
          0);
    }

    return widget;
  }
};

} // namespace meta::qt
