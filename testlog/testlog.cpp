// testlog.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../lib/spdlog/include/spdlog/sinks/basic_file_sink.h"

using namespace spdlog;

int main()
{
  auto logger = spdlog::basic_logger_mt("basic_logger",
                                     "basic_log.log");
	logger->info("Hello Wolrd");
}
