#pragma once

#include <map>
#include <deque>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <boost/asio.hpp>

#include "chat_message.h"
#include "chat_session.h"

using boost::asio::ip::tcp;
class ChatRoom
{
public:
	static constexpr int MAX_RECENT_SIZE = 100;
	ChatRoom(boost::asio::io_context &_io_context) : m_MaxSize(10), m_IOContext(_io_context) {}
	void Join(ChatJoinerPtr _joiner);
	void Start();
	void Write(const std::string &_context);
	std::size_t GetNumClients() { return m_Clients.size(); }
private:
	void StartRead(tcp::socket &_socket);
	void RemoveClient(ChatJoinerPtr _joiner);
	void DoWrite();
	void Deliver(const std::string &_message);
	void DoReadHead();
	void DoReadBody();
private:
	std::size_t m_MaxSize;
	boost::asio::io_context &m_IOContext;
	std::vector<ChatJoinerPtr> m_Clients;
	std::deque<ChatMessage> m_WriteBuf;
	std::deque<ChatMessage> m_Messages;
	std::map<tcp::socket*, ChatMessage> m_ReadBuf;
};
