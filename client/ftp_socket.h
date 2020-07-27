#include <winsock2.h>
#include <string>

#pragma once

class FTPSocket {
 private:
  SOCKET socket_;
  sockaddr_in socaddr_;

 public:
	FTPSocket() {};
  FTPSocket(const std::string ip_address, unsigned int port);
  ~FTPSocket();

  void SendData(const std::string send_data);
  int ReceiveData(char* receive_buffer, unsigned int buffer_size);
  void Close();
};
