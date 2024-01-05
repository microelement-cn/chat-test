#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>

class ChatMessage
{
public:
	static constexpr std::size_t MAX_HEAD_SIZE = 4;
	static constexpr std::size_t MAX_BODY_SIZE = 1024*512;

	ChatMessage() : m_BodySize(0) { std::memset(m_Data, 0, sizeof(m_Data)); }

	ChatMessage(const ChatMessage& _other);
	ChatMessage& operator=(const ChatMessage& _other);

	ChatMessage(ChatMessage&& _other) noexcept;
	ChatMessage& operator=(ChatMessage&& _other) noexcept;
	
	~ChatMessage() {}

	void Clear();
	const char * Data() const { return m_Data; }
	char * Data() { return m_Data; }
	const char * Body() const { return m_Data + MAX_HEAD_SIZE; }
	char * Body() { return m_Data + MAX_HEAD_SIZE; }
	std::size_t Size() { return MAX_HEAD_SIZE + m_BodySize; }
	std::size_t BodySize() { return m_BodySize; }
	bool DecodeHead();
	void EncodeHead();
private:
	char m_Data[MAX_HEAD_SIZE+MAX_BODY_SIZE];
	std::size_t m_BodySize;
};