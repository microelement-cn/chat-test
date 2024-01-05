#include "chat_session.h"

ChatSession::ChatSession()
{

}

void ChatSession::Start()
{
	DoReadHead();
}

void ChatSession::Close()
{
	m_Socket->close();
}


void ChatSession::Deliver(const ChatMessage &_message)
{
	DoWrite();
}

void ChatSession::DoReadHead()
{
	auto self(shared_from_this());
	boost::asio::async_read(*m_Socket, boost::asio::buffer(m_ReadMsg.Data(), ChatMessage::MAX_HEAD_SIZE),
		[this, self](const boost::system::error_code& _error, std::size_t _bytes_transferred)
		{
			if (!_error && m_ReadMsg.DecodeHead())
			{
				DoReadBody();
			}
			else
			{
			}
		});
}

void ChatSession::DoReadBody()
{
	auto self(shared_from_this());
	boost::asio::async_read(*m_Socket, boost::asio::buffer(m_ReadMsg.Body(), m_ReadMsg.BodySize()),
		[this, self](const boost::system::error_code& _error, std::size_t _bytes_transferred)
		{
			if (!_error)
			{
				DoReadHead();
			}
			else
			{
			}
		});
}

void ChatSession::DoWrite()
{
	auto self(shared_from_this());
	boost::asio::async_write(*m_Socket, boost::asio::buffer(m_WriteMsgs.front().Data(), m_WriteMsgs.front().Size()),
		[this, self](const boost::system::error_code& _error, std::size_t _bytes_transferred)
		{
			if (!_error)
			{
				if (!m_WriteMsgs.empty())
					DoWrite();
			}
			else
			{
			}
		});
}

