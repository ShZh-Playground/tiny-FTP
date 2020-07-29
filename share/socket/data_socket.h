#pragma once

#include <string>
#include "ftp_socket.h"

namespace Socket {
	class DataSocket {
	 private:
		enum Mode { PASV, PORT_STRICT, PORT };

		FTPSocket socket_;
		unsigned int port_;

	 public:
		DataSocket(){};
		DataSocket(const FTPSocket& ftp_socket);
		DataSocket(const std::string& ip_address, unsigned int port);
		~DataSocket(){};

		std::string GetResponse();

		void Send(const char* send_data, unsigned int length);

		unsigned int Receive(char* receive_buffer, unsigned int buffer_size);

		unsigned int GetPort();

		void Close();
	};
}


