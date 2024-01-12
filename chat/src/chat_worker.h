#pragma once

#include <map>
#include <memory>
#include <boost/asio.hpp>
#include "chat_common.h"
#include "message_queue.h"

using boost::asio::ip::tcp;

class ChatWorker
{
public:
	ChatWorker(uint32_t _thread_index, boost::asio::io_context& _io_contex);
	void AddSocket(uint32_t session_index, SPSocket _socket);
	void StartWrite(QueueItem _item);
	void WriteItem(QueueItem _item);
	void PostBroadCase(QueueItem _item);
	void BroadCase();
	boost::asio::io_context& GetWorkContext() const { return m_IOContext; }
private:
	void RemoveSocket(int _socket_idx);
private:
	uint32_t m_ThreadIndex;
	boost::asio::io_context& m_IOContext;
	boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_IOWorker;
	MessageQueue m_RefRQueue;
	MessageQueue m_RefWQueue;
	MapSession m_SessionIdxs;
	ChatMessage m_ReadBuf;
};