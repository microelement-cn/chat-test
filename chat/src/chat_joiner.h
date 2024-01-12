#pragma once

#include <memory>
#include "message_queue.h"

class ChatJoiner
{
public:
	virtual ~ChatJoiner() {}
	virtual void Close() = 0;
	virtual void Deliver(QueueItem& _message) = 0;
};

typedef std::shared_ptr<ChatJoiner> ChatJoinerPtr;