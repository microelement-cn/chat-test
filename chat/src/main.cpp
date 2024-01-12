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
	ChatServer::Instance().Init(thread_num);
	ChatServer::Instance().Start();
	ChatServer::Instance().Stop();
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

int signal_io_context_test(int argc, char** argv)
{
	boost::asio::io_context io_context;
	boost::asio::io_context::strand strand(io_context);

	auto handle1 = []() { std::cout << "handle 1: " << std::this_thread::get_id() << std::endl; };
	auto handle2 = []() { std::cout << "handle 2: " << std::this_thread::get_id() << std::endl; };

	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;

	for (size_t i=0; i<10; ++i)
	{
		strand.post(handle1);
		strand.post(handle2);
	}

	std::thread thread1([&io_context]() { io_context.run(); });
	std::thread thread2([&io_context]() { io_context.run(); });

	thread1.join();
	thread2.join();
	return 0;
}

int main(int argc, char** argv)
{
	return server_main(argc, argv);
}
