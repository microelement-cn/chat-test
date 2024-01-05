#pragma once

#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "chat_worker.h"

using boost::asio::ip::tcp;
typedef std::unique_ptr<boost::asio::io_context> UIOContextPtr;
class ChatServer
{
public:
	explicit ChatServer(const tcp::endpoint& _endpoint, std::size_t _work_thread_num);
	void Start();
	void Stop();
	void DoWrite(int _sid, QueueItem &_item);
private:
	void StartAcceptor();
	void StartWorkers();
	void UsableWorkerIdx(uint32_t &_socket_idx, uint32_t& _thread_idx);
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
