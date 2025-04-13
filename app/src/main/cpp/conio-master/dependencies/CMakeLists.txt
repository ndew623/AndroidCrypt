if(conio_BUILD_TESTS)
    # Enable fetching content
    include(FetchContent)

    # Set the root of the Terrapane source repositories
    if(DEFINED ENV{TERRAPANE_SOURCE_URI})
        set(TERRAPANE_SOURCE_URI "$ENV{TERRAPANE_SOURCE_URI}")
    else()
        set(TERRAPANE_SOURCE_URI "https://github.com/terrapane")
    endif()

    # Fetch the Simple Test Framework library
    FetchContent_Declare(stf
        GIT_REPOSITORY ${TERRAPANE_SOURCE_URI}/stf.git
        GIT_TAG        v1.0.1
        GIT_SHALLOW    true)

    # It is not necessary to install the STF library
    set(stf_INSTALL OFF)

    # Make dependencies available
    FetchContent_MakeAvailable(stf)
endif()
