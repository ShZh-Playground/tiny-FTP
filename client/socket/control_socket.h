#pragma once

#include <string>
#include "ftp_socket.h"
#include "data_socket.h"

class ControlSocket {
 private:
	 FTPSocket socket_;
	unsigned int ResolveDataSocketPort(const std::string& data_socket_info);

 public:
	 ControlSocket() {}
	 ControlSocket(const FTPSocket&);
	 ControlSocket(const std::string& ip_address, unsigned int port);
	 ~ControlSocket() {}

	 DataSocket GetDataSocket(const std::string& ip_address, const std::string& port_info);

	 void Send(const std::string& data);
   const std::string GetResponse();

	 void Close();
};