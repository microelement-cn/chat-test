#pragma once

#include <deque>
#include <boost/asio.hpp>
#include "chat_message.h"

using boost::asio::ip::tcp;

typedef std::deque<ChatMessage> ChatMessageQueue;

class ChatClient
{
public:
	ChatClient(boost::asio::io_context &io_context, const tcp::resolver::results_type& endpoints) 
		: m_IOContext(io_context)
		, m_Socket(io_context) 
		, m_connect(false)
		{
			DoConnect(endpoints);
		}
		
	~ChatClient() {}

	void Write(const ChatMessage &_msg);
	void Close();
	
private:
	void DoConnect(const tcp::resolver::results_type& _endpoints);
	void DoReadHead();
	void DoReadBody();
	void DoWrite();

private:
	boost::asio::io_context& m_IOContext;
	tcp::socket m_Socket;
	ChatMessage m_ReadMsg;
	ChatMessageQueue m_WriteMsgs;
	bool m_connect;
};