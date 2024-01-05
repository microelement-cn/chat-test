cmake_minimum_required(VERSION 2.9)
project(chat-client)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_BUILD_TYPE "Debug") 
set(CMAKE_INCLUDE_PATH "/usr/local/gcc920/include")
set(CMAKE_LIBRARY_PATH "/usr/local/gcc920/lib")

set(BOOST_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/boost") 
set(BOOST_INCLUDE_DIRS "${BOOST_ROOT_DIR}/include") 
set(BOOST_LIBRARIES 
    "${BOOST_ROOT_DIR}/lib/libboost_regex.so"
    "${BOOST_ROOT_DIR}/lib/libboost_system.so"
    "${BOOST_ROOT_DIR}/lib/libboost_thread.so"
    "${BOOST_ROOT_DIR}/lib/libboost_context.so"
    "${BOOST_ROOT_DIR}/lib/libboost_date_time.so"
    "${BOOST_ROOT_DIR}/lib/libboost_serialization.so")
    
add_subdirectory(src) 