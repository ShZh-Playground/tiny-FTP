#include <iostream>
#include <vector>
#include "control_socket.h"

using namespace Socket;

ControlSocket::ControlSocket(const FTPSocket& ftp_socket) {
	this->socket_ = ftp_socket;
}

ControlSocket::ControlSocket(const std::string& ip_address, unsigned int port) {
	this->socket_ = FTPSocket(ip_address, port);
}

DataSocket ControlSocket::GetDataSocket(const std::string& ip_address, const std::string& data_socket_info) { 
	unsigned int target_port = ResolveDataSocketPort(data_socket_info);
	std::cout << "Data socket sending port: " << target_port << std::endl;
  return DataSocket(ip_address, target_port);
}

void ControlSocket::Send(const std::string& data) {
  this->socket_.SendData(data.c_str(), data.size());
}

const std::string ControlSocket::GetResponse() {
  int length;
  char buffer[this->socket_.kBufferSize] = {0};

  if ((length = this->socket_.ReceiveData(buffer, this->socket_.kBufferSize)) == -1) {
		std::cout << "Receive no data !" << std::endl;
    exit(1);
  }
	this->response_ = buffer;
	SetStatusCode();
  return buffer;
}

unsigned int ControlSocket::GetStatus() { return this->status_code_; }

void ControlSocket::Close() { this->socket_.Close(); }

void ControlSocket::SetStatusCode() { 
	if (this->response_ != "") {
		this->status_code_ = atoi(this->response_.substr(0, 3).c_str());
	}
}

unsigned int ControlSocket::ResolveDataSocketPort(
    const std::string& data_socket_info) {
  // find content in double parentheses
  auto left_parentheses = data_socket_info.find("(");
  auto right_parentheses = data_socket_info.rfind(")");
  if (right_parentheses <= left_parentheses) {
		std::cout << "Cannot resolve the command that returned!" << std::endl;
    exit(-1);
  }
  auto socket_length = right_parentheses - left_parentheses - 1;
  const std::string target_socket =
      data_socket_info.substr(left_parentheses + 1, socket_length);
  int socket_info[6];
	// Using sscanf to get the numbers in formatted string
  sscanf_s(target_socket.c_str(), "%d,%d,%d,%d,%d,%d", &socket_info[0],
           &socket_info[1], &socket_info[2], &socket_info[3], &socket_info[4],
           &socket_info[5]);
  // New port = 2nd_2_last_number * 256 + last_number
  unsigned int target_port = socket_info[4] * 256 + socket_info[5];

  return target_port;
}

