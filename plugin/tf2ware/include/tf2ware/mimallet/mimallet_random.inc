/**
 * MIT License
 * 
 * Copyright (c) 2025 Daniel Cota
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#pragma newdecls required

#define INT32_MAX	0x7FFFFFFF
#define INT32_MIN  -0x7FFFFFFF

#define x64_HIGH	0
#define x64_LOW		1

// Golden ratio constant for LoopMix128 - golden ratio fractional part * 2^64
static const int GR[2] = { 0x9e3779b9, 0x7f4a7c15 };

// For LoopMix128
int fast_loop[2] = { 0xDEADBEEF, 0x12345678 };
int slow_loop[2] = { 0xABCDEF01, 0x23456789 };
int mix[2] = { 0x1234567, 0x89ABCDEF };

static void add64(const int a[2], const int b[2], int result[2])
{
    int low = a[x64_LOW] + b[x64_LOW];
    int carry = (low < a[x64_LOW]);

    result[x64_LOW] = low;
    result[x64_HIGH] = a[x64_HIGH] + b[x64_HIGH] + carry;
}

static void mul64(const int a[2], const int b[2], int result[2])
{
    int a_lo_lo = a[x64_LOW] & 0xFFFF;
    int a_lo_hi = (a[x64_LOW] >> 16) & 0xFFFF;
    int a_hi_lo = a[x64_HIGH] & 0xFFFF;
    int a_hi_hi = (a[x64_HIGH] >> 16) & 0xFFFF;

    int b_lo_lo = b[x64_LOW] & 0xFFFF;
    int b_lo_hi = (b[x64_LOW]  >> 16) & 0xFFFF;
    int b_hi_lo = b[x64_HIGH] & 0xFFFF;
    int b_hi_hi = (b[x64_HIGH] >> 16) & 0xFFFF;

    // Calculate low * low
    int p0 = a_lo_lo * b_lo_lo;
    int p1 = a_lo_lo * b_lo_hi;
    int p2 = a_lo_hi * b_hi_lo;
    int p3 = a_hi_hi * b_hi_hi;

    // Combine for 32-bit low part
    int temp = (p1 & 0xFFFF) + (p2 & 0xFFFF) + (p0 >> 16);
    int low = (p0 & 0xFFFF) | ((temp & 0xFFFF) << 16);

    // Carry from low to high
    int carry = (temp >> 16) + (p1 >> 16) + (p2 >> 16) + p3;

    // High part: cross terms (a_hi * b_lo) and (a_lo * b_hi) (low words only)
    carry += a_hi_lo * b_lo_lo;
    carry += a_lo_lo * b_hi_lo;

    result[x64_LOW] = low;
    result[x64_HIGH] = carry;
}

static void xor64(const int a[2], const int b[2], int result[2])
{
	result[x64_HIGH] = a[x64_HIGH] ^ b[x64_HIGH];
    result[x64_LOW] = a[x64_LOW] ^ b[x64_LOW];
}

static void rotl64(const int a[2], int n, int result[2])
{
	n &= 63;
	
	if (n == 0)
	{
		result[0] = a[0];
		result[1] = a[1];
	}
	else if (n < 32)
	{
		result[0] = (a[0] << n) | (a[1] >> (32 - n));
		result[1] = (a[1] << n) | (a[0] >> (32 - n));
	}
	else if (n < 64)
	{
		result[0] = (a[1] << (n - 32)) | (a[0] >> (64 - n));
		result[1] = (a[0] << (n - 32)) | (a[1] >> (64 - n));
	}
}

static void loopMix128(int output[2])
{
	int t1[2];

	add64(mix, fast_loop, t1);
	mul64(GR, t1, output);

	/**
	 * We need to check if we've overflowed the fast_loop,
	 * and then treat the negative numbers as zero.
	 */
	if (fast_loop[x64_LOW] <= 0 || fast_loop[x64_HIGH] <= 0)
	{
		add64(slow_loop, GR, slow_loop);
		xor64(mix, slow_loop, mix);
	}

	rotl64(mix, 59, t1);
	add64(t1, fast_loop, mix);

	rotl64(fast_loop, 47, t1);
	add64(t1, GR, fast_loop);
}

static int abs(int x)
{
	/**
	 * We have no way of figuring out what
	 * we're running on so be rude and assume
	 * that we have only 32-bits to work with.
	 */
	int mask = x >> (4 * 8 - 1);
	return (x ^ mask) - mask;
}

public void MimalletInitRand()
{
	/**
	 * The algorithm sucks a bit and we need
	 * to offset the mix on Mallet startup in
	 * order to get actually reasonable values.
	 */
	int seed[2];

	seed[x64_HIGH] = GetURandomInt();
	seed[x64_LOW] = GetURandomInt();

	add64(mix, seed, mix);
}

public int MalletGetRand32()
{
	int array[2];
	loopMix128(array);

	/**
	 * I'm so sorry.
	 */
	return array[x64_LOW] ^ array[x64_HIGH];
}

public void MalletGetRand64(any a[2])
{
	int array[2];
	loopMix128(array);

	a[0] = array[0];
	a[1] = array[1];
}

public int MalletGetRandomInt(int min, int max)
{
	/**
	 * MalletGetRand32() is signed, this causes a plethora of issues.
	 * 
	 * We need to do abs() if dealing with positive values.
	 */
    if (min >= 0)
	{
		return min + abs(MalletGetRand32()) / (INT32_MAX / (max - min + 1) + 1);
	}
	else
	{
		return min + MalletGetRand32() / (INT32_MAX / (max - min + 1) + 1);
	}
}