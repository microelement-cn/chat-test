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
	QueueItem(int _socket_idx, std::shared_ptr<ChatMessage> _message)
	: socket_idx(_socket_idx)
	, message(_message)
	{}

	QueueItem(int _socket_idx, std::shared_ptr<ChatMessage>&& _message)
	: socket_idx(_socket_idx)
	, message(std::move(_message))
	{}
	
	QueueItem(const QueueItem& _other)
	: socket_idx(_other.socket_idx)
	, message(_other.message)
	{}

	QueueItem(QueueItem&& _other) noexcept
	: socket_idx(_other.socket_idx)
	, message(std::move(_other.message)) 
	{
		_other.socket_idx = -1;
	}

	QueueItem & operator=(const QueueItem& _other)
	{
		socket_idx = _other.socket_idx;
		message = _other.message;
		return *this;
	}

	QueueItem & operator=(QueueItem&& _other) noexcept
	{
		if (socket_idx == _other.socket_idx)
			return *this;
		
		socket_idx = _other.socket_idx;
		message = std::move(_other.message);

		_other.socket_idx = -1;
		return *this;
	}

	~QueueItem() {}
	
	int socket_idx;
	std::shared_ptr<ChatMessage> message;
};

typedef std::queue<QueueItem> MessageQueue;
