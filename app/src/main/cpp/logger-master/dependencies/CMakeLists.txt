# Set the root of the Terrapane source repositories
if(DEFINED ENV{TERRAPANE_SOURCE_URI})
    set(TERRAPANE_SOURCE_URI "$ENV{TERRAPANE_SOURCE_URI}")
else()
    set(TERRAPANE_SOURCE_URI "https://github.com/terrapane")
endif()

# Enable fetching content
include(FetchContent)

if(logger_BUILD_TESTS)
    # Fetch the Simple Test Framework library
    FetchContent_Declare(stf
        GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/stf.git
        GIT_TAG        v1.0.2
        GIT_SHALLOW    true)

    # It is not necessary to install the STF library
    set(stf_INSTALL OFF)

    # Make STF available
    FetchContent_MakeAvailable(stf)
endif()

# Fetch the console I/O library
#FetchContent_Declare(conio
#    GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/conio.git
#    GIT_TAG        v1.0.4
#    GIT_SHALLOW    true)

# If not installing logger, turn off installation for dependencies
#set(conio_INSTALL ${logger_INSTALL})

# Make dependencies available
#FetchContent_MakeAvailable(conio)
