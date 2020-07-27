#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define _DLL_EXPORTS
#include "client.h"

using namespace std;

const string CRLF = "\r\n";
const unsigned int kBufferSize = 1024;

#define PrintMessage                            \
  do {                                          \
    std::cout << ReceiveMessage() << std::endl; \
  } while (0)

#define SendControlMessage(command)          \
  do {                                       \
    const string message = command + CRLF;   \
    this->control_socket_.SendData(message); \
    PrintMessage;                            \
  } while (0)

#define CloseDataSocket         \
  do {                          \
    this->data_socket_.Close(); \
    PrintMessage;               \
  } while (0)

Client::Client(const string& ip_address, unsigned int port = 21)
    : ip_address_(ip_address), control_socket_(FTPSocket(ip_address, port)) {
  PrintMessage;
}

Client::~Client() {
  // 给服务器发送QUIT并关闭socket
  SendControlMessage("QUIT");
  this->control_socket_.Close();
}

const string Client::ReceiveMessage() {
  int length;
  char buffer[kBufferSize] = {0};

  if ((length = this->control_socket_.ReceiveData(buffer, kBufferSize)) == -1) {
    cout << "没有接收到数据" << endl;
  }
  return buffer;
}

void Client::Login(const string& username, const string& password) {
  // 先传输用户名，再传密码（这里有先后顺序）
  SendControlMessage("USER " + username);
  SendControlMessage("PASS " + password);
}

vector<string> Client::GetDirList() {
  EnterPassiveMode();
  SendControlMessage("LIST");
  // 接受返回来的data_socket返回的所有输出
  int length;
  stringstream dir_info;
  char receive_buffer[kBufferSize] = {0};
  while ((length = this->data_socket_.ReceiveData(receive_buffer,
                                                  kBufferSize)) != 0) {
    dir_info << string(receive_buffer, length);
  }
  // 返回的格式类似于cmd dir指令（Windows系统）
  string line;
  stringstream line_resovler;
  vector<string> file_property(4);
  vector<string> files_in_dir;
  // TODO: DirTypes(我不知道我自定义的格式C#能否使用)
  while (!dir_info.eof()) {
    getline(dir_info, line);  // 获取每一行
    if (!dir_info.eof()) {    // 舍弃最后一空白行
      line_resovler << line;
      line_resovler >> file_property[0] >> file_property[1] >>
          file_property[2] >> file_property[3];  // 每行用空格分隔四个信息
      files_in_dir.push_back(file_property[3]);  // 最后一个为文件/文件夹名
    }
  }
  CloseDataSocket;
  return files_in_dir;
}

unsigned int Client::GetFileSize(const std::string& filename) {
	SendControlMessage("SIZE " + filename);
}

void Client::DownloadFile(const string& filename) {
  EnterPassiveMode();  // 先进入被动模式
  SendControlMessage("RETR " + filename);
  // 创建文件
  auto file = ofstream(filename, ios::out || ios::binary);
  if (!file) {
    cout << "未能打开文件!" << endl;
    exit(1);
  }
  // 通过接受流来完成下载
  int length = 0;
  char receive_buffer[kBufferSize] = {0};
  while ((length = this->data_socket_.ReceiveData(receive_buffer,
                                                  kBufferSize)) != 0) {
    for (int i = 0; i < length; ++i) {
      file << receive_buffer[i];
    }
  }
  file.close();
  CloseDataSocket;
}

void Client::UploadFile(const string& filename) {
  EnterPassiveMode();  // 先进入被动模式
  SendControlMessage("STOR " + filename);
  // 打开文件
  auto file = ifstream(filename, ios::in || ios::binary);
  if (!file) {
    cout << "未能找到文件！" << endl;
    exit(1);
  }
  // 通过读取文件内容并发送来完成上传
  char send_buffer[kBufferSize] = {0};
  while (!file.eof()) {
    file.read(send_buffer, kBufferSize);
    this->data_socket_.SendData(string(send_buffer));
  }
  file.close();
  CloseDataSocket;
}

//////////////////////////////////////////PrivateMethod/////////////////////////////////////////////

void Client::EnterPassiveMode() {
  // 向服务器传输进入被动模式的指令
  const string passive_mode_message = "PASV" + CRLF;
  this->control_socket_.SendData(passive_mode_message);
  // 接受服务器信息
  const string data_socket_info = ReceiveMessage();
  cout << data_socket_info << endl;
  // 计算端口号并创建数字套接字
  unsigned int target_port = ResolveDataSocketPort(data_socket_info);
  cout << target_port << endl;
  this->data_socket_ = FTPSocket(this->ip_address_, target_port);
}

unsigned int Client::ResolveDataSocketPort(const string& data_socket_info) {
  // 找到()中的内容
  auto left_parentheses = data_socket_info.find("(");
  auto right_parentheses = data_socket_info.rfind(")");
  if (right_parentheses <= left_parentheses) {
    cout << "未能正确解析返回的命令" << endl;
    exit(-1);
  }
  auto socket_length = right_parentheses - left_parentheses - 1;
  const string target_socket =
      data_socket_info.substr(left_parentheses + 1, socket_length);
  int socket_info[6];
  // 利用sscanf直接提取格式化字符串中的数字内容
  sscanf_s(target_socket.c_str(), "%d,%d,%d,%d,%d,%d", &socket_info[0],
           &socket_info[1], &socket_info[2], &socket_info[3], &socket_info[4],
           &socket_info[5]);
  // 新端口号计算公式：倒数第二个数 X 256 + 最后一个数
  unsigned int target_port = socket_info[4] * 256 + socket_info[5];

  return target_port;
}

extern "C" DLL_API IClient* GetClient(const string ip_address) {
  return new Client(ip_address);
}
