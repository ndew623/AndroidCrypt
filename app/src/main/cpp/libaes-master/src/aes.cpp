/*
 *  aes.cpp
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file implements the AES object that performs encryption and
 *      decryption as specified in FIPS 197 ("Advanced Encryption Standard").
 *      This object utilizes one of the underlying AES engines to perform
 *      the actual encryption.
 *
 *  Portability Issues:
 *      None.
 */

#include <algorithm>
#include <terra/secutil/secure_erase.h>
#include <terra/crypto/cipher/aes.h>
#include "aes_tables.h"
#include "aes_utilities.h"
#include "aes_universal.h"
#include "aes_intel.h"
#include "cpu_check.h"

namespace Terra::Crypto::Cipher
{

/*
 *  AES::AES()
 *
 *  Description:
 *      This is a constructor for the AES object with no given key.
 *      It will create the underlying AES engine.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AES::AES()
{
    // Create the AES engine
    CreateEngine();
}

/*
 *  AES::AES()
 *
 *  Description:
 *      This is a constructor for the AES object that accepts a span of octets
 *      as input that contains the key.
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *      This corresponds to 128, 192, and 256 bits.
 *
 *  Comments:
 *      None.
 */
AES::AES(const std::span<const std::uint8_t> key) : AES()
{
    aes_engine->SetKey(key);
}

/*
 *  AES::AES()
 *
 *  Description:
 *      This is a copy constructor for the AES object that accepts a reference
 *      to another AES object as a parameter.
 *
 *  Parameters:
 *      other [in]
 *          The other AES object from which to copy values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AES::AES(const AES &other)
{
    // Create an engine like that of the other object
    switch (other.aes_engine->GetEngineType())
    {
        case AESEngineType::Universal:
            {
                AESUniversal *other_engine =
                    dynamic_cast<AESUniversal *>(other.aes_engine.get());
                aes_engine = std::make_unique<AESUniversal>(*other_engine);
            }
            break;

        case AESEngineType::Intel:
            {
                AESIntel *other_engine =
                    dynamic_cast<AESIntel *>(other.aes_engine.get());
                aes_engine = std::make_unique<AESIntel>(*other_engine);
            }
            break;

        default:
            // Should only happen if there was an allocation failure previously
            throw AESException("Failed to determine AES engine type");
            break;
    }
}

/*
 *  AES::AES()
 *
 *  Description:
 *      This is a move constructor for the AES object that accepts a reference
 *      to another AES object as a parameter.
 *
 *  Parameters:
 *      other [in]
 *          The other AES object from which to move values.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AES::AES(AES &&other) noexcept : AES()
{
    // Clear this engine's key and state data
    aes_engine->ClearKeyState();

    // Swap the engine in this object with other
    std::swap(aes_engine, other.aes_engine);
}

/*
 *  AES::SetKey()
 *
 *  Description:
 *      This function will set the key to be used for subsequent calls to
 *      Encrypt() or Decrypt().
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *
 *  Comments:
 *      None.
 */
void AES::CreateEngine()
{
    // If the processor support AES-NI instructions, try the Intel engine
    if (CPUSupportsAES_NI())
    {
        // Use the AES engine that uses AES-NI instructions
        aes_engine = std::make_unique<AESIntel>();

        // Reset the pointer if AES-NI cannot be used
        if (aes_engine->GetEngineType() == AESEngineType::Unavailable)
        {
            aes_engine.reset();
        }
    }

    // If no other engine is available, use the universal engine
    if (!aes_engine)
    {
        // Use the universal engine that works on all processors
        aes_engine = std::make_unique<AESUniversal>();
    }
}

/*
 *  AES::SetKey()
 *
 *  Description:
 *      This function will set the key to be used for subsequent calls to
 *      Encrypt() or Decrypt().
 *
 *  Parameters:
 *      key [in]
 *          The encryption key to use with this instance of the object.
 *
 *  Returns:
 *      Nothing, though an exception will be thrown if the key provided is not
 *      one of 16, 24, or 32 octets in length as required by the standard.
 *
 *  Comments:
 *      None.
 */
void AES::SetKey(const std::span<const std::uint8_t> key)
{
    aes_engine->SetKey(key);
}

/*
 *  AES::Encrypt()
 *
 *  Description:
 *      This function will encrypt a block of plaintext and return the
 *      ciphertext.
 *
 *  Parameters:
 *      plaintext [in]
 *          The 16-octet data block to encrypt.
 *
 *      ciphertext [out]
 *          The 16-octet encrypted data block.  AES operates in place, so this
 *          span may be the same memory location as the plaintext span.
 *
 *  Returns:
 *      Nothing, though the encrypted data is stored in the ciphertext
 *      parameter as output.
 *
 *  Comments:
 *      None.
 */
void AES::Encrypt(const std::span<const std::uint8_t, 16> plaintext,
                  std::span<std::uint8_t, 16> ciphertext) noexcept
{
    aes_engine->Encrypt(plaintext, ciphertext);
}

/*
 *  AES::Decrypt()
 *
 *  Description:
 *      This function will decrypt a block of ciphertext and return the
 *      plaintext.
 *
 *  Parameters:
 *      ciphertext [in]
 *          The 16-octet data block to decrypt.
 *
 *      plaintext [out]
 *          The 16-octet decrypted data block.  AES operates in place, so this
 *          span may be the same memory location as the ciphertext span.
 *
 *  Returns:
 *      Nothing, though the decrypted data is stored in the plaintext
 *      parameter as output.
 *
 *  Comments:
 *      None.
 */
void AES::Decrypt(const std::span<const std::uint8_t, 16> ciphertext,
                  std::span<std::uint8_t, 16> plaintext) noexcept
{
    aes_engine->Decrypt(ciphertext, plaintext);
}

/*
 *  AES::operator==()
 *
 *  Description:
 *      Compare two AES objects for equality.  Equality means that the
 *      two objects have the same key data.
 *
 *  Parameters:
 *      other [in]
 *          The other AES object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool AES::operator==(const AES &other) const
{
    bool result = false;

    // Ensure the engines are the same
    if (aes_engine->GetEngineType() != other.aes_engine->GetEngineType())
    {
        return result;
    }

    switch (aes_engine->GetEngineType())
    {
        case AESEngineType::Universal:
            {
                AESUniversal *this_engine =
                    dynamic_cast<AESUniversal *>(aes_engine.get());
                AESUniversal *other_engine =
                    dynamic_cast<AESUniversal *>(other.aes_engine.get());
                result = (*this_engine == *other_engine);
            }
            break;

        case AESEngineType::Intel:
            {
                AESIntel *this_engine =
                    dynamic_cast<AESIntel *>(aes_engine.get());
                AESIntel *other_engine =
                    dynamic_cast<AESIntel *>(other.aes_engine.get());
                result = (*this_engine == *other_engine);
            }
            break;

        default:
            throw AESException("Failed to determine AES engine type");
            break;
    }

    return result;
}

/*
 *  AES::operator!=()
 *
 *  Description:
 *      Compare two AES objects for inequality.
 *
 *  Parameters:
 *      other [in]
 *          The other AES object with which to compare.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
bool AES::operator!=(const AES &other) const
{
    return !(*this == other);
}

/*
 *  AES::operator=()
 *
 *  Description:
 *      Assign one AES object to another.
 *
 *  Parameters:
 *      other [in]
 *          The other AES from which to copy data.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      None.
 */
AES &AES::operator=(const AES &other)
{
    // If this is the same object, just return this
    if (this == &other) return *this;

    // Create an engine that aligns with the other object
    switch (other.aes_engine->GetEngineType())
    {
        case AESEngineType::Universal:
            {
                AESUniversal *other_engine =
                    dynamic_cast<AESUniversal *>(other.aes_engine.get());
                aes_engine = std::make_unique<AESUniversal>(*other_engine);
            }
            break;
        case AESEngineType::Intel:
            {
                AESIntel *other_engine =
                    dynamic_cast<AESIntel *>(other.aes_engine.get());
                aes_engine = std::make_unique<AESIntel>(*other_engine);
            }
            break;

        default:
            throw AESException("Failed to determine AES engine type");
            break;
    }

    return *this;
}

/*
 *  AES::operator=()
 *
 *  Description:
 *      Move assignment operator to assign another AES object to this one.
 *
 *  Parameters:
 *      other [in]
 *          The other AES from which to move data.
 *
 *  Returns:
 *      A reference to this AES object.
 *
 *  Comments:
 *      None.
 */
AES &AES::operator=(AES &&other) noexcept
{
    // If this is the same object, just return this
    if (this == &other) return *this;

    // Clear this engine's key and state data
    aes_engine->ClearKeyState();

    // Swap the aes_engine values
    std::swap(aes_engine, other.aes_engine);

    return *this;
}

} // namespace Terra::Crypto::Cipher
