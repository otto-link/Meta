/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <QCheckBox>
#include <QLabel>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

namespace meta::qt
{

template <typename T> struct WidgetRenderer
{
  static QWidget *render(Attribute<T> &attr, QWidget *parent)
  {
    const std::string label = "Unsupported type: " + attr.name();
    return new QLabel(label.c_str(), parent);
  }
};

// helper
template <typename T>
QWidget *render(Attribute<T> &attr, QWidget *parent = nullptr)
{
  return WidgetRenderer<T>::render(attr, parent);
}

// --- Specialization

template <> struct WidgetRenderer<bool>
{
  static QWidget *render(Attribute<bool> &attr, QWidget *parent)
  {
    bool             &value = attr.value();
    const std::string widget_type = meta::common::widget_type(attr);

    if (false) // widget_type == "Toggle")
    {
      //   auto entries = std::make_shared<std::vector<std::string>>(
      //       std::vector<std::string>{"True", "False"});
      //   auto selected = std::make_shared<int>(value ? 0 : 1);

      //   Component toggle = ftxui::Toggle(entries.get(), selected.get());

      //   // wrap in a renderer that captures shared_ptrs and syncs back to
      //   value return ftxui::Renderer(toggle,
      //                          [entries,
      //                           selected,
      //                           &value,
      //                           toggle,
      //                           label = meta::common::label(attr)]()
      //                          {
      //                            value = (*selected == 0);
      //                            return ftxui::hbox({
      //                                ftxui::text(label + " "),
      //                                toggle->Render(),
      //                            });
      //                          });
    }
    else
    {
      QCheckBox *check_box = new QCheckBox(meta::common::label(attr).c_str(),
                                           parent);
      check_box->setChecked(value);
      check_box->connect(check_box,
                         &QCheckBox::toggled,
                         check_box,
                         [check_box, &value](bool checked)
                         { value = checked; });

      return check_box;
    }
  }
};

} // namespace meta::qt