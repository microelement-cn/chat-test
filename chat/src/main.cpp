#include <vector>
#include <iostream>
#include "chat_server.h"
#include "chat_session_pool.h"
using namespace std;

int server_main(int argc, char** argv)
{
	constexpr uint32_t factor_num = 8;
	constexpr uint32_t thread_num = 1;

	ChatSessionPool::Instance().Init(factor_num, thread_num);

	tcp::endpoint endPoint(tcp::v4(), 1979);
	ChatServer chatServer(endPoint, thread_num);
	chatServer.Start();
	chatServer.Stop();
	return 0;
}

int session_test(int argc, char** argv)
{
	ChatSessionPool::Instance().Init(2, 8);

	std::vector<uint32_t> session_array;
	for (size_t i = 0; i < 32; ++i)
	{
		uint32_t session_index = ChatSessionPool::Instance().GainIdleSession();
		uint32_t thread_index = ChatSessionPool::Instance().GainThreadIndex(session_index);
		session_array.push_back(session_index);
	}
	std::cout << "===================================================" << std::endl;
	for (size_t i = 0; i < session_array.size(); ++i)
	{
		ChatSessionPool::Instance().BackIdleSession(session_array[i]);
	}
	int a;
	std::cin >> a;
	return 0;
}

int main(int argc, char** argv)
{
	return server_main(argc, argv);
}
