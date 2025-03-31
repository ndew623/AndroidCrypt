# Enable fetching content
include(FetchContent)

# Set the root of the Terrapane source repositories
if(DEFINED ENV{TERRAPANE_SOURCE_URI})
    set(TERRAPANE_SOURCE_URI "$ENV{TERRAPANE_SOURCE_URI}")
else()
    set(TERRAPANE_SOURCE_URI "https://github.com/terrapane")
endif()

# Retrieve STF only if building tests
if(aescrypt_engine_BUILD_TESTS)
    # Fetch the Simple Test Framework library
    FetchContent_Declare(stf
        GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/stf.git
        GIT_TAG        master
        GIT_SHALLOW    true)

    # It is not necessary to install the STF library
    set (stf_INSTALL OFF)

    # Make STF available
    FetchContent_MakeAvailable(stf)
endif()

# Fetch the AES Crypt Engine
FetchContent_Declare(aescrypt_engine
        GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/aescrypt_engine.git
        GIT_TAG        master
        GIT_SHALLOW    true)

# Fetch the AES library
FetchContent_Declare(libaes
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/libaes.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch security utilities library
FetchContent_Declare(secutil
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/secutil.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch character-related utilities
FetchContent_Declare(charutil
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/charutil.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch the random generator library
FetchContent_Declare(random
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/random.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch the hashing library
FetchContent_Declare(libhash
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/libhash.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch the Key Derivation Function library
FetchContent_Declare(libkdf
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/libkdf.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# Fetch the Logger library
FetchContent_Declare(logger
    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/logger.git
    GIT_TAG        master
    GIT_SHALLOW    true)

# If not installing aescrypt_engine, turn off installation for dependencies
if(NOT aescrypt_engine_INSTALL)
    set(libaes_INSTALL OFF)
    set(secutil_INSTALL OFF)
    set(charutil_INSTALL OFF)
    set(random_INSTALL OFF)
    set(libhash_INSTALL OFF)
    set(libkdf_INSTALL OFF)
    set(logger_INSTALL OFF)
endif()

# Make dependencies available
FetchContent_MakeAvailable(aescrypt_engine libaes secutil charutil random libhash libkdf logger)
