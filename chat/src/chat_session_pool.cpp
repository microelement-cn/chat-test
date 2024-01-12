#include <iostream>
#include "chat_session_pool.h"

ChatSessionPool& ChatSessionPool::Instance()
{
	static ChatSessionPool instance;
	return instance;
}

bool ChatSessionPool::Init(uint32_t _factor_num, uint32_t _thread_num)
{
	if (_factor_num <= 0 || _thread_num <= 0)
		return false;
	
	m_FactorNum = _factor_num;
	m_ThreadNum = _thread_num;
	uint32_t unit_size = _factor_num * _thread_num;
	for (size_t i = 0; i < unit_size; i++)
	{
		m_PosHolds.emplace_back();
		m_PosHolds.back().clear();
	}

	uint32_t session_size = unit_size * const_max_bits_64;
	for (size_t i = 0; i < session_size; i++)
	{
		m_Sessions.emplace_back(nullptr);
	}
	return true;
}

/*
�߳���Ϊn= m_ThreadNum  ÿ���߳�ӵ��m=m_FactorNum��ռλ��
-��ʾ��λ  1��ʾռλ
  0   thread 0*n+0    |--1--11111111111111111111111111111111111111111111111111111111111|      
  1   thread 0*n+1    |1111111111111111111111111111111111111111111111111111111111111111|     
  .   thread 0*n+.    |1111111111111111111111111111111111111111111111111111111111111111|       0
n-1   thread 0*n+n-1  |1111111111111111111111111111111111111111111----11111111111111111|     
 =================================================================^ ѡ�и�λ��============      
                                          |                                                    |
										  |                                                    |
										  |                                                    |
 =======================================================================================
  0   thread 1*n+0    |--1-------------------------------------------------------------|     
  1   thread 1*n+1    |----------------------------------------------------------------|       m
  .   thread 1*n+.    |----------------------------------------------------------------|       
n-1   thread 1*n+n-1  |----------------------------------------------------------------|      
*/
uint32_t ChatSessionPool::GainIdleSession()
{
	uint32_t max_idle_num = 0;
	uint32_t thread_index = const_max_uint32_val;
	uint32_t session_index = const_max_uint32_val;
	uint32_t poshold_index = const_max_uint32_val;
	//std::lock_guard<std::mutex> guard(m_Lock);
	for (size_t i = 0; i < m_FactorNum; i++)
	{
		uint32_t thread_index = const_max_uint32_val;
		uint32_t unit_pos_off = const_max_uint32_val;
		for (size_t j = 0; j < m_ThreadNum; j++)
		{
			uint32_t poshold = i * m_ThreadNum + j;
			if (poshold >= m_PosHolds.size())
				break;

			uint32_t count = 0;
			uint32_t index = const_max_uint32_val;
			m_PosHolds[poshold].idle(count, index);
			if (count > max_idle_num)
			{
				max_idle_num = count;                   //ͬһ�����̵߳�ռλ���Ƚϣ��ҵ���������
				unit_pos_off = index;                   //ͬһ������е�ռλ����
				thread_index = j;
				poshold_index = poshold;
			}
		}

		if (poshold_index != const_max_uint32_val && unit_pos_off < const_max_bits_64)
		{
			m_PosHolds[poshold_index].full = m_PosHolds[poshold_index].full | (uint64_t(1) << unit_pos_off);
			session_index = poshold_index * const_max_bits_64 + unit_pos_off;
			//std::cout << " GainIdleSession[=] Thread index:" 
			//	      << thread_index << "  Session index:" 
			//	      << session_index << " PosHold index:" 
			//	      << poshold_index << " Unit Pos off:" 
			//	      << unit_pos_off << " PosHold value:"
			//	      << m_PosHolds[poshold_index].full  << std::endl;
			break;
		}
	}
	return session_index;

}

void ChatSessionPool::BackIdleSession(uint32_t _session_index)
{
	if (const_max_uint32_val == _session_index)
		return;

	uint32_t  unit_pos_off = const_max_uint32_val;
	uint32_t  poshold_index = const_max_uint32_val;
	//std::lock_guard<std::mutex> guard(m_Lock);
	for (size_t i = 0; i < m_FactorNum; i++)
	{
		uint32_t max_index = (i + 1) * m_ThreadNum * const_max_bits_64;
		if (_session_index < max_index)
		{
			poshold_index = _session_index / const_max_bits_64;
			unit_pos_off = _session_index - poshold_index * const_max_bits_64;
		}
	}

	uint64_t value = (uint64_t(1) << unit_pos_off) ^ const_max_uint64_val;
	m_PosHolds[poshold_index].full = m_PosHolds[poshold_index].full & value;
	//std::cout << " BackIdleSession[=]                 Session index:" 
	//	      << _session_index << " PosHold index:" 
	//	      << poshold_index << " Unit Pos off:" 
	//	      << unit_pos_off << " PosHold value:"
	//	      << m_PosHolds[poshold_index].full <<std::endl;
}

uint32_t ChatSessionPool::GainThreadIndex(uint32_t _session_index)
{
	if (const_max_uint32_val == _session_index)
		return const_max_uint32_val;

	uint32_t poshold_index = _session_index / const_max_bits_64;
	return poshold_index % m_ThreadNum;
}

SPSESSION ChatSessionPool::GainSession(uint32_t _session_index, SPSocket _socket)
{
	if (_session_index >= m_Sessions.size())
		return nullptr;

	if (m_Sessions[_session_index] == nullptr)
	{
		m_Sessions[_session_index] = std::make_shared<ChatSession>(_session_index, _socket);
	}

	return m_Sessions[_session_index];
}

SPSESSION ChatSessionPool::GetSession(uint32_t _session_index)
{
	if (_session_index >= m_Sessions.size())
		return nullptr;

	return m_Sessions[_session_index];
}