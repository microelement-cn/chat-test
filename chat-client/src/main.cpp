
#include <cstdlib>
#include <memory>
#include <thread>
#include <utility>
#include <iostream>
#include <boost/asio.hpp>
#include "chat_client.h"
#include "chat_message.h"

using namespace std;
using boost::asio::ip::tcp;


int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std:cerr << "Usage: chat_client <host> <port> \n";
		return 1;
	}

	boost::asio::io_context io_context;
	tcp::resolver resolver(io_context);
	auto endpoints = resolver.resolve(argv[1], argv[2]);
	ChatClient c(io_context, endpoints);

	std::thread t([&io_context]() { io_context.run(); });

	char line[ChatMessage::MAX_BODY_SIZE + 1];
	while (std::cin.getline(line, ChatMessage::MAX_BODY_SIZE + 1))
	{
		//std::cout << line << std::endl;
		ChatMessage msg;
		msg.BodySize(std::strlen(line));
		std::memcpy(msg.Body(), line, msg.BodySize());
		msg.EncodeHead();
		c.Write(msg);
	}

	c.Close();
	t.join();
	std::cout << "hello world" << std::endl;
	return 0;
}
