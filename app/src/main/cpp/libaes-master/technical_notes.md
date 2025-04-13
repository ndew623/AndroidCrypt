# Advanced Encryption Standard Library

This library implements a time-optimized library for the AES cryptographic
cipher defined in
[FIPS 197](https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf) using C++.
The library also provides an implementation of the AES Key Wrap and AES Key
Wrap with Padding algorithms defined in IETF RFC 3394 and RFC 5649.

To avoid the use macros for performance, C++ introduced `constexpr` functions.
Those significantly improve the performance of code as compared to regular
function calls.  This library makes heavy use of such `constexpr` functions.
However, testing has shown that the performance benefits are only realized when
compiled with compiler optimizations enabled.  If using this library in
environments where `constexpr` function optimizations do not exist, those
functions should be converted to C-style macros.

## Implementation Notes

The AES standard defines a number of functions and operations that can be more
easily understood when examined in code that more closely aligns with how the
standard defines those functions.  However, implementing the library in that
was is less optimal.

This section will introduce those functions and present C-style macros that
may help in understanding the process that ultimately led to the table-based
logic.

### Key Expansion

The very first steps in preparing for encrypting plaintext is to expand
the encryption key.  This essentially expands the key such that bits of the
original key are used in multiple "rounds" during the encryption process.
The programmatic logic for the key expansion is found in the function
`AES::SetKey()` and is detailed in FIPS 197 Section 5.2.  Byte substitution
is a component of the key expansion logic and is described below.  Byte
substitution is also a significant component in the overall encryption
process.

### Encryption Steps

The process of encrypting data involves the following four operations:

  1. Byte substitution (S-box)
  2. Shifting rows in a matrix
  3. Mixing columns in a matrix
  4. XORing the encryption round key

#### Byte Substitution

Perhaps one of the most trivial operations is substitution.  AES defines
a 256-octet matrix called the "S-box" (or "substitution box") that is used to
substitute one octet for another.  This process strengthens the cipher
by obscuring keying material and intermediate processing results.

#### Shifting Rows

Each step in AES operates on a 16 octet 4x4 matrix referred to as the "state
table".  The `ShiftRows()` function moves each row of those rows by some number
of places as explained in section 5.1.2 of FIPS 197.

In code, this function might look like this:

```cpp
#define ShiftRows(x0, x1, x2, x3) \
    { \
        /* Shift row 1 left by one column */ \
        temp = ((x0) & 0x00ff0000); \
        (x0) = ((x0) & 0xff00ffff) | ((x1) & 0x00ff0000); \
        (x1) = ((x1) & 0xff00ffff) | ((x2) & 0x00ff0000); \
        (x2) = ((x2) & 0xff00ffff) | ((x3) & 0x00ff0000); \
        (x3) = ((x3) & 0xff00ffff) | temp; \
        /* Shift row 2 left by two columns */ \
        temp = ((x0) & 0x0000ff00); \
        (x0) = ((x0) & 0xffff00ff) | ((x2) & 0x0000ff00); \
        (x2) = ((x2) & 0xffff00ff) | temp; \
        temp = ((x1) & 0x0000ff00); \
        (x1) = ((x1) & 0xffff00ff) | ((x3) & 0x0000ff00); \
        (x3) = ((x3) & 0xffff00ff) | temp; \
        /* Shift row 3 left by thee columns (i.e., one to the right) */ \
        temp = ((x3) & 0x000000ff); \
        (x3) = ((x3) & 0xffffff00) | ((x2) & 0x000000ff); \
        (x2) = ((x2) & 0xffffff00) | ((x1) & 0x000000ff); \
        (x1) = ((x1) & 0xffffff00) | ((x0) & 0x000000ff); \
        (x0) = ((x0) & 0xffffff00) | temp; \
    }
```

In practice, though, calling this function repeatedly is very slow.  Thus,
you will not find code like this in this implementation.  Rather, this
steps and the next function are combined into a single step.

#### Mixing Columns

This function contains most of the core mathematical operations defined
in the AES standard.  The purpose is to transform the values in each
column of the matrix to produce a more cryptographically secure output.

One of the core functions defined as a part of this process is called
`xtime()`.  It is a multiplication operation that follows the rules for
multiplication over the Galois Field defined in the AES Specification.
At the standard explains in section 4.2.1, multiplying by x "can be
implemented at the byte level as a left shift and a subsequent conditional
bitwise XOR with 0x1b."  In code, that function could be defined as follows:

```cpp
#define xtime(x) (((x) & 0x80) ? ((((x) << 1) & 0xff) ^ 0x1b) : ((x) << 1))
```

In Section 5.1.3 of FIPS 197, the matrix multiplication operation,
which relies on `xtime()`. The `MixColumn()` function multiplies a single
32-bit column by the polynomial `{03}x3 + {01}x2 + {01}x + {02}`.  Multiplying
`x` by `{01}` is `x`.  Multiplying `x` by `{02}` is the same as `xtime(x)`.
Multiplying `x` by `{03}` is the same as multiplying `x` by `{01} * {02}`, or
`x ^ xtime(x)`.

In code, the `MixColumns()` operation for a single column of the matrix would
look like this:

```cpp
#define MixColumn(x) \
    ( \
        /* ENC 0 */ ( (xtime((x) >> 24) ^ \
        /* ENC 1 */     ((((x) >> 16) & 0xff) ^ xtime(((x) >> 16) & 0xff)) ^ \
        /* ENC 2 */     (((x) >> 8) & 0xff) ^ \
        /* ENC 3 */     ((x) & 0xff) ) << 24 ) | \
        /* ENC 0 */ ( (((x) >> 24) ^ \
        /* ENC 1 */     xtime((((x) >> 16) & 0xff)) ^ \
        /* ENC 2 */     ((((x) >> 8) & 0xff) ^ xtime(((x) >> 8) & 0xff)) ^ \
        /* ENC 3 */     ((x) & 0xff) ) << 16 ) | \
        /* ENC 0 */ ( (((x) >> 24) ^ \
        /* ENC 1 */     (((x) >> 16) & 0xff) ^ \
        /* ENC 2 */     xtime(((x) >> 8) & 0xff) ^ \
        /* ENC 3 */     (((x) & 0xff) ^ xtime((x) & 0xff)) ) << 8 ) | \
        /* ENC 0 */ ( ((((x) >> 24) ^ xtime((x) >> 24)) ^ \
        /* ENC 1 */     (((x) >> 16) & 0xff) ^ \
        /* ENC 2 */     (((x) >> 8) & 0xff) ^ \
        /* ENC 3 */     xtime((x) & 0xff) )) \
    )
```

Performing this operations is slow, but by first performing S-box substitution
and creating a table holding the pre-computed values results in excellent
performance.  That's what the encrypting constants tables `Enc0..Enc3` are.
The `ENC 0` style labels in the code above hint at how those tables are
constructed.

Taking each of the `ENC 0` rows and putting those together, then doing the
same for each of the other rows, we get the following code.

```cpp
#define EncTerm0(x) ( (xtime((x)) << 24) | \
                      ((x) << 16) | \
                      ((x) << 8) | \
                      ((x) ^ xtime((x))) )

#define EncTerm1(x) ( (((x) ^ xtime((x))) << 24) | \
                      (xtime((x)) << 16) | \
                      ((x) << 8) | \
                      ((x)) )

#define EncTerm2(x) ( ((x) << 24) | \
                      (((x) ^ xtime((x))) << 16) | \
                      (xtime((x)) << 8) | \
                      ((x)) )

#define EncTerm3(x) ( ((x) << 24) | \
                      ((x) << 16) | \
                      (((x) ^ xtime((x))) << 8) | \
                      (xtime((x))) )
```

The above macros were used to create the actual encrypting constants tables.
These hold the constant values in the terms of the equations defined for the
`MixColumns()` operation.  Before performing `MixColumns()`, a call to
`SubBytes()` is performed.  The order doesn't matter since the values can be
XORed in any order, so doing the byte substitution first and having it as a
part of the constants speeds execution.  The encrypting constants tables in use
were created by calling `EncTerm0(Sbox[i])` for `i` between 0 and 255.  That
particular table corresponds with column 1 of the matrix shown at the top of
page 16 of FIPS 197.  Likewise, `EncTerm1(Sbox[i])` corresponds to the second
column, and so forth.  There are a total of four encrypting constants tables.

The tables `Enc0..Enc3` hold constants that align with the column in the
matrix shown in Section 5.1.3 of FIPS 197 and are used for all rounds
of encryption, except for the final round.  The final round of encryption
is similar to the previous rounds, except that it does not perform the
`MixColumns()` step.  So for the final round, the code consults the
S-box table, shifts the rows, and adds (XOR) the round key.

Putting it all together, what is implemented in the code is a function
called `MixColShiftRow()` that does S-box substitution (by virtue of
the encrypting constants tables already having those substitutions),
shifting rows, and mixing columns in a single step.

It's worth noting that each of the terms of `EncTermN()` contain the same
octet values, but ordered differently.  This results in the
`Enc0..Enc3` tables containing the same values ordered like this:

```text
    Enc0[i] = [ a b b d ]
    Enc1[i] = [ d a b b ]
    Enc2[i] = [ b d a b ]
    Enc3[i] = [ b b d a ]
```

In effect, each successive table is equal to the previous table after applying
a single 8-bit rotation to the right on each element of the table.  One could
utilize a single table and perform bit rotations, though performance is
negatively impacted by doing that as compared to using four separate
pre-computed tables.

### Decryption Steps

Decryption mirrors encryption with a set of processes that work with inverse
logic:

  1. Inverse shifting rows
  2. Inverse byte substitution
  3. XORing the decryption round key
  4. Inverse mix columns

#### Inverse Byte Substitution

Just as octets were substituted during the encryption step, octets are
inversely substituted to recover the original value when decrypting.
For this, there is a table called the `InverseSbox()`.

#### Inverse Row Shifting

Section 5.3.1 of FIPS 197 defined how rows are to be inversely shifted.
As with encryption, this can be logically expressed in code as shown
below, though it is not an efficient implementation.

```cpp
#define InvShiftRows(x0, x1, x2, x3) \
    { \
        /* Shift row 1 right by one column */ \
        temp = ((x3) & 0x00ff0000); \
        (x3) = ((x3) & 0xff00ffff) | ((x2) & 0x00ff0000); \
        (x2) = ((x2) & 0xff00ffff) | ((x1) & 0x00ff0000); \
        (x1) = ((x1) & 0xff00ffff) | ((x0) & 0x00ff0000); \
        (x0) = ((x0) & 0xff00ffff) | temp; \
        /* Shift row 2 left by two columns */ \
        temp = ((x0) & 0x0000ff00); \
        (x0) = ((x0) & 0xffff00ff) | ((x2) & 0x0000ff00); \
        (x2) = ((x2) & 0xffff00ff) | temp; \
        temp = ((x1) & 0x0000ff00); \
        (x1) = ((x1) & 0xffff00ff) | ((x3) & 0x0000ff00); \
        (x3) = ((x3) & 0xffff00ff) | temp; \
        /* Shift row 3 right by thee columns (i.e., one to the left) */ \
        temp = ((x0) & 0x000000ff); \
        (x0) = ((x0) & 0xffffff00) | ((x1) & 0x000000ff); \
        (x1) = ((x1) & 0xffffff00) | ((x2) & 0x000000ff); \
        (x2) = ((x2) & 0xffffff00) | ((x3) & 0x000000ff); \
        (x3) = ((x3) & 0xffffff00) | temp; \
    }
```

The key to higher performance is, again, combining these inverse operations.

#### Inverse Mixing Columns

The `InvMixColumn()` macro below multiplies a single 32-bit column by the
polynomial `{0b}x3 + {0d}x2 + {09}x + {0e}`.  Multiplying by these
coefficients is equal to the following:

```text
     {0b} = x * ({01} ^ {02} ^ {08})
     {0d} = x * ({01} ^ {04} ^ {08})
     {09} = x * ({01} ^ {08})
     {0e} = x * ({02} ^ {04} ^ {08})
```

These expand as explained in Section 4.2.1 of FIPS 197.  For example,
assume the multiplier is `0x57`, then multiplying by `{0b}` results in
the following:

```text
                     {01}   {02}   {08}
     0x57 * {0x0b} = 0x57 ^ 0xae ^ 0x8e = 0x77

                     {01}   {04}   {08}
     0x57 * {0x0d} = 0x57 ^ 0x47 ^ 0x8e = 0x9e

                     {01}   {08}
     0x57 * {0x09} = 0x57 ^ 0x8e        = 0xd9

                     {02}   {04}   {08}
     0x57 * {0x0e} = 0xae ^ 0x47 ^ 0x8e = 0x67

  The value of x * {01} is x
  The value of x * {02} is xtime(x)
  The value of x * {04} is xtime(xtime(x))
  The value of x * {08} is xtime(xtime(xtime(x)))
```

Performing these operations is slow, but by first performing Inverse S-box
substitution and creating a table holding the pre-computed values results in
excellent performance.  Refer to FIP 197 Section 5.3.3 for a more complete
explanation.

The following macros are just to make the `InvMixColumn()` macro more
readable.

```cpp
#define xtime4(x)     xtime(xtime(x))
#define xtime8(x)     xtime(xtime(xtime(x)))
#define xtime_0x0b(x) ((x) ^ xtime(x) ^ xtime8(x))
#define xtime_0x0d(x) ((x) ^ xtime4(x) ^ xtime8(x))
#define xtime_0x09(x) ((x) ^ xtime8(x))
#define xtime_0x0e(x) (xtime(x) ^ xtime4(x) ^ xtime8(x))
```

The `InvMixColumn()` macro is here as documentation and is not used in
compiled code since it operates too slowly.  As with the encryption
explanation, we take related terms in the operation and put them into a
decrypting constants table.

```cpp
#define InvMixColumn(x) \
    ( \
        /* DEC 0 */ (((xtime_0x0e(((x) >> 24))       ) ^ \
        /* DEC 1 */   (xtime_0x0b(((x) >> 16)) & 0xff) ^ \
        /* DEC 2 */   (xtime_0x0d(((x) >>  8)) & 0xff) ^ \
        /* DEC 3 */   (xtime_0x09(((x)      )) & 0xff)) << 24 ) | \
        /* DEC 0 */ (((xtime_0x09(((x) >> 24))       ) ^ \
        /* DEC 1 */   (xtime_0x0e(((x) >> 16)) & 0xff) ^ \
        /* DEC 2 */   (xtime_0x0b(((x) >>  8)) & 0xff) ^ \
        /* DEC 3 */   (xtime_0x0d(((x)      )) & 0xff)) << 16 ) | \
        /* DEC 0 */ (((xtime_0x0d(((x) >> 24))       ) ^ \
        /* DEC 1 */   (xtime_0x09(((x) >> 16)) & 0xff) ^ \
        /* DEC 2 */   (xtime_0x0e(((x) >>  8)) & 0xff) ^ \
        /* DEC 3 */   (xtime_0x0b(((x)      )) & 0xff)) <<  8 ) | \
        /* DEC 0 */ (((xtime_0x0b(((x) >> 24))       ) ^ \
        /* DEC 1 */   (xtime_0x0d(((x) >> 16)) & 0xff) ^ \
        /* DEC 2 */   (xtime_0x09(((x) >>  8)) & 0xff) ^ \
        /* DEC 3 */   (xtime_0x0e(((x)      )) & 0xff))       ) \
    )
```

Macros for creating the decrypting constants tables `Dec0..Dec3` are shown
below.  These tables hold the constant values in the terms of the equations
defined in the `InvMixColumns()` operation.  Before performing
`InvMixColumns()`, a call to `InvSubBytes()` is performed.  The order doesn't
matter since the values can be XORed in any order, so doing the byte
substitution first and having it as a part of the constants speeds execution.
These macros are not used in compiled code, but were used to generate the
decrypting constants tables by calling, as an example,
`DecTerm0(InverseSbox[i])` for i between 0 and 255 to produce Dec0.

```cpp
#define DecTerm0(x) ( (xtime_0x0e(x) << 24) | \
                      (xtime_0x09(x) << 16) | \
                      (xtime_0x0d(x) <<  8) | \
                      (xtime_0x0b(x)      ) )

#define DecTerm1(x) ( (xtime_0x0b(x) << 24) | \
                      (xtime_0x0e(x) << 16) | \
                      (xtime_0x09(x) <<  8) | \
                      (xtime_0x0d(x)      ) )

#define DecTerm2(x) ( (xtime_0x0d(x) << 24) | \
                      (xtime_0x0b(x) << 16) | \
                      (xtime_0x0e(x) <<  8) | \
                      (xtime_0x09(x)      ) )

#define DecTerm3(x) ( (xtime_0x09(x) << 24) | \
                      (xtime_0x0d(x) << 16) | \
                      (xtime_0x0b(x) <<  8) | \
                      (xtime_0x0e(x)      ) )
```

The tables `Dec0..Dec3` hold constants that align with the column in the
matrix shown in Section 5.3.3 of FIPS 197 and are used for all rounds
of decryption, except for the final round.

As observed with the encoding constants tables, note the elements of each
of the `Dec0..Dec3` tables have the same values, but positioned differently.
Specifically, the order can be observed as:

```text
    Dec0[i] = [ a b c d ]
    Dec1[i] = [ d a b c ]
    Dec2[i] = [ c d a b ]
    Dec3[i] = [ b c d a ]
```

In effect, each successive table is equal to the previous table after applying
a single 8-bit rotation to the right on each element of the table.
