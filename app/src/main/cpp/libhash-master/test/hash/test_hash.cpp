/*
 *  test_hmac_sha.cpp
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This module will test code defined in the hash.h file.
 *
 *  Portability Issues:
 *      None.
 */

#include <terra/stf/stf.h>
#include <terra/crypto/hashing/hash.h>
#include <terra/crypto/hashing/sha256.h>

using namespace Terra::Crypto::Hashing;

STF_TEST(HASH, HashDigest)
{
    STF_ASSERT_EQ(GetHashDigestLength(HashAlgorithm::SHA1),    20);
    STF_ASSERT_EQ(GetHashDigestLength(HashAlgorithm::SHA256),  32);
    STF_ASSERT_EQ(GetHashDigestLength(HashAlgorithm::SHA384),  48);
    STF_ASSERT_EQ(GetHashDigestLength(HashAlgorithm::SHA512),  64);
    STF_ASSERT_EQ(GetHashDigestLength(HashAlgorithm::Unknown),  0);
}

STF_TEST(HASH, CreateHash)
{
    {
        auto hash = CreateHashObject(HashAlgorithm::SHA1);
        STF_ASSERT_EQ(HashAlgorithm::SHA1, hash->GetHashAlgorithm());
    }
    {
        auto hash = CreateHashObject(HashAlgorithm::SHA256);
        STF_ASSERT_EQ(HashAlgorithm::SHA256, hash->GetHashAlgorithm());
    }
    {
        auto hash = CreateHashObject(HashAlgorithm::SHA384);
        STF_ASSERT_EQ(HashAlgorithm::SHA384, hash->GetHashAlgorithm());
    }
    {
        auto hash = CreateHashObject(HashAlgorithm::SHA512);
        STF_ASSERT_EQ(HashAlgorithm::SHA512, hash->GetHashAlgorithm());
    }
}

STF_TEST(HASH, SHA256Test)
{
    auto hash = CreateHashObject(HashAlgorithm::SHA256);
    STF_ASSERT_EQ(HashAlgorithm::SHA256, hash->GetHashAlgorithm());

    hash->Input("abc");
    hash->Finalize();
    std::string result = hash->Result();

    STF_ASSERT_EQ(std::string("ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 "
                              "96177a9c b410ff61 f20015ad"),
                  result);
}
