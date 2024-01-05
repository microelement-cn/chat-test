#pragma once
#include <mutex>
#include <vector>
#include <memory>
#include <stdlib.h>
#include "chat_session.h"

constexpr int const_max_byte_num = 8;
constexpr int const_max_bits_64 = 64;
constexpr int const_max_bits_8 = 8;
constexpr uint32_t const_max_uint32_val = 0xffffffff;
constexpr uint64_t const_max_uint64_val = 0xffffffffffffffff;
typedef union POSMARK
{
	struct
	{
		uint8_t part[const_max_byte_num];
	};
	uint64_t full;

	void clear() { full = 0; }
	void idle(uint32_t& _count, uint32_t& _index)
	{
		_count = 0;
		for (int32_t i = const_max_byte_num - 1; i >= 0; --i)
		{
			for (int32_t j = const_max_bits_8 - 1; j >= 0; --j)
			{
				if ((part[i] >> j) & 0x01 ^ 0x01)
				{
					_index = i * const_max_bits_8 + j;
					++_count; 
				}
			}
		}
	}
} POSMARK;


typedef std::vector<POSMARK> POSHOLDS;
typedef std::vector<std::shared_ptr<ChatSession>> SESSIONS;
class ChatSessionPool
{
public:
	static ChatSessionPool& Instance();

	ChatSessionPool()
		: m_FactorNum(0)
		, m_ThreadNum(0)
	{}

	~ChatSessionPool() {}

	bool Init(uint32_t _factor_num, uint32_t _thread_num);
	uint32_t GainIdleSession();
	void BackIdleSession(uint32_t _session_index);
	uint32_t GainThreadIndex(uint32_t _session_index);
private:
	std::mutex  m_Lock;
	uint32_t	m_FactorNum;
	uint32_t    m_ThreadNum;
	POSHOLDS	m_PosHolds;
	SESSIONS	m_Sessions;
};
