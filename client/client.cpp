#include "stdafx.h"

#include <string>
#include <iostream>
#define _DLL_EXPORTS
#include "client.h"

const unsigned int kBufferSize = 1024;

Client::Client(const std::string ip_address, unsigned int port=21) {
  this->control_socket_ = FTPSocket(ip_address, port);
  std::cout << receive_message() << std::endl;
}

Client::~Client() {}

std::string Client::receive_message() {
  int length;
	char buffer[kBufferSize] = {0};

	if ((length = this->control_socket_.receive_data(buffer, kBufferSize)) == -1) {
		std::cout << "没有接收到数据" << std::endl;
	}
	return buffer;
}

extern "C" DLL_API IClient* GetClient(const std::string ip_address) {
	return new Client(ip_address);
}
