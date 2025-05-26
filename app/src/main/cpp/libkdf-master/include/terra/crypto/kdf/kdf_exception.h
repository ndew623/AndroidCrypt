/*
 *  kdf_exception.h
 *
 *  Copyright (C) 2024
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      Define an exception type that will be thrown by various KDF functions.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <stdexcept>

namespace Terra::Crypto::KDF
{

// Define an exception class for KDF-related exceptions
class KDFException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

} // Terra::Crypto::KDF
