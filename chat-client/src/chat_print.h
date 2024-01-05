#pragma once
#include <iostream>
#include <mutex>
#include <string>

class ChatPrint
{
public:
	static std::mutex s_Mutex;
	static void Print(std::string& _log)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);
		std::cout << _log << std::endl;
	}

	static void Print(std::string&& _log)
	{
		std::lock_guard<std::mutex> guard(s_Mutex);
		std::cout << _log << std::endl;
	}
};