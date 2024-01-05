cmake_minimum_required(VERSION 2.9)

project(chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_INCLUDE_PATH "/usr/local/gcc920/include")
set(CMAKE_LIBRARY_PATH "/usr/local/gcc920/lib")

set(BOOST_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/boost")
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT_DIR}/include")
set(BOOST_LIBRARIES 
    "${BOOST_ROOT_DIR}/lib/libboost_regex.so.1.82.0"
    "${BOOST_ROOT_DIR}/lib/libboost_system.so.1.82.0"
    "${BOOST_ROOT_DIR}/lib/libboost_thread.so.1.82.0"
    "${BOOST_ROOT_DIR}/lib/libboost_context.so.1.82.0"
    "${BOOST_ROOT_DIR}/lib/libboost_date_time.so.1.82.0"
    "${BOOST_ROOT_DIR}/lib/libboost_serialization.so.1.82.0")

#set(FCGI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/fcgi")
#set(FCGI_INCLUDE_DIRS "${FCGI_ROOT_DIR}/include")
#set(FCGI_LIBRARIES "${FCGI_ROOT_DIR}/lib/libfcgi.so")
#set(LOG4CPP_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/log4j")
#set(LOG4CPP_INCLUDE_DIRS "${LOG4CPP_ROOT_DIR}/include")
#set(LOG4CPP_LIBRARIES "${LOG4CPP_ROOT_DIR}/lib/liblog4cpp.so")
#set(LIBCONFIG_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/libconfig")
#set(LIBCONFIG_INCLUDE_DIRS "${LIBCONFIG_ROOT_DIR}/include")
#set(LIBCONFIG_LIBRARIES "${LIBCONFIG_ROOT_DIR}/lib/libconfig++.so")
#set(LKHUB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(src)



