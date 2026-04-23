# Entrega: caja negra y caja blanca (dos ejecutables)

## Ejecutables

| Ejecutable | Contenido |
|------------|-----------|
| `buggy_calc_blackbox_tests` | Solo `test_blackbox_comprehensive.cpp`. **No** se define `BUGGY_CALCULATOR_TESTING`: no hay `friend` ni lectura de privados. |
| `buggy_calc_whitebox_tests` | `test_blackbox_comprehensive.cpp` **más** `test_whitebox_comprehensive.cpp`. Con `BUGGY_CALCULATOR_TESTING`: acceso vía `CalculatorTestAccess` y aserciones de ramas/estado. |

La caja blanca **reutiliza toda** la batería de caja negra (mismas entradas/salidas) y añade pruebas estructurales y de estado interno.

## Compilar y ejecutar

```bash
cd tests
cmake -S . -B build
cmake --build build
cd build
./buggy_calc_blackbox_tests
./buggy_calc_whitebox_tests
ctest --output-on-failure
```

## Guardar resultado en archivo (Windows)

Desde la carpeta `tests`:

```powershell
.\run_tests_save_log.ps1
```

Genera **`tests/test_results.txt`** con la salida completa de caja negra y caja blanca (UTF-8 sin BOM).

- Con informe XML de Google Test además del `.txt`:

  ```powershell
  .\run_tests_save_log.ps1 -Xml
  ```

  Los XML quedan en `tests/build/gtest_blackbox.xml` y `tests/build/gtest_whitebox.xml`.

- Si Qt/MinGW no están en las rutas por defecto del script, indica `-QtBin` y `-MingwBin`.

En Linux sin pantalla, `ctest` usa `QT_QPA_PLATFORM=offscreen`.

## Caja negra: alcance

Se cubre, por **observación de la GUI**, entre otras:

- Dígitos 0–9 (parametrizado; el **8** falla frente a especificación).
- Formación de número, cero inicial, longitud máxima del display (15).
- `+`, `-`, `×`, `÷`, cadenas y precedencia **tal como la implementa esta app** (ej. `2+3×4`).
- Punto decimal, segundo punto ignorado.
- `±`, memoria `MC`/`MR`/`MS`/`M+`, `Clear All`.
- Etiquetas **“Clear”** y **“Backspace”** contrastadas con el comportamiento esperado por el usuario.
- Unarios: `x²`, `1/x` (incl. `####`), `Sqrt` (especificación vs defecto).
- Flujo tras `=`.
- Entradas **no aplicables** (consola/letras): `GTEST_SKIP` con explicación.
- Crashes intencionales: `GTEST_SKIP` (división por cero, pulsación larga, doble clic, `calculateCounter > 2` antes de `=`).

## Caja blanca: alcance adicional

- Ramas de `Calculator::calculate` (incl. operador desconocido sin efecto).
- Contraste **especificación** vs **defecto actual** en la resta (una prueba espera 7, otra documenta 13).
- Estado tras clics: `pendingAdditiveOperator`, `pendingMultiplicativeOperator`, `waitingForOperand`, `calculateCounter`, `sumSoFar`, `factorSoFar`, `sumInMemory`.
- Rama `additiveOperatorClicked` que **liquida** antes un multiplicativo pendiente (`2×3+`).

## Complejidad ciclomática (recordatorio)

Para `Calculator::calculate`, **V(G) ≈ 6** (varias ramas `if/else if` + divisor cero).

## Cobertura

Con GCC/Clang:

```bash
cmake -S . -B build -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Ejecutar el binario deseado y generar informe con `lcov`/`genhtml` (ver documentación previa del curso).

## Resultados esperados en un proyecto *buggy*

Varias pruebas de **especificación** fallarán (resta, dígito 8, `Sqrt`, intercambio de etiquetas, etc.). Los `GTEST_SKIP` no cuentan como fallo. Las pruebas que documentan el **defecto actual** pueden pasar.
