#include <iostream>
#include <sstream>
#include "chat_print.h"
#include "chat_client.h"

using namespace std;
void ChatClient::Write(const ChatMessage &_msg)
{
	if (!m_connect)
		return;

	boost::asio::post(m_IOContext, 
		[this, _msg](){
			bool writing = !m_WriteMsgs.empty();
			m_WriteMsgs.push_back(_msg);
			if (!writing)
			{
				DoWrite();
			}
		});
}

void ChatClient::Close()
{
	boost::asio::post(m_IOContext, [this](){m_Socket.close();});
}

void ChatClient::DoConnect(const tcp::resolver::results_type& _endpoints)
{
	ChatPrint::Print("Doconnect boot");
	boost::asio::async_connect(m_Socket, _endpoints, 
		[this](boost::system::error_code _error, tcp::endpoint _ep)
		{
			if (!_error)
			{
				m_connect = true;
				stringstream ss;
				ss << "Connect success:" << _ep << endl;
				ChatPrint::Print(ss.str());
				DoReadHead();
			}
			else
			{
				m_connect = false;
				ChatPrint::Print("Connect failed");
			}
		});
}

void ChatClient::DoReadHead()
{
	boost::asio::async_read(m_Socket, boost::asio::buffer(m_ReadMsg.Data(), ChatMessage::MAX_HEAD_SIZE),
		[this](boost::system::error_code _error, std::size_t /*length*/)
		{
			if (!_error && m_ReadMsg.DecodeHead())
			{
				DoReadBody();
			}
			else
			{
				m_Socket.close();
				m_connect = false;
			}
		});
}

void ChatClient::DoReadBody()
{
	boost::asio::async_read(m_Socket, boost::asio::buffer(m_ReadMsg.Body(), m_ReadMsg.BodySize()),
		[this](boost::system::error_code _error, std::size_t /*lenght*/)
		{
			if (!_error)
			{
				std::cout.write(m_ReadMsg.Body(), m_ReadMsg.BodySize()); std::cout << "\n";
				DoReadHead();
			}
			else
			{
				m_Socket.close();
				m_connect = false;
			}
		});
}

void ChatClient::DoWrite()
{
	boost::asio::async_write(m_Socket, boost::asio::buffer(m_WriteMsgs.front().Data(), m_WriteMsgs.front().Size()), 
		[this](boost::system::error_code _error, std::size_t /*lenght*/)
		{
			if (!_error && m_connect)
			{
				m_WriteMsgs.pop_front();
				if (!m_WriteMsgs.empty())
				{
					DoWrite();
				}
			}
			else
			{
				m_Socket.close();
				m_connect = false;
			}
		});
}


