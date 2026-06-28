/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#pragma once
#include <string>

#include <QWidget>
#include <QPlainTextEdit>

namespace meta::qt::helpers
{

int plain_text_height(const QPlainTextEdit *te, int n_lines);

int text_width(const QWidget *w, const std::string &s);

std::string truncate_string(const std::string &s, size_t max_len);

} // namespace meta::qt::helpers