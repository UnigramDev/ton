cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

if (NOT OPENSSL_FOUND)
  find_package(OpenSSL REQUIRED)
endif()

add_executable(generate-random-id generate-random-id.cpp )
target_link_libraries(generate-random-id tl_api ton_crypto keys)

target_include_directories(generate-random-id PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..)

add_executable(json2tlo json2tlo.cpp )
target_link_libraries(json2tlo tl_api ton_crypto keys )
target_include_directories(json2tlo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/..)
