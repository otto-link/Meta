/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <format>

#include <QComboBox>
#include <QLabel>
#include <QWidget>

#include "meta/type/type_name.hpp"
#include "meta_common.hpp"

#include "meta_qt/meta_widget.hpp"

namespace meta::qt
{

template <> struct WidgetRenderer<int>
{
  static MetaWidget *render(Attribute<int> &attr, QWidget *parent)
  {
    const std::string widget_type = meta::common::widget_type(attr);
    const std::string label_txt = meta::common::label(attr);
    const std::string format = meta::common::format(attr);
    // const int         min = meta::common::min(attr);
    // const int         max = meta::common::max(attr);
    // const int         step = meta::common::step(attr);
    const auto items = meta::common::enum_items<int>(attr);

    int &value = attr.value();

    MetaWidget *widget = make_meta_widget_vbox(parent);
    auto       *layout = static_cast<QVBoxLayout *>(widget->layout());

    if (!label_txt.empty())
    {
      QLabel *label = new QLabel(label_txt.c_str(), widget);
      layout->addWidget(label);
    }

    if (widget_type == "EnumComboBox")
    {
      // --- ENUM COMBO BOX

      auto *combo = new QComboBox(widget);
      layout->addWidget(combo);

      const auto *m = attr.metadata().find(meta::keys::constraints::enum_items);

      std::vector<std::pair<int, std::string>> items;

      if (m) items = std::any_cast<decltype(items)>(m->to_any());

      int current_index = 0;
      int index = 0;

      for (const auto &[val, name] : items)
      {
        combo->addItem(QString::fromStdString(name), QVariant::fromValue(val));

        if (val == value) current_index = index;

        ++index;
      }

      combo->setCurrentIndex(current_index);

      widget->connect(combo,
                      QOverload<int>::of(&QComboBox::currentIndexChanged),
                      widget,
                      [&value, widget, combo](int)
                      {
                        value = combo->currentData().toInt();
                        Q_EMIT widget->value_changed();
                      });
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
