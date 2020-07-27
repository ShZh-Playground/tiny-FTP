#include "data_socket.h"

DataSocket::DataSocket(const std::string& ip_address, unsigned int port)
    : socket_(FTPSocket(ip_address, port)) {}

DataSocket::DataSocket(const FTPSocket& ftp_socket) : socket_(ftp_socket) {}

std::string DataSocket::GetResponse() {
  std::string result;
  int length = 0;

  char receive_buffer[kBufferSize] = {0};
  while ((length = this->socket_.ReceiveData(receive_buffer, kBufferSize)) !=
         0) {
    result += std::string(receive_buffer, length);
  }
  return result;
}

void DataSocket::Send(std::string send_data) { this->socket_.SendData(send_data); }

void DataSocket::Close() { this->socket_.Close(); }
