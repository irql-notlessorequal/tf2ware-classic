#pragma newdecls required

#if defined _mimallet_random
  #endinput
#endif
#define _mimallet_random

#define INT32_MAX	0x7FFFFFFF
#define INT32_MIN  -0x7FFFFFFF

static int hi[2], lo[2];

int int64_ge(int a[2], int b[2])
{
	if (a[1] != b[1])
		return a[1] > b[1];
	else
		return a[0] >= b[0];
}

void int64_sub(int r[2], int a[2], int b[2])
{
	int alo = a[0], ahi = a[1];
	int blo = b[0], bhi = b[1];
	int borrow = alo < blo ? 1 : 0;

	r[0] = alo - blo;
	r[1] = ahi - bhi - borrow;
}

int int64_mod32(int a[2], int m)
{
	int high = a[1] % m;
	int low = a[0] % m;
	int base = (1 << 32) % m;
	int b = (65536 % m);

	base = (b * b) % m;

	int result = (high * base) % m;
	result = (result + (low % m)) % m;

	if (result < 0)
		result += m;	
	
	return result;
}

void int64_not(const int s[2], int out[2])
{
	out[0] = ~s[0];
	out[1] = ~s[1];
}

void int64_shl(const int input[2], int shift, int out[2])
{
	if (shift == 0)
	{
		out[0] = input[0];
		out[1] = input[1];
	}
	else if (shift < 32)
	{
		out[0] = (input[0] << shift) | ((input[1] >> (32 - shift)) & ((1 << shift) - 1));
		out[1] = input[1] << shift;
	}
	else if (shift < 64)
	{
		out[0] = input[1] << (shift - 32);
		out[1] = 0;
	}
	else
	{
		out[0] = 0;
		out[1] = 0;
	}
}

void int64_shr(const int input[2], int shift, int out[2])
{
	if (shift == 0)
	{
		out[0] = input[0];
		out[1] = input[1];
	}
	else if (shift < 32)
	{
		out[1] = (input[1] >> shift) | (input[0] << (32 - shift));
		out[0] = input[0] >> shift;
	}
	else if (shift < 64)
	{
		out[1] = input[0] >> (shift - 32);
		out[0] = input[0] < 0 ? -1 : 0;
	}
	else
	{
		out[1] = input[0] < 0 ? -1 : 0;
		out[0] = input[0] < 0 ? -1 : 0;
	}
}

void int64_add(const int a[2], const int b[2], int out[2])
{
	int low	  = a[1] + b[1];
	int carry = ((a[1] >= 0 && b[1] >= 0 && low < 0) || (a[1] < 0 && b[1] < 0 && low >= 0)) ? 1 : 0;
	out[1]	  = low;
	out[0]	  = a[0] + b[0] + carry;
}

void ranrot_seed(const int s[2])
{
	int64_not(s, hi);
	lo[0] = s[0];
	lo[1] = s[1];
}

void ranrot(int out[2])
{
	int t1[2], t2[2];

	// hi = (hi << 19) + (hi >> 23)
	int64_shl(hi, 19, t1);
	int64_shr(hi, 23, t2);
	int64_add(t1, t2, hi);

	// lo = (lo << 29) + (lo >> 31)
	int64_shl(lo, 29, t1);
	int64_shr(lo, 31, t2);
	int64_add(t1, t2, lo);

	// hi += lo
	int64_add(hi, lo, hi);

	// lo += hi
	int64_add(lo, hi, lo);

	// return hi
	out[0] = hi[0];
	out[1] = hi[1];
}

public void MimalletInitRand()
{
	int seed[2];
	
	/* Use the current system time as the
	 * base seed, this'll change soon. */
	GetTime(seed);
	
	/* Offset the seed with the Valve RNG(tm) */
	seed[0] &= GetURandomInt();
	seed[1] &= GetURandomInt();

	/* Initialize the PRNG. */
	ranrot_seed(seed);
}

public int MalletGetRand32()
{
	int array[2];
	ranrot(array);

	/* Ignore the upper 32-bits. */
	return array[1] & 0xffffffff;
}

public void MalletGetRand64(int a[2])
{
	ranrot(a);
}

public int MalletGetRandomInt(int min, int max)
{
	int out[2], range64[2], limit[2], tmp[2];
	int range = max - min + 1;
	if (range <= 0)
		return min; // handle bad input

	// range64 = {range, 0}
	range64[0] = range;
	range64[1] = 0;

	// limit = {0xFFFFFFFF, 0x7FFFFFFF} - (limit % range)
	limit[0] = -1;
	limit[1] = 0x7FFFFFFF;
	tmp[0] = int64_mod32(limit, range);
	tmp[1] = 0;
	int64_sub(limit, limit, tmp);

	do {
		ranrot(out);
	} while (int64_ge(out, limit));

	int val = int64_mod32(out, range);
	return min + val;
}