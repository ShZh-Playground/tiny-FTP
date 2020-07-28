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

bool Client::Rename(const std::string& old_name, const std::string& new_name) {
	SendControlMessage("RNFR " + old_name);
	if (this->control_socket_.GetStatus() != 350) return false;
	SendControlMessage("RNTO " + new_name);
  return this->control_socket_.GetStatus() == 250;
}

bool Client::RemoveFile(const std::string& filename) {
	SendControlMessage("DELE " + filename);
	return this->control_socket_.GetStatus() == 250;
}

bool Client::RemoveDir(const std::string& dirname) {
	return true; }

bool Client::MakeDir(const std::string& dirname) {
	SendControlMessage("MKD " + dirname);
	return this->control_socket_.GetStatus() == 257;
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
  SendControlMessage("TYPE I");	// 二进制传输
  SendControlMessage("RETR " + filename);
  // 创建文件
  auto file = ofstream(filename, ios::out | ios::binary);
	AssertFileExisted(file);
	// 利用字节流下载文件
  int length = 0;
  char receive_buffer[kBufferSize] = {0};
  while ((length = this->data_socket_.Receive(receive_buffer, kBufferSize)) != 0) {
		file.write(receive_buffer, length);
		cout << "Buffer receive: " << length << endl;
  }
  file.close();
  CloseDataSocket;
}

void Client::UploadFile(const string& filename) {
  EnterPassiveMode();  // 先进入被动模式
  SendControlMessage("TYPE I");	// 二进制传输
  SendControlMessage("STOR " + filename);
  // 打开文件
  auto file = ifstream(filename, ios::in | ios::binary);
	AssertFileExisted(file);
  // 通过读取文件内容并发送来完成上传
  char send_buffer[kBufferSize] = {0};
  while (!file.eof()) {
    file.read(send_buffer, kBufferSize);
    this->data_socket_.Send(send_buffer, file.gcount());
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

void Client::GetListByMLSD() {
	EnterPassiveMode();
	SendControlMessage("MLSD /");
}

extern "C" DLL_API IClient* GetClient(const string ip_address) {
  return new Client(ip_address);
}
