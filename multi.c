/*
 * ============================================================
 * Project  : 4-bit Sequential Shift-and-Add Multiplier
 * File     : multi.c
 * Language : C
 * Description:
 *   Software simulation of the hardware sequential multiplier.
 *   Replicates the clock-by-clock behavior of the VHDL design
 *   using the shift-and-add algorithm on 4-bit inputs.
 * ============================================================
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* ---- State struct (mirrors VHDL signals) ---- */
typedef struct {
    uint8_t A;      /* 4-bit accumulator        */
    uint8_t E;      /* carry/extension bit       */
    uint8_t Q_reg;  /* 4-bit shifted multiplier  */
    uint8_t flag;   /* alternating clock phase   */
    int     count;  /* remaining iterations      */
} MultiplierState;

/* ---- Reset (active-low in VHDL) ---- */
void reset(MultiplierState *s, uint8_t Q) {
    s->A     = 0x0;
    s->E     = 0;
    s->flag  = 1;
    s->count = 4;
    s->Q_reg = Q & 0xF;
}

/* ---- Single rising-edge clock step ---- */
void clock_tick(MultiplierState *s, uint8_t M) {
    if (s->count <= 0) return;

    uint8_t sum5 = 0;

    /* Compute combinational sum outside process */
    sum5 = (s->A + (M & 0xF)) & 0x1F; /* 5-bit result */

    if (s->Q_reg & 0x1) {               /* Q_reg(0) = '1' */
        if (s->flag) {                  /* flag = '1' => add phase */
            s->A = sum5 & 0xF;
            s->E = (sum5 >> 4) & 0x1;
        }
    }

    s->flag = !s->flag;

    if (!s->flag) {                     /* flag = '0' => shift phase */
        uint8_t new_A3   = s->E;
        uint8_t new_A    = ((new_A3 << 3) | (s->A >> 1)) & 0xF;
        uint8_t lsb_A    = s->A & 0x1;
        s->Q_reg = ((lsb_A << 3) | (s->Q_reg >> 1)) & 0xF;
        s->A     = new_A;
        s->count--;
    }
}

/* ---- Read product ---- */
uint8_t get_product(const MultiplierState *s) {
    return ((s->A & 0xF) << 4) | (s->Q_reg & 0xF);
}

/* ---- Run full multiplication (8 ticks = 4 add/shift pairs) ---- */
uint8_t multiply(uint8_t M, uint8_t Q) {
    MultiplierState s;
    reset(&s, Q);

    /* 8 clock cycles: each iteration = 1 add tick + 1 shift tick */
    for (int i = 0; i < 8; i++) {
        clock_tick(&s, M);
    }
    return get_product(&s);
}

/* ---- Demo ---- */
int main(void) {
    printf("=== 4-bit Sequential Shift-and-Add Multiplier (C) ===\n\n");
    printf("%-6s %-6s %-12s %-12s %s\n",
           "M", "Q", "Expected", "Got", "Status");
    printf("----------------------------------------------------\n");

    uint8_t test_M[] = {3, 5, 7, 15, 0,  1,  6,  9};
    uint8_t test_Q[] = {4, 6, 3, 15, 9, 15, 11, 14};
    int     pass = 0, fail = 0;

    for (int i = 0; i < 8; i++) {
        uint8_t expected = (test_M[i] * test_Q[i]) & 0xFF;
        uint8_t result   = multiply(test_M[i], test_Q[i]);
        int ok = (result == expected);
        printf("%-6u %-6u %-12u %-12u %s\n",
               test_M[i], test_Q[i], expected, result, ok ? "PASS" : "FAIL");
        ok ? pass++ : fail++;
    }

    printf("----------------------------------------------------\n");
    printf("Results: %d PASS / %d FAIL\n", pass, fail);
    return (fail == 0) ? 0 : 1;
}
