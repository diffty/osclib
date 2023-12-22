cmake_minimum_required(VERSION 3.8.0)
project(osclib VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(osclib src/main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_test (NAME outputTest COMMAND osclib)

set (passRegex "^test" "^All ok")
set (failRegex "Error" "Fail")

set_property (TEST outputTest
              PROPERTY PASS_REGULAR_EXPRESSION "${passRegex}")
set_property (TEST outputTest
              PROPERTY FAIL_REGULAR_EXPRESSION "${failRegex}")