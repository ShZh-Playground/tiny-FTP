#include "stdafx.h"

#include <string>
#include <iostream>
#define _DLL_EXPORTS
#include "client.h"

const std::string CRLF = "\r\n";
const unsigned int kBufferSize = 1024;

Client::Client(const std::string& ip_address, unsigned int port=21) {
  this->control_socket_ = FTPSocket(ip_address, port);
  std::cout << ReceiveMessage() << std::endl;
}

Client::~Client() {}

void Client::Login(const std::string& username, const std::string& password) {
	// 先传输用户名
	const std::string username_message = "USER " + username + CRLF;
	this->control_socket_.SendData(username_message);
	std::cout << ReceiveMessage() << std::endl;
	// 再传输密码
	const std::string password_message = "PASS " + password + CRLF;
	this->control_socket_.SendData(password_message);
	std::cout << ReceiveMessage() << std::endl;
}

const std::string Client::ReceiveMessage() {
  int length;
	char buffer[kBufferSize] = {0};

	if ((length = this->control_socket_.ReceiveData(buffer, kBufferSize)) == -1) {
		std::cout << "没有接收到数据" << std::endl;
	}
	return buffer;
}

extern "C" DLL_API IClient* GetClient(const std::string ip_address) {
	return new Client(ip_address);
}
