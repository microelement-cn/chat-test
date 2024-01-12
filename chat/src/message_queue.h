#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
#include "chat_message.h"

using boost::asio::ip::tcp;

class QueueItem
{
public:
	QueueItem(int _session_index, std::shared_ptr<ChatMessage> _message)
	: session_index(_session_index)
	, message(_message)
	{}

	QueueItem(int _socket_idx, std::shared_ptr<ChatMessage>&& _message)
	: session_index(_socket_idx)
	, message(std::move(_message))
	{}
	
	QueueItem(const QueueItem& _other)
	: session_index(_other.session_index)
	, message(_other.message)
	{}

	QueueItem(QueueItem&& _other) noexcept
	: session_index(_other.session_index)
	, message(std::move(_other.message)) 
	{
		_other.session_index = -1;
	}

	QueueItem & operator=(const QueueItem& _other)
	{
		session_index = _other.session_index;
		message = _other.message;
		return *this;
	}

	QueueItem & operator=(QueueItem&& _other) noexcept
	{
		if (session_index == _other.session_index)
			return *this;
		
		session_index = _other.session_index;
		message = std::move(_other.message);

		_other.session_index = -1;
		return *this;
	}

	~QueueItem() {}
	
	uint32_t session_index;
	std::shared_ptr<ChatMessage> message;
};

typedef std::queue<QueueItem> MessageQueue;
