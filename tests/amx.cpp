// SPDX-FileCopyrightText: Copyright (c) 2023 merryhime <https://mary.rs>
// SPDX-License-Identifier: MIT

#include <array>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <numeric>

#include <catch2/catch_test_macros.hpp>

#include "architecture.hpp"
#include "oaknut/oaknut.hpp"

using namespace oaknut;
using namespace oaknut::util;

#define T(HEX, CMD)                   \
    TEST_CASE(#CMD)                   \
    {                                 \
        using namespace oaknut;       \
        using namespace oaknut::util; \
                                      \
        std::uint32_t result;         \
        CodeGenerator code{&result};  \
                                      \
        code.CMD;                     \
                                      \
        REQUIRE(result == HEX);       \
    }

T(0x201007, AMX_LDX(X7))
T(0x201025, AMX_LDY(X5))
T(0x201049, AMX_STX(X9))
T(0x20106f, AMX_STY(X15))
T(0x201085, AMX_LDZ(X5))
T(0x2010a9, AMX_STZ(X9))
T(0x2010d8, AMX_LDZI(X24))
T(0x2010e3, AMX_STZI(X3))
T(0x20110c, AMX_EXTRX(X12))
T(0x20112b, AMX_EXTRY(X11))
T(0x201146, AMX_FMA64(X6))
T(0x201167, AMX_FMS64(X7))
T(0x201182, AMX_FMA32(X2))
T(0x2011ae, AMX_FMS32(X14))
T(0x2011cd, AMX_MAC16(X13))
T(0x2011eb, AMX_FMA16(X11))
T(0x201206, AMX_FMS16(X6))
T(0x201220, AMX_SET())
T(0x201221, AMX_CLR())
T(0x20125e, AMX_VECINT(X30))
T(0x20127d, AMX_VECFP(X29))
T(0x201291, AMX_MATINT(X17))
T(0x2012af, AMX_MATFP(X15))
T(0x2012c9, AMX_GENLUT(X9))

#if defined(ON_ARM64) & defined(__APPLE__)

#    include "oaknut/code_block.hpp"
#    include "oaknut/dual_code_block.hpp"

TEST_CASE("AMX_memcpy")
{
    std::array<uint8_t, 64> x_vec;
    x_vec.fill(1);

    std::array<uint8_t, 64> y_vec;
    y_vec.fill(0);

    CodeBlock mem{4096};
    CodeGenerator code{mem.ptr()};

    auto amx_memcpy = code.xptr<void (*)(void*, void*)>();

    mem.unprotect();

    code.AMX_SET();

    // Clear upper byte
    code.UBFX(X0, X0, 0, 56);
    code.UBFX(X1, X1, 0, 56);

    // Load vector X
    code.AMX_LDX(X0);

    // Store X into Y
    code.AMX_STX(X1);

    code.AMX_CLR();

    code.RET();

    mem.protect();
    mem.invalidate_all();

    amx_memcpy(x_vec.data(), y_vec.data());

    REQUIRE(x_vec == y_vec);
}

TEST_CASE("AMX_mac16-vector")
{
    std::array<uint16_t, 32> x_vec;
    std::iota(x_vec.begin(), x_vec.end(), 0);

    std::array<uint16_t, 32> y_vec;
    y_vec.fill(1);

    std::array<uint16_t, 32> z_vec;
    z_vec.fill(0);

    CodeBlock mem{4096};
    CodeGenerator code{mem.ptr()};

    mem.unprotect();

    auto amx_func = code.xptr<void (*)(void*, void*, void*)>();

    code.AMX_SET();

    // Ensure upper byte of addresses are clear
    code.UBFX(X0, X0, 0, 56);
    code.UBFX(X1, X1, 0, 56);
    code.UBFX(X2, X2, 0, 56);

    // Load vectors X, Y, and Z
    // 64 bytes of data each
    code.AMX_LDX(X0);
    code.AMX_LDY(X1);
    code.AMX_LDZ(X2);

    // Bit  Width
    // 63	1	Vector mode (1) or matrix mode (0)
    // 62	1	Z is i32 (1) or Z is i16 (0)	Ignored in vector mode; Z is always i16 there
    // 61	1	X is i8 (1) or X is i16 (0)
    // 60	1	Y is i8 (1) or Y is i16 (0)
    // 55	5	Right shift amount	Applied to x*y. When zero, sign of x and y inputs is less relevant.
    // 48	7	Ignored
    // 46	2	X enable mode
    // 41	5	X enable value	Meaning dependent upon associated mode
    // 39	2	Ignored
    // 37	2	Y enable mode	Ignored in vector mode
    // 32	5	Y enable value	Ignored in vector mode Meaning dependent upon associated mode
    // 30	2	Ignored
    // 29	1	Skip X input (1) or use X input (0)
    // 28	1	Skip Y input (1) or use Y input (0)
    // 27	1	Skip Z input (1) or use Z input (0)
    // 26	1	Ignored
    // 20	6	Z row	High bits ignored in matrix mode
    // 19	1	Ignored
    // 10	9	X offset (in bytes)
    // 9	1	Ignored
    // 0	9	Y offset (in bytes)
    code.MOV(X3, 0b1'0'0'0'00000'0000000'00'00000'00'00'00000'00'0'0'0'0'000000'0'000000000'0'000000000);

    // Z[i] += X[i] * Y[i]
    code.AMX_MAC16(X3);

    // Store Z
    code.AMX_STZ(X2);

    code.AMX_CLR();

    code.RET();

    mem.protect();
    mem.invalidate_all();

    amx_func(x_vec.data(), y_vec.data(), z_vec.data());

    REQUIRE(std::equal(x_vec.cbegin(), x_vec.cend(), z_vec.cbegin()));
}

TEST_CASE("AMX_mac16-matrix")
{
    std::array<uint16_t, 32> x_vec;
    std::iota(x_vec.begin(), x_vec.end(), 0);

    std::array<uint16_t, 32> y_vec;
    y_vec.fill(1);

    std::array<uint16_t, 32> z_vec;
    z_vec.fill(0);

    CodeBlock mem{4096};
    CodeGenerator code{mem.ptr()};

    mem.unprotect();

    auto amx_func = code.xptr<void (*)(void*, void*, void*)>();

    code.AMX_SET();

    // Ensure upper byte of addresses are clear
    code.UBFX(X0, X0, 0, 56);
    code.UBFX(X1, X1, 0, 56);
    code.UBFX(X2, X2, 0, 56);

    // Load vectors X, Y, and Z
    // 64 bytes of data each
    code.AMX_LDX(X0);
    code.AMX_LDY(X1);
    code.AMX_LDZ(X2);

    // Bit  Width
    // 63	1	Vector mode (1) or matrix mode (0)
    // 62	1	Z is i32 (1) or Z is i16 (0)	Ignored in vector mode; Z is always i16 there
    // 61	1	X is i8 (1) or X is i16 (0)
    // 60	1	Y is i8 (1) or Y is i16 (0)
    // 55	5	Right shift amount	Applied to x*y. When zero, sign of x and y inputs is less relevant.
    // 48	7	Ignored
    // 46	2	X enable mode
    // 41	5	X enable value	Meaning dependent upon associated mode
    // 39	2	Ignored
    // 37	2	Y enable mode	Ignored in vector mode
    // 32	5	Y enable value	Ignored in vector mode Meaning dependent upon associated mode
    // 30	2	Ignored
    // 29	1	Skip X input (1) or use X input (0)
    // 28	1	Skip Y input (1) or use Y input (0)
    // 27	1	Skip Z input (1) or use Z input (0)
    // 26	1	Ignored
    // 20	6	Z row	High bits ignored in matrix mode
    // 19	1	Ignored
    // 10	9	X offset (in bytes)
    // 9	1	Ignored
    // 0	9	Y offset (in bytes)
    code.MOV(X3, 0b0'0'0'0'00000'0000000'00'00000'00'00'00000'00'0'0'0'0'000000'0'000000000'0'000000000);

    // z[j][i] += x[i] * y[j]
    code.AMX_MAC16(X3);

    // Store Z
    code.AMX_STZ(X2);

    code.AMX_CLR();

    code.RET();

    mem.protect();
    mem.invalidate_all();

    amx_func(x_vec.data(), y_vec.data(), z_vec.data());

    REQUIRE(std::equal(x_vec.cbegin(), x_vec.cend(), z_vec.cbegin()));
}

TEST_CASE("AMX_sum32-rgba")
{
    constexpr std::uint32_t test_color = 0xaa'bb'cc'dd;
    // Given 16 32-bit integers, will be able to sum the bytes of each channel
    // into rows of Z
    // A proof-of-concept implementation for getting the average color of an image.
    std::array<uint32_t, 16> x_vec;
    x_vec.fill(test_color);

    // Four rows of Z
    std::array<std::array<uint32_t, 16>, 4> z_mat;
    for (auto& z_vec : z_mat) {
        z_vec.fill(0);
    }

    static_assert(sizeof(x_vec) == 64ULL);
    static_assert(sizeof(z_mat) == 64ULL * 4);

    CodeBlock mem{4096};
    CodeGenerator code{mem.ptr()};

    mem.unprotect();

    auto amx_func = code.xptr<void (*)(void*, void*)>();
    {
        code.AMX_SET();

        // Ensure upper byte of addresses are clear
        code.UBFX(X0, X0, 0, 56);

        // Load vector X
        // 64 bytes of data each
        code.AMX_LDX(X0);

        // Bit  Width
        // 63	1	Z is signed (1) or unsigned (0)
        // (47≠4) 63	1	X is signed (1) or unsigned (0)
        // 58	5	Right shift amount	Ignored when ALU mode in {5, 6}
        // 57	1	Ignored
        // 54	3	Must be zero	No-op otherwise
        // 53	1	Indexed load (1) or regular load (0)
        // (53=1) 52	1	Ignored
        // (53=1) 49	3	Register to index into
        // (53=1) 48	1	Indices are 4 bits (1) or 2 bits (0)
        // (53=1) 47	1	Indexed load of Y (1) or of X (0)
        // (53=0) 47	6	ALU mode
        // 46	1	Ignored
        // 42	4	Lane width mode	Meaning dependent upon ALU mode
        // 41	1	Ignored
        // (31=1) 35	6	Ignored
        // (31=1) 32	3	Broadcast mode
        // (31=0) 38	3	Write enable or broadcast mode
        // (31=0) 32	6	Write enable value or broadcast lane index	Meaning dependent upon associated mode
        // 31	1	Perform operation for multiple vectors (1) or just one vector (0)	M2 only (always reads as 0 on M1)
        // (47=4) 30	1	Saturate Z (1) or truncate Z (0)
        // (47=4) 29	1	Right shift is rounding (1) or truncating (0)
        // (47≠4) 29	2	X shuffle
        // 27	2	Y shuffle
        // (47=4) 26	1	Z saturation is signed (1) or unsigned (0)
        // (47≠4) 26	1	Y is signed (1) or unsigned (0)
        // (31=1) 25	1	"Multiple" means four vectors (1) or two vectors (0)	Top two bits of Z row ignored if operating on four vectors
        // 20	6	Z row	Low bits ignored in some lane width modes When 31=1, top bit or top two bits ignored
        // 19	1	Ignored
        // 10	9	X offset (in bytes)
        // 9	1	Ignored
        // 0	9	Y offset (in bytes)
        const uint64_t vec_op =
            // ALU mode
            //  0 : f = Z+(X * Y) >> s
            // 11 : f = Z+(X) >> s (M2 only)
            ((11ULL) << 47) |

            // Lane width mode:
            // 10: Z.u32[i] += f(X.u8[i], Y.u8[i])
            // Produces 64 32-bit integers, requiring 256 bytes of data total!
            // four rows of Z: interleaved quartet(perfect for RGBA!):
            // Z0	0	4	8	12	16	20	24	28	32	36	40	44	48	52	56	60 : A
            // Z1	1	5	9	13	17	21	25	29	33	37	41	45	49	53	57	61 : B
            // Z2	2	6	10	14	18	22	26	30	34	38	42	46	50	54	58	62 : G
            // Z3	3	7	11	15	19	23	27	31	35	39	43	47	51	55	59	63 : R
            ((10ULL) << 42);

        code.MOV(X2, vec_op);

        // Z[_][i] = f(X[i], X[i])
        code.AMX_VECINT(X2);

        // Store Z
        // We are writing to four rows of Z, each row having a color-channel
        // So it will need to be stored four times!

        for (std::size_t z_row = 0; z_row < 4; ++z_row) {
            code.MOV(X3, z_row);
            code.ADD(X2, X1, X3, LSL, 6);  // ptr = z_row * 64
            code.BFI(X2, X3, 56, 8);       // Set z_row to write
            code.AMX_STZ(X2);
        }

        code.AMX_CLR();

        code.RET();
    }

    mem.protect();
    mem.invalidate_all();

    amx_func(x_vec.data(), z_mat.data());

    for (std::size_t z_row = 0; z_row < 4; ++z_row) {
        const std::uint8_t expected_sum = static_cast<std::uint8_t>(test_color >> (z_row * 8));
        for (const auto& z_element : z_mat[z_row]) {
            REQUIRE(z_element == expected_sum);
            // std::printf("%02x ", z_element);
        }
        // std::putchar('\n');
    }
}

#endif