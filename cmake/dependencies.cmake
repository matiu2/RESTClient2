# C++
find_library(CPP c++)

# Lib sup c++
find_library(SUP_CPP supc++
             HINTS /usr/lib/gcc/x86_64-linux-gnu/5)

# Threads
find_package(Threads)

# Boost
FIND_PACKAGE(Boost 1.58 REQUIRED COMPONENTS system coroutine iostreams)
include_directories(${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES})

if (${ENABLE_TESTS})
  FIND_PACKAGE(Boost 1.58 REQUIRED COMPONENTS regex)
endif(${ENABLE_TESTS})

## Boost hana (not yet available in 1.58) (header only library)
ExternalProject_Add(hana
    PREFIX 3rd_party
    GIT_REPOSITORY https://github.com/boostorg/hana.git
    GIT_TAG v1.2.0
    GIT_SHALLOW 1
    TLS_VERIFY true
    TLS_CAINFO certs/DigiCertHighAssuranceEVRootCA.crt
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
)
SET(HANA_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rd_party/src/hana/include)
INCLUDE_DIRECTORIES(${HANA_INCLUDE_DIR})


# OpenSSL
FIND_PACKAGE(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES})

# jsonpp11 - JSON wrapper
include(ExternalProject)

ExternalProject_Add(jsonpp11
    PREFIX 3rd_party
    GIT_REPOSITORY https://github.com/matiu2/jsonpp11
    CMAKE_CACHE_ARGS "-DCMAKE_CXX_FLAGS:string=-fPIC -std=c++1z -stdlib=libc++" 
                     "-DCMAKE_INSTALL_PREFIX:path=${CMAKE_CURRENT_BINARY_DIR}"
                     "-DCMAKE_BUILD_TYPE:string=RELEASE"
                     "-DBUILD_TESTS:boolean=OFF"
    TLS_VERIFY true
    GIT_TAG jsonpp14
    TLS_CAINFO certs/DigiCertHighAssuranceEVRootCA.crt
    TEST_BEFORE_INSTALL 0
    BUILD_COMMAND make
    INSTALL_COMMAND make install
    UPDATE_COMMAND ""
)
SET(JSON_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${JSON_INCLUDE_DIR})
find_library(JSON_LIB json
             PATHS ${CMAKE_CURRENT_BINARY_DIR}/3rd_party/src/json_spirit-build
             NO_DEFAULT_PATH)
