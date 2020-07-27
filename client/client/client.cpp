#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define _DLL_EXPORTS
#include "../socket/data_socket.h"
#include "client.h"
#include "utils.h"
#include "../data_types/dir.h"
#include "../data_types/path_info.h"

using namespace std;

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
  stringstream dir_info;
	dir_info << this->data_socket_.GetResponse();
  // 返回的格式类似于cmd dir指令（Windows系统）
  Dir dir;
  string line;
  stringstream line_resovler;
  while (!dir_info.eof()) {
    getline(dir_info, line);  // 获取每一行
    if (!dir_info.eof()) {    // 舍弃最后一空白行
      line_resovler << line;
			PathInfo pi(line_resovler);
      dir.Push(pi);
    }
  }
  CloseDataSocket;
  return dir.GetFilesName();
}

unsigned int Client::GetFileSize(const std::string& filename) {
  const string file_size_message = "SIZE " + filename + CRLF;
  this->control_socket_.SendData(file_size_message);
  stringstream file_size_info;
	file_size_info << ReceiveMessage();
	int numbers[2];
	file_size_info >> numbers[0] >> numbers[1];
	return numbers[1];
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
	file << this->data_socket_.GetResponse();
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
    this->data_socket_.Send(string(send_buffer));
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
  this->data_socket_ = DataSocket(this->ip_address_, target_port);
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
