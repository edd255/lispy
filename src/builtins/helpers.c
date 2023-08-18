#include "helpers.h"

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
        if (0 == strcmp(larithmop.key, key)) {
            return larithmop.value;
        }
    }
    return LOP_UNKNOWN;
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
const llogic_map_t llogic_map[] = {
    {"and", LLOGIC_AND},
    {"or", LLOGIC_OR},
    {"not", LLOGIC_NOT},
};

enum LLOGIC llogic_from_string(char* key) {
    for (int i = 0; i < NUMBER_OF_LLOGICS; i++) {
        llogic_map_t llogic = llogic_map[i];
        if (0 == strcmp(llogic.key, key)) {
            return llogic.value;
        }
    }
    return LLOGIC_UNKNOWN;
}
