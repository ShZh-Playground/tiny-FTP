#include <winsock2.h>
#include <string>

#pragma once

namespace Socket {
	class FTPSocket {
	 private:
		SOCKET socket_;
		sockaddr_in socaddr_;

	 public:
		static const unsigned int kBufferSize = 1024;
		FTPSocket(){};
		FTPSocket(const std::string ip_address, unsigned int port);
		~FTPSocket();

		void SendData(const char* send_data, unsigned int length);
		int ReceiveData(char* receive_buffer, unsigned int buffer_size);
		void Close();
	};
}
