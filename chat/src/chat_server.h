#pragma once

#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chat_worker.h"
#include "chat_common.h"

using boost::asio::ip::tcp;

class ChatServer
{
public:
	static ChatServer& Instance();
	explicit ChatServer(const tcp::endpoint& _endpoint);
	void Init(uint32_t _thread_num);
	void Start();
	void Stop();
	void WriteUser(QueueItem _item);
	void BroadCase(QueueItem _item);
	void RemoveSocket(uint32_t _session_index);
private:
	void StartAcceptor();
	void StartWorkers();
	ChatWorker& GetChatWorker(int _idx);
	UIOContextPtr& GetIOContext(int _idx);
private:
	boost::asio::io_context m_ListenContext;
	std::size_t m_ThreadNum;
	tcp::acceptor m_Acceptor;
	std::vector<UIOContextPtr> m_IOContexts;
	boost::thread_group m_ThreadPool;
	std::vector<ChatWorker> m_ChatWorkers;
};
