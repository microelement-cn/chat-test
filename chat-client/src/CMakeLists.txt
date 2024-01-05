######## #chat-client# ########

set(CHAT_INCLUDE_FILES 
    chat_print.h 
    chat_message.h 
    chat_client.h)

source_group("src" FILES ${CHAT_INCLUDE_FILES})

set(CHAT_SOURCE_FILES 
    chat_print.cpp 
    chat_message.cpp 
    chat_client.cpp 
    main.cpp)
	
source_group("src" FILES ${CHAT_SOURCE_FILES}) 
include_directories( /usr/local/include/c++/9.2.0 ${BOOST_INCLUDE_DIRS} )

set(PTHREAD_LIBRARIES pthread) 
add_executable(chat-client ${CHAT_SOURCE_FILES})
set_property(TARGET chat-client PROPERTY FOLDER "src") 

target_link_libraries(chat-client ${BOOST_LIBRARIES} ${PTHREAD_LIBRARIES}) 