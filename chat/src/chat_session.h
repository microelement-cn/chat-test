#pragma once

#include <queue>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include "chat_common.h"
#include "chat_message.h"
#include "message_queue.h"

using boost::asio::ip::tcp;

constexpr int const_max_byte_num = 8;
constexpr int const_max_bits_64 = 64;
constexpr int const_max_bits_8 = 8;
constexpr uint32_t const_max_uint32_val = 0xffffffff;
constexpr uint64_t const_max_uint64_val = 0xffffffffffffffff;

class ChatServer;
class ChatSession : std::enable_shared_from_this<ChatSession>
{
public:
	ChatSession();
	ChatSession(uint32_t _session_index, SPSocket _socket);
	ChatSession(const ChatSession& _other);
	ChatSession(ChatSession&& _other) noexcept;
	ChatSession& operator=(const ChatSession& _other);
	ChatSession& operator=(ChatSession&& _other) noexcept;
	~ChatSession();
	void StartRead();
	void Close();
	void DoCallBackDeliver(QueueItem& _item);
	void NoCallBackDeliver(QueueItem& _item);
	void SetSessionIndex(uint32_t _session_index) { m_SessionIndex = _session_index; }
	uint32_t GetSessionIndex() const { return m_SessionIndex; }
	void SetSPSocket(SPSocket _sp_socket) { m_pSocket = _sp_socket; }
	SPSocket GetSPSocket() { return m_pSocket; }
private:
	void DoReadHead();
	void DoReadBody();
	void DoWrite();

	uint32_t m_SessionIndex;
	SPSocket m_pSocket;
	ChatMessage m_ReadBuf;
	MessageQueue m_RefWQueue;
};