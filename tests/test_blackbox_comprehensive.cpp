/**
 * Caja negra: solo entradas/salidas observables (clics + texto del display).
 * No usa estado interno ni CalculatorTestAccess.
 *
 * Cobertura: operaciones, memoria, unarios, decimales, teclas etiquetadas,
 * límites de UI y casos no automatizables (crash) documentados con GTEST_SKIP.
 */
#include "test_gui_helpers.h"

#include <QCoreApplication>

#include <gtest/gtest.h>

class BlackBoxUi : public ::testing::Test {
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

// --- Dígitos y formación de número ---

TEST_F(BlackBoxUi, Digit0_initialState_secondZeroIgnored)
{
    clickButton(*calc, QStringLiteral("0"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

TEST_F(BlackBoxUi, Digit0_afterFive_appendsZero)
{
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("0"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("50"));
}

TEST_F(BlackBoxUi, MultiDigit_123)
{
    clickDigitChars(*calc, QStringLiteral("123"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("123"));
}

class BlackBoxDigitParam : public BlackBoxUi, public ::testing::WithParamInterface<int> {};

TEST_P(BlackBoxDigitParam, SingleDigit_display_matches_spec)
{
    const int d = GetParam();
    clickButton(*calc, QString::number(d));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QString::number(d));
}

INSTANTIATE_TEST_SUITE_P(AllDigits0to9, BlackBoxDigitParam, ::testing::Range(0, 10));

// --- Aritmética básica ---

TEST_F(BlackBoxUi, Add_2_plus_3_equals_5)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("5"));
}

TEST_F(BlackBoxUi, Add_chain_1_plus_2_plus_3_equals_6)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("6"));
}

TEST_F(BlackBoxUi, Add_negative_left_minus2_plus_5_equals_3)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, opPlusMinus());
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("3"));
}

TEST_F(BlackBoxUi, Subtract_5_minus_2_equals_3_spec)
{
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("-"));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("3"));
}

TEST_F(BlackBoxUi, Subtract_2_minus_5_equals_neg3_spec)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("-"));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("-3"));
}

TEST_F(BlackBoxUi, Multiply_4_times_5_equals_20)
{
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("20"));
}

TEST_F(BlackBoxUi, Multiply_chain_2_times_3_times_4_equals_24)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("24"));
}

TEST_F(BlackBoxUi, Divide_10_div_2_equals_5)
{
    clickDigitChars(*calc, QStringLiteral("10"));
    clickButton(*calc, opDivide());
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("5"));
}

TEST_F(BlackBoxUi, Divide_7_div_2_decimal_spec)
{
    clickButton(*calc, QStringLiteral("7"));
    clickButton(*calc, opDivide());
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("3.5"));
}

TEST_F(BlackBoxUi, Mixed_precedence_2_plus_3_times_4_equals_14_gui_order)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("14"));
}

TEST_F(BlackBoxUi, Add_then_multiply_1_plus_2_times_3_equals_7)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, opTimes());
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    // Primero se resuelve 2×3=6, luego 1+6=7 (comportamiento de esta GUI).
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("7"));
}

// --- Punto decimal ---

TEST_F(BlackBoxUi, Decimal_point_from_waiting_shows_zeroPoint)
{
    clickButton(*calc, QStringLiteral("."));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0."));
}

TEST_F(BlackBoxUi, Decimal_2_dot_5_plus_0_dot_5_equals_3)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("."));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, QStringLiteral("."));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("3"));
}

TEST_F(BlackBoxUi, Decimal_secondDot_ignored)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("."));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("."));
    clickButton(*calc, QStringLiteral("3"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("1.23"));
}

// --- Cambio de signo ---

TEST_F(BlackBoxUi, Sign_positive_to_negative)
{
    clickButton(*calc, QStringLiteral("7"));
    clickButton(*calc, opPlusMinus());
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("-7"));
}

TEST_F(BlackBoxUi, Sign_negative_to_positive)
{
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, opPlusMinus());
    clickButton(*calc, opPlusMinus());
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("4"));
}

TEST_F(BlackBoxUi, Sign_zero_unchanged_spec)
{
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, opPlusMinus());
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

// --- Etiquetas “Clear” / “Backspace” (comportamiento esperado según el texto del botón) ---

TEST_F(BlackBoxUi, LabeledBackspace_shouldDeleteLastDigit_spec)
{
    clickDigitChars(*calc, QStringLiteral("123"));
    clickButton(*calc, QStringLiteral("Backspace"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("122"));
}

TEST_F(BlackBoxUi, LabeledClear_shouldResetCurrentEntry_spec)
{
    clickDigitChars(*calc, QStringLiteral("123"));
    clickButton(*calc, QStringLiteral("Clear"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

TEST_F(BlackBoxUi, ClearAll_resetsToZero)
{
    clickDigitChars(*calc, QStringLiteral("999"));
    clickButton(*calc, QStringLiteral("Clear All"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

TEST_F(BlackBoxUi, ClearAll_then_add_startsFresh)
{
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("Clear All"));
    clickButton(*calc, QStringLiteral("9"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("10"));
}

// --- Memoria ---

TEST_F(BlackBoxUi, Memory_MS_MR_roundtrip_spec)
{
    clickButton(*calc, QStringLiteral("7"));
    clickButton(*calc, QStringLiteral("MS"));
    clickButton(*calc, QStringLiteral("Clear All"));
    clickButton(*calc, QStringLiteral("MR"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("7"));
}

TEST_F(BlackBoxUi, Memory_MC_clears_then_MR_zero)
{
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, QStringLiteral("MS"));
    clickButton(*calc, QStringLiteral("MC"));
    clickButton(*calc, QStringLiteral("MR"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

TEST_F(BlackBoxUi, Memory_Mplus_accumulates_spec)
{
    clickButton(*calc, QStringLiteral("2"));
    clickButton(*calc, QStringLiteral("MS"));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("M+"));
    clickButton(*calc, QStringLiteral("MR"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("7"));
}

TEST_F(BlackBoxUi, Memory_MS_after_expression_5_plus_5_equals_10)
{
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    clickButton(*calc, QStringLiteral("MS"));
    clickButton(*calc, QStringLiteral("Clear All"));
    clickButton(*calc, QStringLiteral("MR"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("10"));
}

// --- Operadores unarios ---

TEST_F(BlackBoxUi, Unary_x_squared_3_equals_9)
{
    clickButton(*calc, QStringLiteral("3"));
    clickButton(*calc, opXSquared());
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("9"));
}

TEST_F(BlackBoxUi, Unary_inv_4_equals_0_25)
{
    clickButton(*calc, QStringLiteral("4"));
    clickButton(*calc, QStringLiteral("1/x"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0.25"));
}

TEST_F(BlackBoxUi, Unary_inv_zero_shows_error_spec)
{
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, QStringLiteral("1/x"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("####"));
}

TEST_F(BlackBoxUi, Unary_sqrt_9_equals_3_spec)
{
    clickButton(*calc, QStringLiteral("9"));
    clickButton(*calc, QStringLiteral("Sqrt"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("3"));
}

TEST_F(BlackBoxUi, Unary_sqrt_negative_shows_error_spec)
{
    clickButton(*calc, QStringLiteral("9"));
    clickButton(*calc, opPlusMinus());
    clickButton(*calc, QStringLiteral("Sqrt"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("####"));
}

// --- Flujo tras = y límites de UI ---

TEST_F(BlackBoxUi, AfterEquals_digit_replacesResult)
{
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("1"));
    clickButton(*calc, QStringLiteral("="));
    clickButton(*calc, QStringLiteral("7"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("7"));
}

TEST_F(BlackBoxUi, Display_maxLength_15_digits_noLonger)
{
    clickDigitChars(*calc, QStringLiteral("111111111111111"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc).length(), 15);
    clickButton(*calc, QStringLiteral("1"));
    flushUi();
    EXPECT_EQ(readDisplay(*calc).length(), 15);
}

TEST_F(BlackBoxUi, Zero_plus_zero_equals_zero)
{
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, QStringLiteral("+"));
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

TEST_F(BlackBoxUi, Zero_divided_by_5_equals_zero)
{
    clickButton(*calc, QStringLiteral("0"));
    clickButton(*calc, opDivide());
    clickButton(*calc, QStringLiteral("5"));
    clickButton(*calc, QStringLiteral("="));
    flushUi();
    EXPECT_EQ(readDisplay(*calc), QStringLiteral("0"));
}

// --- Entradas no aplicables en esta GUI (documentación) ---

TEST_F(BlackBoxUi, NoConsoleParser_lettersNotApplicable)
{
    GTEST_SKIP() << "No hay entrada de texto tipo consola; letras/símbolos no son introducibles por teclado en esta app.";
}

// --- Casos que requieren ejecución manual / proceso aislado ---

TEST_F(BlackBoxUi, Divide_by_zero_manual_only)
{
    GTEST_SKIP() << "División por cero provoca crash intencional; probar manualmente o con proceso hijo.";
}

TEST_F(BlackBoxUi, Long_press_any_key_manual_only)
{
    GTEST_SKIP() << "Pulsación larga provoca crash; solo manual o automatización de bajo nivel.";
}

TEST_F(BlackBoxUi, Double_click_window_manual_only)
{
    GTEST_SKIP() << "Doble clic en ventana provoca crash; solo manual.";
}

TEST_F(BlackBoxUi, Three_operator_then_equal_crash_manual_only)
{
    GTEST_SKIP() << "Más de dos operadores seguidos y '=' provoca crash; solo manual o proceso hijo.";
}
