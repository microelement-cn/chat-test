#include "chat_message.h"

ChatMessage::ChatMessage(const ChatMessage& _other)
{
	std::memcpy(m_Data, _other.m_Data, MAX_HEAD_SIZE+MAX_BODY_SIZE);
	m_BodySize = _other.m_BodySize;
}

ChatMessage& ChatMessage::operator=(const ChatMessage& _other)
{
	std::memcpy(m_Data, _other.m_Data, MAX_HEAD_SIZE+MAX_BODY_SIZE);
	m_BodySize = _other.m_BodySize;
	return *this;
}

ChatMessage::ChatMessage(ChatMessage&& _other) noexcept
{
	std::memcpy(m_Data, _other.m_Data, MAX_HEAD_SIZE+MAX_BODY_SIZE);
	m_BodySize = _other.m_BodySize;
	_other.Clear();
}

ChatMessage& ChatMessage::operator=(ChatMessage&& _other) noexcept
{
	std::memcpy(m_Data, _other.m_Data, MAX_HEAD_SIZE+MAX_BODY_SIZE);
	m_BodySize = _other.m_BodySize;
	_other.Clear();
	return *this;
}

void ChatMessage::Clear()
{
	m_BodySize = 0;
	std::memset(m_Data, 0, sizeof(m_Data));
}

bool ChatMessage::DecodeHead()
{
	char header[MAX_HEAD_SIZE+1] = "";
	std::strncat(header, m_Data, MAX_HEAD_SIZE);
	m_BodySize = std::atoi(header);
	if (m_BodySize > MAX_BODY_SIZE)
	{
		m_BodySize = 0;
		return false;
	}

	return true;
}

void ChatMessage::EncodeHead()
{
	char header[MAX_HEAD_SIZE+1] = "";
	std::sprintf(header, "%4d", static_cast<int>(m_BodySize));
	std::memcpy(m_Data, header, MAX_HEAD_SIZE);
}

