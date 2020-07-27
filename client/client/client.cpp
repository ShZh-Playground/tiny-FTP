#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#define _DLL_EXPORTS
#include "client.h"
#include "utils.h"
#include "../socket/data_socket.h"
#include "../socket/control_socket.h"
#include "../data_types/dir.h"
#include "../data_types/path_info.h"

using namespace std;

Client::Client(const string& ip_address, unsigned int port = 21)
    : ip_address_(ip_address), control_socket_(ControlSocket(ip_address, port)) {
  PrintMessage;
}

Client::~Client() {
  // 给服务器发送QUIT并关闭socket
  SendControlMessage("QUIT");
  this->control_socket_.Close();
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
  this->control_socket_.Send(file_size_message);
  stringstream file_size_info;
	file_size_info << this->control_socket_.GetResponse();
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

void Client::EnterPassiveMode() {
  // 向服务器传输进入被动模式的指令
  const string passive_mode_message = "PASV" + CRLF;
  this->control_socket_.Send(passive_mode_message);
  // 接受服务器信息
  const string data_socket_info = this->control_socket_.GetResponse();
  cout << data_socket_info << endl;
  // 计算端口号并创建数字套接字
	this->data_socket_ = this->control_socket_.GetDataSocket(this->ip_address_, data_socket_info);
}

extern "C" DLL_API IClient* GetClient(const string ip_address) {
  return new Client(ip_address);
}
