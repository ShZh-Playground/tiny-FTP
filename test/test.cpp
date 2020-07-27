// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include "pch.h"
#include "../client/client.h"
#include <iostream>

#pragma comment(lib, "client.lib")

int main()
{
	IClient* client = GetClient("192.168.176.1");
	client->Login("shzh", "123456");
	delete client;
	return 0;
}
