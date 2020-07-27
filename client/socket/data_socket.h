#pragma once

#include <string>
#include "ftp_socket.h"

class DataSocket {
 private:
  enum Mode { PASV, PORT_STRICT, PORT };

  FTPSocket socket_;

 public:
  DataSocket() {};
  DataSocket(const FTPSocket& ftp_socket);
  DataSocket(const std::string& ip_address, unsigned int port);
  ~DataSocket() {};

  std::string GetResponse();

	void Send(std::string send_data);
  unsigned int Receive(char* receive_buffer, unsigned int buffer_size);

	void Close();
};
