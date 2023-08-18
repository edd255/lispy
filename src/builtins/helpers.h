#ifndef HOME_EDD_CODE_LISPY_SRC_BUILTINS_HELPERS_H
#define HOME_EDD_CODE_LISPY_SRC_BUILTINS_HELPERS_H

#include <stdbool.h>

//==== HELPER METHODS ==========================================================
//---- Arithmetic methods ------------------------------------------------------
enum LARITHMOP {
    LOP_ADD,
    LOP_SUB,
    LOP_MUL,
    LOP_DIV,
    LOP_MOD,
    LOP_POW,
    LOP_MAX,
    LOP_MIN,
    LOP_UNKNOWN
};

typedef struct larithmop_map_t larithmop_map_t;
struct larithmop_map_t {
    char* key;
    enum LARITHMOP value;
} __attribute__((aligned(16)));

#define NUMBER_OF_LARITHMOPS \
    (int)(sizeof(larithmop_map) / sizeof(larithmop_map_t))

enum LARITHMOP op_from_string(char* key);

long power_long(long base, long exponent);

//---- Logical methods ---------------------------------------------------------
enum LLOGIC { LLOGIC_AND, LLOGIC_OR, LLOGIC_NOT, LLOGIC_UNKNOWN };

typedef struct llogic_map_t llogic_map_t;
struct llogic_map_t {
    char* key;
    enum LLOGIC value;
} __attribute__((aligned(16)));

#define NUMBER_OF_LLOGICS (int)(sizeof(llogic_map) / sizeof(llogic_map_t))

enum LLOGIC llogic_from_string(char* key);

#endif
