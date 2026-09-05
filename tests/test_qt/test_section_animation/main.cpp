#include <QApplication>
#include <QCoreApplication>
#include <QLabel>
#include <QScrollArea>
#include <QVariantAnimation>
#include <QVBoxLayout>

#include <array>
#include <iostream>

#include "meta_qt/designs/industrial/section.hpp"

using meta::qt::industrial::Section;

namespace
{
void flush()
{
  for (int i = 0; i < 8; ++i)
  {
    QCoreApplication::sendPostedEvents();
    QCoreApplication::processEvents();
  }
}

// Observe actual paint events as well as settled geometry: a transient bad
// layout must not be hidden by draining several rounds of LayoutRequest first.
class PanelCheck : public QObject
{
public:
  std::array<Section *, 4> sections{};
  int failures = 0;
  int paints = 0;
  bool watching = false;

  void check()
  {
    for (int i = 0; i < 4; ++i)
    {
      auto *s = sections[i];
      if (s->height() != s->sizeHint().height())
      {
        if (failures < 5)
          std::cerr << "section=" << i << " actual=" << s->height()
                    << " hint=" << s->sizeHint().height() << '\n';
        fail("section height differs from its requested reveal");
      }
      const auto *layout = s->parentWidget()->layout();
      const int expected_y = i
                                 ? sections[i - 1]->geometry().bottom() + 1
                                       + layout->spacing()
                                 : layout->contentsMargins().top();
      if (s->y() != expected_y) fail("section position or gap changed");
    }
  }

  void fail(const char *message)
  {
    if (failures++ < 5) std::cerr << message << '\n';
  }

  bool eventFilter(QObject *, QEvent *event) override
  {
    if (watching && event->type() == QEvent::Paint)
    {
      ++paints;
      check();
    }
    return false;
  }
};

int exercise(int viewport_height)
{
  meta::qt::Theme theme;
  QScrollArea scroll;
  scroll.setAttribute(Qt::WA_DontShowOnScreen);
  scroll.setWidgetResizable(true);
  scroll.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scroll.resize(520, viewport_height);
  auto *container = new QWidget;
  auto *outer = new QVBoxLayout(container);
  outer->setAlignment(Qt::AlignTop);
  auto *node = new QWidget;
  auto *node_layout = new QVBoxLayout(node);
  node_layout->setAlignment(Qt::AlignTop);
  outer->addWidget(node);
  auto *page = new QWidget;
  auto *layout = new QVBoxLayout(page);
  layout->setAlignment(Qt::AlignTop);
  node_layout->addWidget(page);

  PanelCheck checker;
  const std::array<int, 4> rows{3, 3, 2, 8};
  for (int i = 0; i < 4; ++i)
  {
    auto *s = new Section(QString::number(i), theme);
    checker.sections[i] = s;
    for (int j = 0; j < rows[i]; ++j)
    {
      auto *row = new QLabel(QString("Parameter %1").arg(j));
      row->setFixedHeight(36);
      s->content_layout->addWidget(row);
    }
    layout->addWidget(s);
    s->set_expanded(i != 3);
    s->installEventFilter(&checker);
  }
  scroll.setWidget(container);
  scroll.show();
  flush();
  checker.watching = true;

  auto advance = [&](Section *s, int ms)
  {
    s->findChild<QVariantAnimation *>()->setCurrentTime(ms);
    flush();
    checker.check();
  };
  auto *last = checker.sections.back();
  for (bool expanded : {true, false})
  {
    last->set_expanded(expanded);
    for (int ms = 0; ms <= theme.metrics.section_ms; ms += 16)
      advance(last, ms);
    advance(last, theme.metrics.section_ms);
  }

  // Reverse each section while the last card is collapsed. The new transition
  // must start at the displayed height, including repeated same-state calls.
  for (int i = 0; i < 4; ++i)
  {
    auto *s = checker.sections[i];
    const bool initially_open = i != 3;
    s->set_expanded(!initially_open);
    advance(s, 48);
    const int before = s->sizeHint().height();
    s->set_expanded(initially_open);
    if (s->sizeHint().height() != before)
      checker.fail("reversing animation jumps to an endpoint");
    advance(s, 32);
    const int repeated = s->sizeHint().height();
    s->set_expanded(initially_open);
    if (s->sizeHint().height() != repeated)
      checker.fail("same-state request jumps to an endpoint");
    advance(s, theme.metrics.section_ms);
  }

  // Fully open sections must still follow body changes after animation ends.
  auto *extra = new QLabel("Added parameter");
  extra->setFixedHeight(36);
  const int old_height = checker.sections[0]->height();
  checker.sections[0]->content_layout->addWidget(extra);
  flush();
  checker.check();
  if (checker.sections[0]->height() <= old_height)
    checker.fail("expanded section did not follow new content");

  std::cout << "viewport=" << viewport_height << " paints=" << checker.paints
            << " failures=" << checker.failures << '\n';
  checker.watching = false;
  return checker.failures;
}
} // namespace

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  int failures = 0;
  for (int height : {1100, 740, 400}) failures += exercise(height);
  return failures ? 1 : 0;
}

