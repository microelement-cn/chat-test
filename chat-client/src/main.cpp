
#include <cstdlib>
#include <memory>
#include <thread>
#include <vector>
#include <utility>
#include <iostream>
#include <boost/asio.hpp>
#include "chat_client.h"
#include "chat_message.h"

using namespace std;
using boost::asio::ip::tcp;

typedef std::shared_ptr<tcp::resolver> ResolverPtr;
typedef std::shared_ptr<ChatClient> ChatClientPtr;
int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std:cerr << "Usage: chat_client <host> <port> \n";
		return 1;
	}

	std::atomic_bool bRunning = true;
	constexpr int const_max_thread_num = 150;
	boost::asio::io_context io_context[const_max_thread_num];
	std::vector<ResolverPtr> resolver_ptrs;
	std::vector<ChatClientPtr> chatclient_ptrs;
	std::vector<std::thread> io_threads;
	std::vector<std::thread> in_threads;
	for (size_t i = 0; i < const_max_thread_num; i++)
	{
		resolver_ptrs.emplace_back(std::make_shared<tcp::resolver>(io_context[i]));
		auto endpoints = resolver_ptrs.back()->resolve(argv[1], argv[2]);
		chatclient_ptrs.emplace_back(std::make_shared<ChatClient>(io_context[i], endpoints));
		io_threads.emplace_back(std::thread([&io_context, i]() { io_context[i].run(); }));
	}

	for (size_t i = 0; i < const_max_thread_num; i++)
	{
		in_threads.emplace_back(
		std::thread([i, &chatclient_ptrs, &bRunning]() {
				for (size_t j=0; j<100000; ++j)
				{
					int r = rand();
					char line[ChatMessage::MAX_BODY_SIZE + 1] = { 0 };
					sprintf(line, "thread:%d, random:%d", i, r);
					ChatMessage msg;
					msg.BodySize(std::strlen(line));
					std::memcpy(msg.Body(), line, msg.BodySize());
					msg.EncodeHead();
					chatclient_ptrs[i]->Write(msg);
				}
			}));
	}
	char line[ChatMessage::MAX_BODY_SIZE + 1];
	while (std::cin.getline(line, ChatMessage::MAX_BODY_SIZE + 1))
	{
		if (line[0] == 'a')
			bRunning.store(false);
			break;
	}

	for (size_t i = 0; i < chatclient_ptrs.size(); i++)
	{
		chatclient_ptrs[i]->Close();
	}

	for (size_t i = 0; i < io_threads.size(); i++)
	{
		io_threads[i].join();
	}

	for (size_t i = 0; i < in_threads.size(); i++)
	{
		in_threads[i].join();
	}
	std::cout << "hello world" << std::endl;
	return 0;
}
