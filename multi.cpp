/*
 * ============================================================
 * Project  : 4-bit Sequential Shift-and-Add Multiplier
 * File     : multi.cpp
 * Language : C++17
 * Description:
 *   Object-oriented simulation of the hardware sequential
 *   multiplier. Encapsulates the VHDL entity/architecture
 *   as a C++ class with per-tick simulation support.
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <string>

/* ============================================================
 * Class: SequentialMultiplier
 * Models the VHDL entity "multi" — signals become members,
 * each rising clock edge maps to one call of tick().
 * ============================================================ */
class SequentialMultiplier {
public:
    /* Constructor — active-low reset immediately applied */
    explicit SequentialMultiplier(uint8_t M, uint8_t Q)
        : M_(M & 0xF), Q_(Q & 0xF)
    {
        reset();
    }

    /* Active-low reset: initialise all registers */
    void reset() {
        A_     = 0;
        E_     = 0;
        flag_  = true;   /* '1' */
        count_ = 4;
        Q_reg_ = Q_;
    }

    /* Simulate one rising clock edge */
    void tick() {
        if (count_ <= 0) return;

        /* Combinational sum (outside process in VHDL) */
        uint8_t sum5 = static_cast<uint8_t>((A_ + M_) & 0x1F);

        /* Add phase */
        if (Q_reg_ & 0x1) {
            if (flag_) {
                A_ = sum5 & 0xF;
                E_ = (sum5 >> 4) & 0x1;
            }
        }

        flag_ = !flag_;

        /* Shift phase */
        if (!flag_) {
            uint8_t lsb_A = A_ & 0x1;
            A_     = static_cast<uint8_t>(((E_ << 3) | (A_ >> 1)) & 0xF);
            Q_reg_ = static_cast<uint8_t>(((lsb_A << 3) | (Q_reg_ >> 1)) & 0xF);
            count_--;
        }
    }

    /* Run all 8 ticks to complete the multiplication */
    void run() {
        for (int i = 0; i < 8; ++i) tick();
    }

    /* Read the 8-bit product output */
    uint8_t product() const {
        return static_cast<uint8_t>(((A_ & 0xF) << 4) | (Q_reg_ & 0xF));
    }

    /* Cycle count remaining */
    int remainingCycles() const { return count_; }

    /* Verbose step-by-step trace */
    void runVerbose() {
        reset();
        std::cout << "  Tick | A    | E | Q_reg | count\n";
        std::cout << "  -----|------|---|-------|------\n";
        for (int i = 0; i < 8; ++i) {
            tick();
            std::cout << "  " << std::setw(4) << (i + 1)
                      << " | " << std::setw(4) << static_cast<int>(A_)
                      << " | " << static_cast<int>(E_)
                      << " | " << std::setw(5) << static_cast<int>(Q_reg_)
                      << " | " << count_ << "\n";
        }
    }

private:
    uint8_t M_, Q_;          /* Inputs (fixed after construction) */
    uint8_t A_    = 0;       /* Accumulator                       */
    uint8_t E_    = 0;       /* Extension / carry bit             */
    uint8_t Q_reg_= 0;       /* Shifted copy of Q                 */
    bool    flag_ = true;    /* Phase flag                        */
    int     count_= 4;       /* Remaining shift cycles            */
};

/* ============================================================
 * Helper: run one test case
 * ============================================================ */
struct TestCase { uint8_t M, Q; };

bool runTest(const TestCase& tc, bool verbose = false) {
    SequentialMultiplier mul(tc.M, tc.Q);

    if (verbose) {
        std::cout << "\n  Trace for M=" << +tc.M << "  Q=" << +tc.Q << "\n";
        mul.runVerbose();
    } else {
        mul.run();
    }

    uint8_t expected = static_cast<uint8_t>(tc.M * tc.Q);
    uint8_t got      = mul.product();
    bool    ok       = (got == expected);

    std::cout << std::setw(4) << +tc.M
              << std::setw(5) << +tc.Q
              << std::setw(11) << +expected
              << std::setw(9)  << +got
              << "    " << (ok ? "PASS" : "FAIL") << "\n";
    return ok;
}

/* ============================================================
 * main
 * ============================================================ */
int main() {
    std::cout << "=== 4-bit Sequential Shift-and-Add Multiplier (C++) ===\n\n";
    std::cout << std::setw(4) << "M"
              << std::setw(5) << "Q"
              << std::setw(11) << "Expected"
              << std::setw(9)  << "Got"
              << "    Status\n";
    std::cout << std::string(42, '-') << "\n";

    std::vector<TestCase> tests = {
        {3,4}, {5,6}, {7,3}, {15,15},
        {0,9}, {1,15},{6,11},{9,14}
    };

    int pass = 0, fail = 0;
    for (auto& tc : tests)
        runTest(tc) ? ++pass : ++fail;

    std::cout << std::string(42, '-') << "\n";
    std::cout << "Results: " << pass << " PASS / " << fail << " FAIL\n";

    /* Show verbose trace for one interesting case */
    {
        SequentialMultiplier mul(7, 3);
        std::cout << "\n--- Verbose trace: M=7, Q=3 ---\n";
        mul.runVerbose();
        std::cout << "Product = " << +mul.product() << "\n";
    }

    return (fail == 0) ? 0 : 1;
}
