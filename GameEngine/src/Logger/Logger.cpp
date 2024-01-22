#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

std::vector<LogEntry> Logger::messages = std::vector<LogEntry>();

Logger::Logger()
{
}


Logger::~Logger()
{
}

std::string CurrentDateTimeToString()
{
	std::time_t t = std::time(nullptr);
	std::tm tm;
	localtime_s(&tm, &t);
	std::ostringstream oss;

	oss << std::put_time(&tm, "%d-%b-%Y %H:%M:%S");

	return oss.str();
}

void Logger::Log(const std::string& message)
{

	std::string oss = CurrentDateTimeToString();


	std::cout << "[LOG] : " << oss << " - " << message << std::endl;
	messages.push_back({
		LOG_INFO,
		oss
	});
}

void Logger::Err(const std::string& message)
{
	std::string oss = CurrentDateTimeToString();
	// 12-Oct-2020 09:34:10 - Message
	std::cerr << "[ERR] : " << oss << " - " << message << std::endl;
	messages.push_back({
		LOG_ERROR,
		oss
	});
}
