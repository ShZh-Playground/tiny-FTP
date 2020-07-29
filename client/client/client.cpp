#include <filesystem>
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

vector<string> Client::GetDirList(const std::string& target_dir) {
  EnterPassiveMode();
  SendControlMessage("LIST " + target_dir);
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

bool Client::ChangeWorkingDir(const std::string& dirname) { 
	SendControlMessage("CWD " + dirname);
	return GetWorkingDir() == ""? false : control_socket_.GetStatus() == 250;
}

std::string Client::GetWorkingDir() {
	const string get_dir_message = "PWD" + CRLF;
	this->control_socket_.Send(get_dir_message);
	const string dir_info = this->control_socket_.GetResponse();
	// 双引号中间的内容就是WD
	auto first_quot = dir_info.find_first_of("\"");
	auto last_quot = dir_info.find_last_of("\"");
	if (last_quot <= first_quot) {
		cout << "没有找到Working Dirctory" << endl << endl;
		exit(1);
	}
	// 判断失败的情况
	return this->control_socket_.GetStatus()
            ? dir_info.substr(first_quot + 1, last_quot - first_quot - 1) : "";
}

int Client::GetFileSize(const std::string& filename) {
  const string file_size_message = "SIZE " + filename + CRLF;
  this->control_socket_.Send(file_size_message);
  stringstream file_size_info;
	file_size_info << this->control_socket_.GetResponse();
	if (this->control_socket_.GetStatus() == 550 || this->control_socket_.GetStatus() == 451) { return -1; }
	int numbers[2];
	file_size_info >> numbers[0] >> numbers[1];
	return numbers[1];
}

void Client::DownloadFile(const string& filename) {
  EnterPassiveMode();  // 先进入被动模式
  SendControlMessage("TYPE I");	// 二进制传输

	if (!experimental::filesystem::exists(filename)) {
    // 正常上传
    auto file = ofstream(filename, ios::out | ios::binary);
    AssertFileExisted(file);
    DownloadFileByBuffer(file);
    file.close();
	} else {
		// 断点续传
		DownloadFileWithCheckPoint(filename);
	}
  CloseDataSocket;
}

void Client::DownloadFileWithCheckPoint(const string& filename) {
  auto file = ofstream(filename, ios::out | ios::binary | ios::app);
  AssertFileExisted(file);
  // 查看文件大小顺便将文件指针移到末尾
  file.seekp(0, ios::end);
  auto local_file_size = file.tellp();
  auto server_file_size = GetFileSize(filename);
  // 开启断点重传
  if (local_file_size != server_file_size) {
    SendControlMessage("REST " + to_string(local_file_size));
    SendControlMessage("RETR " + filename);
    DownloadFileByBuffer(file);
    file.close();
  }
}

void Client::UploadFileWithCheckPoint(const std::string& filename,
                                      int server_file_size) {
  auto file = ifstream(filename, ios::out | ios::binary);
  AssertFileExisted(file);
  file.seekg(0, ios::end);
  auto local_file_size = file.tellg();
  if (local_file_size > server_file_size) {
    SendControlMessage("REST " + to_string(server_file_size));
    SendControlMessage("STOR " + filename);
    file.seekg(local_file_size);  // 移动指针
    UploadFileByBuffer(file);
    file.close();
  }
}

void Client::UploadFile(const string& filename) {
  EnterPassiveMode();  // 先进入被动模式
  SendControlMessage("TYPE I");	// 二进制传输

	int server_file_size = GetFileSize(filename);
	if (server_file_size != -1) {
		// 正常下载
		SendControlMessage("STOR " + filename);
		auto file = ifstream(filename, ios::in | ios::binary);
		AssertFileExisted(file);
		UploadFileByBuffer(file);
		file.close();
	} else {
		UploadFileWithCheckPoint(filename, server_file_size);
	}
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
