#pragma once
#include <map>
#include <memory>
#include <stdlib.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

typedef std::map<uint32_t, uint32_t> MapSession;
typedef std::shared_ptr<tcp::socket> SPSocket;
typedef std::unique_ptr<boost::asio::io_context> UIOContextPtr;

constexpr uint32_t MAX_READ_CACHE_SIZE = 524288;
struct AcceptParam
{
	AcceptParam(uint32_t _session_index, 
                uint32_t _threads_index,
                SPSocket _shared_socket,
                UIOContextPtr& _io_context)
	: session_index(_session_index)
	, threads_index(_threads_index)
	, shared_socket(_shared_socket)
	, io_context(_io_context)
	{}
	uint32_t session_index;
	uint32_t threads_index;
	SPSocket shared_socket;
	UIOContextPtr& io_context;
};
