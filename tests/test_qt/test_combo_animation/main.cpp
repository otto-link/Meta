#include <QApplication>
#include <QImage>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPointer>
#include <QScreen>
#include <QVariantAnimation>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#include <iostream>

#include "meta_qt/designs/industrial/combo.hpp"

using meta::qt::industrial::ComboPopup;

namespace
{
int failures = 0;

void check(bool condition, const char *message)
{
  if (!condition)
  {
    ++failures;
    std::cerr << message << '\n';
  }
}

QImage frame(ComboPopup *popup)
{
  return popup->grab().toImage().convertToFormat(QImage::Format_ARGB32);
}

int opaque_rows(const QImage &image)
{
  int count = 0;
  for (int y = 0; y < image.height(); ++y)
    if (qAlpha(image.pixel(image.width() / 2, y))) ++count;
  return count;
}

void key(ComboPopup *popup, int code)
{
  QKeyEvent event(QEvent::KeyPress, code, Qt::NoModifier);
  QApplication::sendEvent(popup, &event);
}

void mouse(ComboPopup *popup, QEvent::Type type, const QPoint &pos)
{
  QMouseEvent event(type,
                    QPointF(pos),
                    QPointF(popup->mapToGlobal(pos)),
                    Qt::LeftButton,
                    Qt::LeftButton,
                    Qt::NoModifier);
  QApplication::sendEvent(popup, &event);
}

void exercise(bool flipped, int dismissal, bool interrupt_open)
{
  meta::qt::Theme      theme;
  QWidget              owner;
  QPointer<ComboPopup> popup = new ComboPopup(
      theme,
      {"add", "exclusion", "gradients", "maximum", "replace"},
      0,
      &owner);
  popup->setAttribute(Qt::WA_DontShowOnScreen);
  int selections = 0;
  QObject::connect(popup,
                   &ComboPopup::selected,
                   [&selections](int) { ++selections; });

  const QRect screen = QApplication::primaryScreen()->availableGeometry();
  const QRect field(screen.left() + 100,
                    flipped ? screen.bottom() - 30 : screen.top() + 30,
                    240,
                    24);
  popup->popup_for(field);
  auto *animation = popup->findChild<QVariantAnimation *>();
  animation->pause();

  check(popup->windowFlags().testFlag(Qt::FramelessWindowHint),
        "Windows translucent popup must be frameless");
#ifdef Q_OS_WIN
  if (QApplication::platformName() == "windows")
  {
    // QWidget::grab() cannot see the OS shadow. Inspect the native window
    // class as well so a full-size outline cannot escape the pixel checks.
    const auto hwnd = reinterpret_cast<HWND>(popup->winId());
    check((GetClassLongPtr(hwnd, GCL_STYLE) & CS_DROPSHADOW) == 0,
          "native popup shadow must not outline the unrevealed window");
  }
#endif
  check(opaque_rows(frame(popup)) == 0,
        "zero reveal must not flash the fully open card");

  animation->setCurrentTime(theme.metrics.section_ms / 5);
  const QImage opening = frame(popup);
  const int    partial = opaque_rows(opening);
  check(partial > 0 && partial < opening.height(),
        "opening must reveal gradually");
  const int hidden_y = flipped ? 0 : opening.height() - 1;
  check(qAlpha(opening.pixel(opening.width() / 2, hidden_y)) == 0,
        "unrevealed popup surface must be transparent");

  if (!interrupt_open) animation->setCurrentTime(theme.metrics.section_ms);
  const int before_close = opaque_rows(frame(popup));
  switch (dismissal)
  {
  case 0: key(popup, Qt::Key_Escape); break;
  case 1: mouse(popup, QEvent::MouseButtonPress, QPoint(-10, -10)); break;
  case 2: key(popup, Qt::Key_Return); break;
  case 3:
    mouse(popup,
          QEvent::MouseButtonRelease,
          QPoint(popup->width() / 2, flipped ? popup->height() - 16 : 16));
    break;
  }
  check(popup && popup->isVisible(),
        "dismissal must keep popup visible while closing");
  if (!popup || !popup->isVisible()) return;
  check(opaque_rows(frame(popup)) == before_close,
        "closing must start at current reveal without jumping");
  animation->pause();
  animation->setCurrentTime(theme.metrics.section_ms / 3);
  const int during_close = opaque_rows(frame(popup));
  check(during_close > 0 && during_close < before_close,
        "closing must shrink and clear the previously painted surface");
  key(popup, Qt::Key_Return);
  popup->close();
  check(opaque_rows(frame(popup)) == during_close,
        "repeated dismissal must not restart closing");
  check(selections == (dismissal >= 2 ? 1 : 0),
        "selection must be emitted exactly once, and never on dismissal");

  animation->setCurrentTime(theme.metrics.section_ms);
  check(!popup->isVisible(), "popup must hide when closing finishes");
  QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
  check(popup.isNull(), "closed popup must be deleted");
}
} // namespace

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  for (bool flipped : {false, true})
    for (int dismissal = 0; dismissal < 4; ++dismissal)
      for (bool interrupt : {false, true})
        exercise(flipped, dismissal, interrupt);
  std::cout << "16 popup scenarios; failures=" << failures << '\n';
  return failures ? 1 : 0;
}
