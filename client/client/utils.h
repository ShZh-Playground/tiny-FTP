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

#define AssertFileExisted(file)                         \
do {                                            \
    if (!file) {                                \
      std::cout << "未能找到文件！" << std::endl; \
      exit(1);                                  \
    }                                           \
  } while (0)

