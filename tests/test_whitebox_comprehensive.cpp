/**
 * Caja blanca: incluye todo lo observable en caja negra (misma GUI) más
 * aserciones sobre ramas, estado interno y defectos conocidos en el código.
 */
#include "calculator_test_access.h"
#include "test_gui_helpers.h"

#include <QCoreApplication>

#include <gtest/gtest.h>

using buggy_calc_test::CalculatorTestAccess;

class WhiteBoxUi : public ::testing::Test {
protected:
    void SetUp() override
    {
        calc = new Calculator;
        calc->show();
        flushUi();
    }

    void TearDown() override
    {
        delete calc;
        calc = nullptr;
    }

    Calculator *calc = nullptr;
};

class WhiteBoxCalculateOnly : public ::testing::Test {
protected:
    Calculator c;
};

// --- Ramas de Calculator::calculate (sin GUI) ---

TEST_F(WhiteBoxCalculateOnly, Calculate_plus)
{
    CalculatorTestAccess::setSumSoFar(c, 2.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 3.0, QStringLiteral("+")));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(c), 5.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_minus_bySpec_shouldSubtract)
{
    CalculatorTestAccess::setSumSoFar(c, 10.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 3.0, QStringLiteral("-")));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(c), 7.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_minus_currentDefect_adds)
{
    CalculatorTestAccess::setSumSoFar(c, 10.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 3.0, QStringLiteral("-")));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(c), 13.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_times)
{
    CalculatorTestAccess::setFactorSoFar(c, 4.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 5.0, opTimes()));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::factorSoFar(c), 20.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_divide_normal)
{
    CalculatorTestAccess::setFactorSoFar(c, 10.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 2.0, opDivide()));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::factorSoFar(c), 5.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_unknownOperator_noChange)
{
    CalculatorTestAccess::setSumSoFar(c, 9.0);
    CalculatorTestAccess::setFactorSoFar(c, 9.0);
    CalculatorTestAccess::clearPending(c);
    ASSERT_TRUE(CalculatorTestAccess::invokeCalculate(c, 1.0, QStringLiteral("%")));
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(c), 9.0);
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::factorSoFar(c), 9.0);
}

TEST_F(WhiteBoxCalculateOnly, Calculate_divideByZero_crash_notAutomated)
{
    GTEST_SKIP() << "calculate() llama crashApp(); usar prueba manual o proceso hijo.";
}

// --- Estado interno tras secuencias GUI (conocimiento del código) ---

TEST_F(WhiteBoxUi, State_after_5_plus_pendingAndWaiting)
{
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("+"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("5"));
    EXPECT_EQ(CalculatorTestAccess::pendingAdditive(*calc), QStringLiteral("+"));
    EXPECT_TRUE(CalculatorTestAccess::pendingMultiplicative(*calc).isEmpty());
    EXPECT_TRUE(CalculatorTestAccess::waitingForOperand(*calc));
    EXPECT_EQ(CalculatorTestAccess::calculateCounter(*calc), 1);
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(*calc), 5.0);
}

TEST_F(WhiteBoxUi, State_after_3_times_pendingMultiplicative)
{
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, opTimes());
    flushUi();
    EXPECT_EQ(CalculatorTestAccess::pendingMultiplicative(*calc), opTimes());
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::factorSoFar(*calc), 3.0);
    EXPECT_TRUE(CalculatorTestAccess::waitingForOperand(*calc));
}

TEST_F(WhiteBoxUi, State_calculateCounter_increments_per_operator_click)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("+"));
    flushUi();
    EXPECT_EQ(CalculatorTestAccess::calculateCounter(*calc), 2);
}

TEST_F(WhiteBoxUi, State_calculateCounter_resets_after_equals)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(CalculatorTestAccess::calculateCounter(*calc), 0);
}

TEST_F(WhiteBoxUi, State_memory_MS_stores_sumInMemory)
{
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, QStringLiteral("MS"));
    flushUi();
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumInMemory(*calc), 4.0);
}

TEST_F(WhiteBoxUi, State_memory_MC_zeroes_sumInMemory)
{
    clickButton(*calc, QStringLiteral("8"));
    clickButton(*calc, QStringLiteral("MS"));
    clickButton(*calc, QStringLiteral("MC"));
    flushUi();
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumInMemory(*calc), 0.0);
}

TEST_F(WhiteBoxUi, State_after_clearAll_pendingCleared)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("Clear All"));
    flushUi();
    EXPECT_TRUE(CalculatorTestAccess::pendingAdditive(*calc).isEmpty());
    EXPECT_TRUE(CalculatorTestAccess::pendingMultiplicative(*calc).isEmpty());
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::sumSoFar(*calc), 0.0);
    EXPECT_DOUBLE_EQ(CalculatorTestAccess::factorSoFar(*calc), 0.0);
}

TEST_F(WhiteBoxUi, Structural_equalClicked_guard_counter_gt2_not_run_here)
{
    GTEST_SKIP() << "equalClicked() con calculateCounter>2 llama crashApp(); ver prueba manual.";
}

// --- Cobertura explícita de ramas difíciles vía GUI + estado ---

TEST_F(WhiteBoxUi, Branch_additive_flushes_pending_multiplicative_first)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, QStringLiteral("+"));
    flushUi();
    EXPECT_TRUE(CalculatorTestAccess::pendingMultiplicative(*calc).isEmpty());
    EXPECT_EQ(CalculatorTestAccess::pendingAdditive(*calc), QStringLiteral("+"));
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("6"));
}
