/*
 * ============================================================
 * Project  : 4-bit Sequential Shift-and-Add Multiplier
 * File     : Multiplier.cs
 * Language : C# 10 / .NET 6+
 * Description:
 *   Clean OOP simulation of the VHDL sequential multiplier.
 *   The MultiplierEntity class models the hardware entity;
 *   Program.cs drives a full test suite with console output.
 * ============================================================
 */

using System;
using System.Collections.Generic;

namespace SequentialMultiplier
{
    /* ========================================================
     * Class: MultiplierState
     * Mirrors the VHDL signal set inside the architecture.
     * ======================================================== */
    public class MultiplierState
    {
        public byte  A     { get; set; } = 0;   // 4-bit accumulator
        public byte  E     { get; set; } = 0;   // extension / carry
        public byte  QReg  { get; set; } = 0;   // shifted multiplier
        public bool  Flag  { get; set; } = true; // phase flag ('1')
        public int   Count { get; set; } = 4;   // remaining cycles
    }

    /* ========================================================
     * Class: MultiplierEntity
     * Represents the VHDL entity "multi".
     * ======================================================== */
    public class MultiplierEntity
    {
        private readonly byte _m;   // multiplicand (fixed input)
        private readonly byte _q;   // multiplier   (fixed input)
        private MultiplierState _s  = new();

        public MultiplierEntity(byte m, byte q)
        {
            _m = (byte)(m & 0xF);
            _q = (byte)(q & 0xF);
            Reset();
        }

        /* Active-low reset */
        public void Reset()
        {
            _s = new MultiplierState { QReg = _q };
        }

        /* One rising clock edge */
        public void Tick()
        {
            if (_s.Count <= 0) return;

            // Combinational sum (concurrent signal in VHDL)
            byte sum5 = (byte)((_s.A + _m) & 0x1F);

            // Add phase
            if ((_s.QReg & 0x1) == 1)
            {
                if (_s.Flag)
                {
                    _s.A = (byte)(sum5 & 0xF);
                    _s.E = (byte)((sum5 >> 4) & 0x1);
                }
            }

            _s.Flag = !_s.Flag;

            // Shift phase
            if (!_s.Flag)
            {
                byte lsbA  = (byte)(_s.A & 0x1);
                _s.A    = (byte)(( (_s.E << 3) | (_s.A >> 1) ) & 0xF);
                _s.QReg = (byte)(( (lsbA  << 3) | (_s.QReg >> 1) ) & 0xF);
                _s.Count--;
            }
        }

        /* Run all 8 ticks */
        public void Run()
        {
            for (int i = 0; i < 8; i++) Tick();
        }

        /* 8-bit product output */
        public byte Product => (byte)(( (_s.A & 0xF) << 4 ) | (_s.QReg & 0xF));

        /* Step-by-step trace */
        public void RunVerbose()
        {
            Reset();
            Console.WriteLine($"  Trace  M={_m}  Q={_q}");
            Console.WriteLine("  Tick | A    | E | Q_reg | count");
            Console.WriteLine("  -----|------|---|-------|------");
            for (int i = 0; i < 8; i++)
            {
                Tick();
                Console.WriteLine($"  {i+1,4} | {_s.A,4} | {_s.E} | {_s.QReg,5} | {_s.Count}");
            }
        }
    }

    /* ========================================================
     * Program entry point
     * ======================================================== */
    internal static class Program
    {
        private record TestCase(byte M, byte Q);

        private static bool RunTest(TestCase tc, bool verbose = false)
        {
            var mul = new MultiplierEntity(tc.M, tc.Q);

            if (verbose)
                mul.RunVerbose();
            else
                mul.Run();

            byte expected = (byte)(tc.M * tc.Q);
            byte got      = mul.Product;
            bool ok       = got == expected;

            Console.WriteLine($"  {tc.M,4} {tc.Q,5} {expected,11} {got,9}    {(ok ? "PASS" : "FAIL")}");
            return ok;
        }

        public static int Main(string[] args)
        {
            Console.WriteLine("=== 4-bit Sequential Shift-and-Add Multiplier (C#) ===\n");
            Console.WriteLine($"{"M",4} {"Q",5} {"Expected",11} {"Got",9}    Status");
            Console.WriteLine(new string('-', 45));

            var tests = new List<TestCase>
            {
                new(3,  4), new(5,  6), new(7,  3), new(15, 15),
                new(0,  9), new(1, 15), new(6, 11), new(9,  14)
            };

            int pass = 0, fail = 0;
            foreach (var tc in tests)
                if (RunTest(tc)) pass++; else fail++;

            Console.WriteLine(new string('-', 45));
            Console.WriteLine($"Results: {pass} PASS / {fail} FAIL");

            // Verbose trace
            Console.WriteLine();
            var demo = new MultiplierEntity(7, 3);
            demo.RunVerbose();
            Console.WriteLine($"  Product = {demo.Product}");

            return fail == 0 ? 0 : 1;
        }
    }
}
