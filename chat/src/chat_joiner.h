#pragma once

#include <memory>
#include "chat_message.h"

class ChatJoiner
{
public:
	virtual ~ChatJoiner() {}
	virtual void Close() = 0;
	virtual void Deliver(const ChatMessage& _message) = 0;
};

typedef std::shared_ptr<ChatJoiner> ChatJoinerPtr;