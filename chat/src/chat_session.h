#pragma once

#include <queue>
#include <utility>
#include <boost/asio.hpp>
#include "chat_joiner.h"
#include "chat_room.h"

using boost::asio::ip::tcp;
class ChatSession
	: public ChatJoiner
	, std::enable_shared_from_this<ChatSession>
{
public:
	ChatSession();
	void Start();
	void Close();
	void Deliver(const ChatMessage& _message);

private:
	void DoReadHead();
	void DoReadBody();
	void DoWrite();

	std::shared_ptr<tcp::socket> m_Socket;
	ChatMessage m_ReadMsg;
	std::queue<ChatMessage> m_WriteMsgs;
};