######## #chat# ########

#include_directories("/usr/local/gcc920/include/c++/9.2.0")
#link_directories("/usr/local/gcc920/lib64")

set(CHAT_INCLUDE_FILES 
	chat_print.h
	chat_message.h
	message_queue.h
	chat_worker.h
    chat_server.h)

source_group("src" FILES ${CHAT_INCLUDE_FILES})

set(CHAT_SOURCE_FILES 
	chat_print.cpp
	chat_message.cpp
	chat_worker.cpp
	chat_server.cpp
    main.cpp)

source_group("src" FILES ${CHAT_SOURCE_FILES})
include_directories( /usr/local/include/c++/9.2.0
                    ${BOOST_INCLUDE_DIRS} )

set(PTHREAD_LIBRARIES pthread)
add_executable(chat ${CHAT_SOURCE_FILES})
set_property(TARGET chat PROPERTY FOLDER "src")
target_link_libraries(chat ${BOOST_LIBRARIES} ${PTHREAD_LIBRARIES}) 
