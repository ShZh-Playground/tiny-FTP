#pragma once

#include <string>
#include "data_socket.h"
#include "ftp_socket.h"

namespace Socket {
	class ControlSocket {
	 private:
		FTPSocket socket_;
		std::string response_ = "";
		unsigned int status_code_ = 0;

		void SetStatusCode();
		unsigned int ResolveDataSocketPort(const std::string& data_socket_info);

	 public:
		ControlSocket() {}
		ControlSocket(const FTPSocket&);
		ControlSocket(const std::string& ip_address, unsigned int port);
		~ControlSocket() {}

		DataSocket GetDataSocket(const std::string& ip_address,
														 const std::string& port_info);

		void Send(const std::string& data);
		const std::string GetResponse();
		unsigned int GetStatus();

		void Close();
	};
}
