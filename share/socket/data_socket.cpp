#include "data_socket.h"

using namespace Socket;

DataSocket::DataSocket(const std::string& ip_address, unsigned int port)
    : socket_(FTPSocket(ip_address, port)), port_(port) {}

DataSocket::DataSocket(const FTPSocket& ftp_socket) : socket_(ftp_socket) {}

std::string DataSocket::GetResponse() {
  std::string result;
  int length = 0;

  char receive_buffer[this->socket_.kBufferSize] = {0};
  while ((length = this->socket_.ReceiveData(receive_buffer, this->socket_.kBufferSize)) != 0) {
    result += std::string(receive_buffer, length);
  }
  return result;
}

void DataSocket::Send(const char* send_data, unsigned int length) {
  this->socket_.SendData(send_data, length);
}

unsigned int DataSocket::Receive(char* receive_buffer,
                                 unsigned int buffer_size) {
  return this->socket_.ReceiveData(receive_buffer, buffer_size);
}

unsigned int DataSocket::GetPort() { return this->port_; }

void DataSocket::Close() { this->socket_.Close(); }
