#pragma once

#include <string>
#include "ftp_socket.h"

class DataSocket {
 private:
  enum Mode { PASV, PORT_STRICT, PORT };

  static const unsigned int kBufferSize = 1024;
  FTPSocket socket_;

 public:
  DataSocket() {};
  DataSocket(const FTPSocket& ftp_socket);
  DataSocket(const std::string& ip_address, unsigned int port);
  ~DataSocket() {};

  std::string GetResponse();

	void Send (std::string send_data);

	void Close();
};
