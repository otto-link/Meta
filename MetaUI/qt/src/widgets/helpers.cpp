/* Copyright (c) 2026 Otto Link. Distributed under the terms of the GNU General
   Public License. The full license is in the file LICENSE, distributed with
   this software. */
#include <string>

#include <QFontMetrics>
#include <QWidget>
#include <QPlainTextEdit>

namespace meta::qt::helpers
{

// compute a fixed pixel height that fits [n_lines] of text including
// the frame margin of a QPlainTextEdit.
int plain_text_height(const QPlainTextEdit *te, int n_lines)
{
  const int lh = QFontMetrics(te->font()).lineSpacing();
  // QPlainTextEdit adds ~4 px top + bottom document margin by default.
  return lh * n_lines + 8;
}

int text_width(const QWidget *w, const std::string &s)
{
  return QFontMetrics(w->font()).horizontalAdvance(QString::fromStdString(s));
}

std::string truncate_string(const std::string &s, size_t max_len)
{
  return s.size() <= max_len ? s : s.substr(0, max_len - 1) + "…";
}

} // namespace meta::qt::helpers
