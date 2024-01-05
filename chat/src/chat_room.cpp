#include "chat_room.h"
#include <algorithm>

using namespace std;

void ChatRoom::Join(ChatJoinerPtr _joiner)
{
	if (m_Clients.size() < m_MaxSize)
	{
		m_Clients.push_back(_joiner);
	}
	else
	{
		cout << "Chat room full, can not join!" << endl;
	}
}

void ChatRoom::Start()
{
	m_IOContext.run();
}

void ChatRoom::Write(const std::string &_message)
{
	boost::asio::post(m_IOContext, [this, _message]() {
			bool bEmpty = m_Messages.empty();
			m_Messages.push_back(_message);
			if (!bEmpty)
				DoWrite();
		});
}


void ChatRoom::StartRead(tcp::socket &_socket)
{
	DoReadHead();
}

void ChatRoom::RemoveClient(ChatJoinerPtr _joiner)
{
	_joiner.close();
	m_Clients.erase(std::remove(m_Clients.begin(), m_Clients.end(), _joiner), m_Clients.end());
}

void ChatRoom::DoWrite()
{
	tcp::socket& socket = m_Clients.front();
	boost::asio::async_write(socket, boost::asio::buffer(m_Messages.front()), 
		[this, &socket](const boost::system::error_code& _error, std::size_t _bytes_transferred){
			if (!_error)
			{
				m_Messages.pop_front();
				if (!m_Messages.empty())
				{
					DoWrite();
				}
			}
			else
			{
				cerr << "Error write from socket: " << _error.message() << endl;
				RemoveClient(socket);
			}
		});
}

void ChatRoom::Deliver(const std::string &_message)
{
	m_Messages.push_back(_message);
	while (m_Messages.size() > MAX_RECENT_SIZE)
	{
		m_Messages.pop_front();
	}

	for (auto client : m_Clients)
	{
		
	}
}

void ChatRoom::DoReadHead(tcp::socket &_socket)
{
	boost::asio::async_read(_socket, boost::asio::buffer(m_ReadBuf[&_socket].Data(), ChatMessage::MAX_HEAD_SIZE),
		[this, &_socket] (const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error && m_ReadBuf[&_socket].DecodeHead())
			{
				DoReadBody(_socket);
			}
			else if (_error == boost::asio::error::eof)
			{
				cout << "Socket disconnected." << endl;
				RemoveClient(_socket);
			}
			else
			{
				cerr << "Error read from socket: " << _error.message() << endl;
				RemoveClient(_socket);
			}
		});

}

void ChatRoom::DoReadBody(tcp::socket &_socket)
{
	boost::asio::async_read(_socket, boost::asio::buffer(m_ReadBuf[&_socket].Data(), m_ReadBuf[&_socket].BodySize()),
		[this, &_socket] (const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
			{
				std::string message(m_ReadBuf[&_socket].Body());
				m_ReadBuf[&_socket].Clear();
				cout << "Received message from socket: " << message << endl;
				//广播消息
				Deliver(message);
				StartRead(_socket);
			}
			else if (_error == boost::asio::error::eof)
			{
				cout << "Socket disconnected." << endl;
				RemoveClient(_socket);
			}
			else
			{
				cerr << "Error read from socket: " << _error.message() << endl;
				RemoveClient(_socket);
			}
		});

}


