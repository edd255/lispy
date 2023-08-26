#include "builtins/helpers.h"

#include <stdbool.h>
#include <string.h>

//==== HELPER METHODS ==========================================================
//---- Arithmetic methods ------------------------------------------------------
const lispy_arithm_op_map_t lispy_arithm_ops_map[] = {
    {"+", LISPY_ARITHM_ADD},
    {"-", LISPY_ARITHM_SUB},
    {"*", LISPY_ARITHM_MUL},
    {"/", LISPY_ARITHM_DIV},
    {"%", LISPY_ARITHM_MOD},
    {"^", LISPY_ARITHM_POW},
    {"max", LISPY_ARITHM_MAX},
    {"min", LISPY_ARITHM_MIN},
};

enum LISPY_ARITHM_OP arithm_op_from_str(const char* key) {
#pragma unroll
    for (int i = 0; i < NUMBER_OF_ARITHM_OPS; i++) {
        lispy_arithm_op_map_t larithmop = lispy_arithm_ops_map[i];
        if (0 == strcmp(larithmop.key, key)) {
            return larithmop.value;
        }
    }
    return LISPY_ARITHM_UNKNOWN;
}

long power_long(long base, long exponent) {
    if (0 == exponent) {
        return 1;
    }
    if (1 == exponent) {
        return base;
    }
    long result = 1;
    bool neg = false;
    if (0 > exponent) {
        neg = true;
    }
    while (0 < exponent) {
        if (1 == exponent % 2) {
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }
    return neg ? 1 / result : result;
}

//---- Logical methods ---------------------------------------------------------
const lispy_logic_op_map_t lispy_logic_ops_map[] = {
    {"and", LISPY_LOGIC_AND},
    {"or", LISPY_LOGIC_OR},
    {"not", LISPY_LOGIC_NOT},
};

enum LISPY_LOGIC_OP logic_op_from_str(const char* key) {
#pragma unroll
    for (int i = 0; i < NUMBER_OF_LOGIC_OPS; i++) {
        lispy_logic_op_map_t llogic = lispy_logic_ops_map[i];
        if (0 == strcmp(llogic.key, key)) {
            return llogic.value;
        }
    }
    return LISPY_LOGIC_UNKNOWN;
}
