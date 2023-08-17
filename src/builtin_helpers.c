#include "builtin_helpers.h"

#include <stdbool.h>
#include <string.h>

//==== HELPER METHODS ==========================================================
//---- Arithmetic methods ------------------------------------------------------
const larithmop_map_t larithmop_map[] = {
    {"+", LOP_ADD},
    {"-", LOP_SUB},
    {"*", LOP_MUL},
    {"/", LOP_DIV},
    {"%", LOP_MOD},
    {"^", LOP_POW},
    {"max", LOP_MAX},
    {"min", LOP_MIN},
};

enum LARITHMOP op_from_string(char* key) {
    for (int i = 0; i < NUMBER_OF_LARITHMOPS; i++) {
        larithmop_map_t larithmop = larithmop_map[i];
        if (strcmp(larithmop.key, key) == 0) {
            return larithmop.value;
        }
    }
    return LOP_UNKNOWN;
}

long power_long(long base, long exponent) {
    if (exponent == 0) {
        return 1;
    }
    if (exponent == 1) {
        return base;
    }
    long result = 1;
    bool neg = false;
    if (exponent < 0) {
        neg = true;
    }
    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }
    return neg ? 1 / result : result;
}

//---- Logical methods ---------------------------------------------------------
const llogic_map_t llogic_map[] = {
    {"and", LLOGIC_AND},
    {"or", LLOGIC_OR},
    {"not", LLOGIC_NOT}};

enum LLOGIC llogic_from_string(char* key) {
    for (int i = 0; i < NUMBER_OF_LLOGICS; i++) {
        llogic_map_t llogic = llogic_map[i];
        if (strcmp(llogic.key, key) == 0) {
            return llogic.value;
        }
    }
    return LLOGIC_UNKNOWN;
}
