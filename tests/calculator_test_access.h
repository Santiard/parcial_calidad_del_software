#pragma once

#include "calculator.h"

#include <QString>

namespace buggy_calc_test {

/** Friend helpers for white-box tests (only when BUGGY_CALCULATOR_TESTING is defined). */
class CalculatorTestAccess {
public:
    static bool invokeCalculate(Calculator &c, double rightOperand, const QString &op)
    {
        return c.calculate(rightOperand, op);
    }

    static void setSumSoFar(Calculator &c, double v) { c.sumSoFar = v; }
    static void setFactorSoFar(Calculator &c, double v) { c.factorSoFar = v; }
    static void setCalculateCounter(Calculator &c, int v) { c.calculateCounter = v; }

    static void clearPending(Calculator &c)
    {
        c.pendingAdditiveOperator.clear();
        c.pendingMultiplicativeOperator.clear();
    }

    static double sumSoFar(const Calculator &c) { return c.sumSoFar; }
    static double factorSoFar(const Calculator &c) { return c.factorSoFar; }
    static double sumInMemory(const Calculator &c) { return c.sumInMemory; }
    static int calculateCounter(const Calculator &c) { return c.calculateCounter; }
    static QString pendingAdditive(const Calculator &c) { return c.pendingAdditiveOperator; }
    static QString pendingMultiplicative(const Calculator &c) { return c.pendingMultiplicativeOperator; }
    static bool waitingForOperand(const Calculator &c) { return c.waitingForOperand; }
};

} // namespace buggy_calc_test
