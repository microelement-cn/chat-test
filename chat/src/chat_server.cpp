#include <atomic>
#include <sstream>
#include <iostream>
#include "chat_print.h"
#include "chat_server.h"
#include "chat_common.h"
#include "chat_session_pool.h"

using namespace std;
ChatServer& ChatServer::Instance()
{
	static ChatServer instance(tcp::endpoint(tcp::v4(), 1979));
	return instance;
}

ChatServer::ChatServer(const tcp::endpoint& _endpoint)
	: m_ThreadNum(0)
	, m_Acceptor(m_ListenContext, _endpoint)
{
}

void ChatServer::Init(uint32_t _work_thread_num)
{
	if (_work_thread_num == 0)
		return;
		
	m_ThreadNum = _work_thread_num;
	if (_work_thread_num <= 0)
		exit(0);

	for (std::size_t i = 0; i < _work_thread_num; ++i)
	{
		boost::asio::io_context* iocontext = new boost::asio::io_context();
		m_IOContexts.push_back(UIOContextPtr(iocontext));
		m_ChatWorkers.emplace_back(i, *iocontext);
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

void ChatServer::WriteUser(QueueItem _item)
{
	if (m_ChatWorkers.empty())
		return;
	
	uint32_t threads_index = ChatSessionPool::Instance().GainThreadIndex(_item.session_index);
	if (threads_index == const_max_uint32_val)
		return;

	GetChatWorker(threads_index).WriteItem(_item);
}

void ChatServer::BroadCase(QueueItem _item)
{
	for (size_t i = 0; i < m_ChatWorkers.size(); i++)
	{
		m_ChatWorkers[i].PostBroadCase(_item);
	}
}

void ChatServer::RemoveSocket(uint32_t _session_index)
{
	m_ListenContext.post([this, _session_index]() {
			ChatSessionPool::Instance().BackIdleSession(_session_index);
		});
}

void ChatServer::StartAcceptor()
{
	uint32_t session_index = ChatSessionPool::Instance().GainIdleSession();
	uint32_t threads_index = ChatSessionPool::Instance().GainThreadIndex(session_index);

	UIOContextPtr& io_context = GetIOContext(threads_index);
	SPSocket shared_socket = std::make_shared<tcp::socket>(*io_context);

	AcceptParam param(session_index, threads_index, shared_socket, io_context);
	m_Acceptor.async_accept(*shared_socket,
		[this, param](const boost::system::error_code& error)
		{
			if (!error)
			{
				ChatWorker& worker = GetChatWorker(param.threads_index);
				param.io_context->post([&worker, param](){
					worker.AddSocket(param.session_index, param.shared_socket);
					});

				stringstream ss;
				ss << "Accepted connection from: " << param.shared_socket->remote_endpoint() << endl;
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


