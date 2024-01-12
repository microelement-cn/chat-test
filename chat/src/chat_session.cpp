#include <string>
#include <sstream>
#include <boost/asio.hpp>
#include "chat_print.h"
#include "chat_server.h"
#include "chat_session.h"
#include "chat_session_pool.h"

using boost::asio::ip::tcp;

ChatSession::ChatSession()
: m_SessionIndex(const_max_uint32_val)
, m_pSocket(nullptr)
{}

ChatSession::ChatSession(uint32_t _session_index, SPSocket _socket)
: m_SessionIndex(_session_index)
, m_pSocket(_socket)
{}

ChatSession::ChatSession(const ChatSession& _other)
: m_SessionIndex(_other.m_SessionIndex)
, m_pSocket(_other.m_pSocket)
, m_ReadBuf(_other.m_ReadBuf)
{}

ChatSession::ChatSession(ChatSession&& _other) noexcept
: m_SessionIndex(_other.m_SessionIndex)
, m_pSocket(std::move(_other.m_pSocket))
, m_ReadBuf(std::move(_other.m_ReadBuf))
{
	std::queue<ChatSession> emptyQueue;
	_other.m_pSocket = nullptr;
	_other.m_ReadBuf.Clear();
}

ChatSession& ChatSession::operator=(const ChatSession& _other)
{
	m_SessionIndex = _other.m_SessionIndex;
	m_pSocket = _other.m_pSocket;
	m_ReadBuf = _other.m_ReadBuf;
	return *this;
}

ChatSession& ChatSession::operator=(ChatSession&& _other) noexcept
{
	m_SessionIndex = _other.m_SessionIndex;
	m_pSocket = std::move(_other.m_pSocket);
	m_ReadBuf = std::move(_other.m_ReadBuf);

	_other.m_ReadBuf.Clear();
	_other.m_pSocket = nullptr;
	_other.m_SessionIndex = const_max_uint32_val;
	return *this;
}

ChatSession::~ChatSession()
{
	m_pSocket = nullptr;
}

void ChatSession::StartRead()
{
	DoReadHead();
}

void ChatSession::Close()
{
	m_pSocket->close();
	ChatSessionPool::Instance().BackIdleSession(m_SessionIndex);
}


void ChatSession::DoCallBackDeliver(QueueItem &_item)
{
	m_RefWQueue.push(_item);
	DoWrite();
}

void ChatSession::NoCallBackDeliver(QueueItem& _item)
{
	boost::asio::async_write(*m_pSocket, boost::asio::buffer(_item.message->Data(), _item.message->Size()),
		[this, _item](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
			{
				std::stringstream ss;
				ss << "Send success. =======" << _item.message->Data() << std::endl;
				ChatPrint::Print(ss.str());
			}
		});
}

void ChatSession::DoReadHead()
{
	boost::asio::async_read(*m_pSocket, boost::asio::buffer(m_ReadBuf.Data(), ChatMessage::MAX_HEAD_SIZE),
		[this](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error && m_ReadBuf.DecodeHead())
			{
				DoReadBody();
			}
			else if (_error == boost::asio::error::eof)
			{
				m_RefWQueue.swap(MessageQueue());
				ChatPrint::Print("Socket disconnected.");
				Close();
				ChatServer::Instance().RemoveSocket(m_SessionIndex);
			}
			else
			{
				m_RefWQueue.swap(MessageQueue());
				std::string log = "Error read head: " + _error.message();
				ChatPrint::Print(log);
				Close();
				ChatServer::Instance().RemoveSocket(m_SessionIndex);
			}
		});
}

void ChatSession::DoReadBody()
{
	boost::asio::async_read(*m_pSocket, boost::asio::buffer(m_ReadBuf.Body(), m_ReadBuf.BodySize()),
		[this](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
			{
				m_ReadBuf.Data()[m_ReadBuf.Size()] = 0;
				std::shared_ptr<ChatMessage> rItem = std::make_shared<ChatMessage>(m_ReadBuf);
				QueueItem item(m_SessionIndex, rItem);
				DoReadHead();
				ChatServer::Instance().BroadCase(item);
			}
			else
			{
				m_RefWQueue.swap(MessageQueue());
				std::string log = "Error read body: " + _error.message();
				ChatPrint::Print(log);
				Close();
				ChatServer::Instance().RemoveSocket(m_SessionIndex);
			}
		});
}

void ChatSession::DoWrite()
{
	auto self(shared_from_this());
	if (m_RefWQueue.empty())
		return;

	QueueItem item = m_RefWQueue.front();
	m_RefWQueue.pop();

	boost::asio::async_write(*m_pSocket, boost::asio::buffer(item.message->Data(), item.message->Size()),
		[this](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
			{
				DoWrite();
			}
			else
			{ 
				m_RefWQueue.swap(MessageQueue());
				Close();
				ChatServer::Instance().RemoveSocket(m_SessionIndex);
			}
		});
}

