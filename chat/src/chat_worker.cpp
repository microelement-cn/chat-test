#include <sstream>
#include <iostream>
#include "chat_print.h"
#include "chat_worker.h"
#include "chat_message.h"
#include "chat_session.h"
#include "chat_session_pool.h"

using namespace std;
ChatWorker::ChatWorker(uint32_t _thread_index, boost::asio::io_context& _io_contex)
: m_ThreadIndex(_thread_index)
, m_IOContext(_io_contex)
, m_IOWorker(_io_contex.get_executor())
{
}

void ChatWorker::AddSocket(uint32_t _session_index, SPSocket _socket)
{
	string log = "INFO AddSocket _session_index=" + std::to_string(_session_index);
	ChatPrint::Print(log);
	
	SPSESSION pSession = ChatSessionPool::Instance().GainSession(_session_index, _socket);
	if (nullptr == pSession)
		return;

	m_SessionIdxs[_session_index] = _session_index;
	pSession->StartRead();
}

void ChatWorker::StartWrite(QueueItem _item)
{
	uint32_t thread_index = ChatSessionPool::Instance().GainThreadIndex(_item.session_index);
	if (thread_index != m_ThreadIndex)
		return;
	
	SPSESSION spSession = ChatSessionPool::Instance().GetSession(_item.session_index);
	if (nullptr == spSession)
		return;
	
	spSession->DoCallBackDeliver(_item);
}

void ChatWorker::WriteItem(QueueItem _item)
{
	m_IOContext.post([this, _item]() {
			StartWrite(_item);
		});
}

void ChatWorker::PostBroadCase(QueueItem _item)
{
	m_IOContext.post([this, _item]() {
		m_RefWQueue.push(_item);
		BroadCase();
		});
}

void ChatWorker::BroadCase()
{
	std::string log = "========= Pop -------   Write queue size: " + std::to_string(m_RefWQueue.size());
	ChatPrint::Print(log);
	while (!m_RefWQueue.empty())
	{
		auto item = m_RefWQueue.front();
		m_RefWQueue.pop();

		for (auto iter= m_SessionIdxs.begin(); iter!= m_SessionIdxs.end(); ++iter)
		{
			SPSESSION spSesssion = ChatSessionPool::Instance().GetSession(iter->first);
			if (nullptr == spSesssion)
				continue;

			spSesssion->NoCallBackDeliver(item);
		}
	}	

	log = "========= Pop +++++++   Write queue size: " + std::to_string(m_RefWQueue.size());
	ChatPrint::Print(log);
}

void ChatWorker::RemoveSocket(int _socket_idx)
{
}



