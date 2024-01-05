#include <atomic>
#include <sstream>
#include <iostream>
#include "chat_print.h"
#include "chat_server.h"
#include "chat_session_pool.h"

using namespace std;
ChatServer::ChatServer(const tcp::endpoint& _endpoint, std::size_t _work_thread_num)
	: m_ThreadNum(_work_thread_num)
	, m_Acceptor(m_ListenContext, _endpoint)
{
	if (_work_thread_num <= 0)
		exit(0);
	
	for (std::size_t i = 0; i < _work_thread_num; ++i)
	{
		boost::asio::io_context *iocontext = new boost::asio::io_context();
		m_IOContexts.push_back(UIOContextPtr(iocontext));
		m_ChatWorkers.emplace_back(*iocontext);
	}
}

void ChatServer::Start()
{
	ChatPrint::Print("ChatServer Start.");
	StartAcceptor();
	StartWorkers();
}

void ChatServer::Stop()
{
	m_ListenContext.run();
	m_ThreadPool.join_all();
}

void ChatServer::DoWrite(int _sid,  QueueItem &_item)
{
	if (m_ChatWorkers.empty())
		return;
	
	GetChatWorker(_sid).WriteItem(_item);
}

void ChatServer::StartAcceptor()
{
	//uint32_t session_index = ChatSessionPool::Instance().GainIdleSession() ;
	//uint32_t threads_index = ChatSessionPool::Instance().GainThreadIndex(session_index);
	uint32_t session_index = const_max_uint32_val;
	uint32_t threads_index = const_max_uint32_val;
	UsableWorkerIdx(session_index, threads_index);

	UIOContextPtr& io_context = GetIOContext(threads_index);
	std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(*io_context);
	m_Acceptor.async_accept(*socket, 
		[this, session_index, threads_index, &io_context, socket](const boost::system::error_code& error)
		{
			if (!error)
			{
				ChatWorker& worker = GetChatWorker(threads_index);
				io_context->post([&worker, session_index, socket](){
					worker.AddSocket(session_index, socket);
					});

				stringstream ss;
				ss << "Accepted connection from: " << socket->remote_endpoint() << endl;
				ChatPrint::Print(ss.str());
			}
			else
			{
				stringstream ss;
				ss << "Error accept connection: " << error.message() << endl;
				ChatPrint::Print(ss.str());
			}

			StartAcceptor();
		});
}

void ChatServer::StartWorkers()
{
	for (size_t i=0; i<m_IOContexts.size(); ++i)
	{
		m_ThreadPool.create_thread([this, i](){m_IOContexts[i]->run();});
	}
}

void ChatServer::UsableWorkerIdx(uint32_t &_socket_idx, uint32_t& _thread_idx)
{
	static std::atomic<int> next_worker(0);
	_socket_idx = next_worker++;
	_thread_idx = _socket_idx % m_ThreadNum;
}

ChatWorker& ChatServer::GetChatWorker(int _idx)
{
	int i = _idx % m_ThreadNum;
	return m_ChatWorkers[i];
}

UIOContextPtr& ChatServer::GetIOContext(int _idx)
{
	int i = _idx % m_ThreadNum;
	return m_IOContexts[i];
}


