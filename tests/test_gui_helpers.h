#pragma once

#include "calculator.h"

#include <QAbstractButton>
#include <QCoreApplication>
#include <QLineEdit>
#include <QString>
#include <QTest>

#include <gtest/gtest.h>

inline void flushUi()
{
    QCoreApplication::processEvents();
}

inline QString readDisplay(Calculator &c)
{
    QLineEdit *display = c.findChild<QLineEdit *>();
    EXPECT_NE(display, nullptr);
    return display ? display->text() : QString();
}

inline QAbstractButton *findButton(const Calculator &c, const QString &text)
{
    const QList<QAbstractButton *> buttons = c.findChildren<QAbstractButton *>();
    for (QAbstractButton *b : buttons) {
        if (b->text() == text)
            return b;
    }
    return nullptr;
}

inline void clickButton(Calculator &c, const QString &text)
{
    QAbstractButton *b = findButton(c, text);
    ASSERT_NE(b, nullptr) << "Button not found: " << text.toStdString();
    QTest::mouseClick(b, Qt::LeftButton);
}

/** UTF-8 operators as shown on buttons (see calculator.cpp). */
inline QString opTimes()
{
    return QString::fromUtf8("\xC3\x97");
}

inline QString opDivide()
{
    return QString::fromUtf8("\xC3\xB7");
}

/** "x²" as on the power button. */
inline QString opXSquared()
{
    return QString::fromUtf8("x\xC2\xB2");
}

/** ± sign button. */
inline QString opPlusMinus()
{
    return QString::fromUtf8("\xC2\xB1");
}

/** Clicks a sequence of digit characters ('0'–'9' only); ignores others. */
inline void clickDigitChars(Calculator &c, const QString &digits)
{
    for (const QChar &ch : digits) {
        if (ch.isDigit())
            clickButton(c, QString(ch));
    }
}
