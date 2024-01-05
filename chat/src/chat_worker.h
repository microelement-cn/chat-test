#pragma once

#include <map>
#include <memory>
#include <boost/asio.hpp>
#include "message_queue.h"

using boost::asio::ip::tcp;

class ChatWorker
{
public:
	ChatWorker(boost::asio::io_context& _io_contex);
	void AddSocket(int _socket_idx, std::shared_ptr<tcp::socket> _socket);
	void StartRead(int _socket_idx, tcp::socket& _socket);
	void DoReadHead(int _socket_idx, tcp::socket& _socket);
	void DoReadBody(int _socket_idx, tcp::socket& _socket);
	void StartWrite();
	void WriteItem(QueueItem& _item);
	void Broadcase();
	boost::asio::io_context& GetWorkContext() const { return m_IOContext; }
private:
	void RemoveSocket(int _socket_idx);
private:
	boost::asio::io_context& m_IOContext;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_IOWorker;
	MessageQueue m_RefRQueue;
	MessageQueue m_RefWQueue;
	ChatMessage m_ReadBuf;
	std::map<int,std::shared_ptr<tcp::socket>> m_SocketPtrs;
};