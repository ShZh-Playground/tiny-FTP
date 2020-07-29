#pragma once

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

// Normal settings
const string CRLF = "\r\n";
const unsigned int kBufferSize = 1024;

// Inline functions
#define PrintMessage                                               \
  do {                                                             \
    std::cout << this->control_socket_.GetResponse() << std::endl; \
  } while (0)

#define SendControlMessage(command)        \
  do {                                     \
    const string message = command + CRLF; \
    this->control_socket_.Send(message);   \
    PrintMessage;                          \
  } while (0)

#define CloseDataSocket         \
  do {                          \
    this->data_socket_.Close(); \
    PrintMessage;               \
  } while (0)

#define AssertFileExisted(file)                   \
  do {                                            \
    if (!file) {                                  \
      std::cout << "未能找到文件！" << std::endl; \
      exit(1);                                    \
    }                                             \
  } while (0)

#define DownloadFileByBuffer(file)	\
	do {\
		int length = 0;\
		char receive_buffer[kBufferSize] = {0};\
		while ((length = this->data_socket_.Receive(receive_buffer, kBufferSize)) !=0) {\
			file.write(receive_buffer, length);\
			cout << "Buffer receive: " << length << endl;\
		}\
	}while (0) 