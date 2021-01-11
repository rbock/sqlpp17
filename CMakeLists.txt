# Copyright (c) 2016, Roland Bock
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or
#    other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

###############################################################################
# Preamble
###############################################################################
cmake_minimum_required(VERSION 3.14)
project(sqlpp17 VERSION 0.1.0 LANGUAGES CXX)

###############################################################################
# Project Wide Setup
###############################################################################
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

option(SQLITE3_CONNECTOR "Build SQLite3 Connector" OFF)
option(POSTGRESQL_CONNECTOR "Build PostgreSQL Connector" OFF)
option(MYSQL_CONNECTOR "Build MySQL Connector" OFF)

option(USE_SQLCIPHER "Build SQLite3 Connector with SQLCipher instead" OFF)

###############################################################################
# Dependencies
###############################################################################
find_package(Threads REQUIRED)

if(SQLITE3_CONNECTOR)
  if(USE_SQLCIPHER)
    find_package(SQLCipher REQUIRED)
  else()
    find_package(SQLite3 REQUIRED)
  endif()
endif()

if(POSTGRESQL_CONNECTOR)
  find_package(PostgreSQL REQUIRED)
endif()

if(MYSQL_CONNECTOR)
  find_package(MySQL REQUIRED)
endif()

###############################################################################
# Main targets
###############################################################################
add_subdirectory(include/sqlpp17)

###############################################################################
# Tests
###############################################################################
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
