#include <sstream>
#include <iostream>
#include "chat_print.h"
#include "chat_worker.h"
#include "chat_message.h"

using namespace std;
ChatWorker::ChatWorker(boost::asio::io_context& _io_contex)
: m_IOContext(_io_contex)
, m_IOWorker(_io_contex.get_executor())
{
}

void ChatWorker::AddSocket(int _socket_idx, std::shared_ptr<tcp::socket> _socket)
{
	string log = "INFO AddSocket _socket_idx=" + std::to_string(_socket_idx);
	ChatPrint::Print(log);
	
	auto iter = m_SocketPtrs.find(_socket_idx);
	if (iter != m_SocketPtrs.end())
		return;

	m_SocketPtrs.insert(std::make_pair(_socket_idx, _socket));
	StartRead(_socket_idx, *_socket);
}

void ChatWorker::StartRead(int _socket_idx, tcp::socket& _socket)
{
	DoReadHead(_socket_idx, _socket);
}

void ChatWorker::DoReadHead(int _socket_idx, tcp::socket& _socket)
{
	boost::asio::async_read(_socket, boost::asio::buffer(m_ReadBuf.Data(), ChatMessage::MAX_HEAD_SIZE),
		[this, _socket_idx, &_socket] (const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error && m_ReadBuf.DecodeHead())
			{
				DoReadBody(_socket_idx, _socket);
			}
			else if (_error == boost::asio::error::eof)
			{
				ChatPrint::Print("Socket disconnected.");
				RemoveSocket(_socket_idx);
			}
			else
			{
				string log = "Error read head: " + _error.message();
				ChatPrint::Print(log);
				RemoveSocket(_socket_idx);
			}
		});

}

void ChatWorker::DoReadBody(int _socket_idx, tcp::socket& _socket)
{
    boost::asio::async_read(_socket, boost::asio::buffer(m_ReadBuf.Body(), m_ReadBuf.BodySize()),
        [this, _socket_idx, &_socket](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
			{
				m_ReadBuf.Data()[m_ReadBuf.Size()] = 0;
				std::string log = "DoReadBody " + std::string(m_ReadBuf.Data());
				ChatPrint::Print(log);
				std::shared_ptr<ChatMessage> rItem = std::make_shared<ChatMessage>(m_ReadBuf);
				m_RefRQueue.emplace(_socket_idx, rItem);
				m_RefWQueue.emplace(_socket_idx, rItem);
				DoReadHead(_socket_idx, _socket);
				ChatPrint::Print("DoReadHead again");
				Broadcase();
				ChatPrint::Print("Broadcase again");
			}
			else
			{
				string log =  "Error read body: " + _error.message();
				ChatPrint::Print(log);
				RemoveSocket(_socket_idx);
			}
        });

}

void ChatWorker::StartWrite()
{
	if (m_RefWQueue.empty())
		return;
	
	auto item = m_RefWQueue.front();
	m_RefWQueue.pop();
	
	auto iter = m_SocketPtrs.find(item.socket_idx);
	if (iter == m_SocketPtrs.end())
		return;
	
	boost::asio::async_write(*(iter->second), boost::asio::buffer(item.message->Data(), item.message->Size()),
		[this](const boost::system::error_code& _error, std::size_t _bytes_transferred) {
			if (!_error)
				StartWrite();
		});
}

void ChatWorker::WriteItem(QueueItem& _item)
{
	m_IOContext.post([this, &_item]() {
			m_RefWQueue.push(_item);
			StartWrite();
		});
}

void ChatWorker::Broadcase()
{
	while (!m_RefWQueue.empty())
	{
		auto item = m_RefWQueue.front();
		m_RefWQueue.pop();

		for (auto iter=m_SocketPtrs.begin(); iter!=m_SocketPtrs.end(); ++iter)
		{
			boost::asio::async_write(*(iter->second), boost::asio::buffer(item.message->Data(), item.message->Size()),
				[this, item](const boost::system::error_code& _error, std::size_t _bytes_transferred){
					if (!_error)
					{
						stringstream ss;
						ss << "Send success. =======" << item.message->Data() <<endl;
						ChatPrint::Print(ss.str());
					}
				});
		}
	}	
}

void ChatWorker::RemoveSocket(int _socket_idx)
{
	auto iter = m_SocketPtrs.find(_socket_idx);
	if (iter == m_SocketPtrs.end())
		return;

	if (nullptr == iter->second)
	{
		m_SocketPtrs.erase(iter);
		return;
	}
	
	iter->second->close();
	m_SocketPtrs.erase(iter);
}



