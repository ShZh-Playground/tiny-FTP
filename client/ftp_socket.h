#include <string>
#include <winsock2.h>

#pragma once
class FTPSocket {
 private:
  SOCKET socket_;
  sockaddr_in socaddr_;

 public:
  FTPSocket(const std::string ip_address, unsigned int port);
  ~FTPSocket();

	void send_data(const std::string send_data);
	int receive_data(char* receive_buffer, unsigned int buffer_size);
	void close();
};
