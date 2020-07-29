#pragma once

#include <string>
#include <vector>
#include "../socket/ftp_socket.h"
#include "../socket/data_socket.h"
#include "../socket/control_socket.h"

#ifdef _DLL_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

// 利用COM的思想，使用接口导出其子类
class IClient {
 public:
	virtual ~IClient() {}

  virtual void Login(const std::string& username, const std::string& password)=0;
  virtual std::vector<std::string> GetDirList(const std::string&) = 0;
	virtual bool MakeDir(const std::string& dirname)=0;

	virtual bool ChangeWorkingDir(const std::string&)=0;
	virtual std::string GetWorkingDir()=0;
	
	virtual void DownloadFile(const std::string& filename)=0;
  virtual void UploadFile(const std::string& filename)=0;
  virtual bool RemoveFile(const std::string& filename)=0;
	virtual bool Rename(const std::string& old_name, const std::string& new_name)=0;
  virtual int GetFileSize(const std::string& filename)=0;
};

class Client: public IClient {
 private:
	const std::string ip_address_;
  ControlSocket control_socket_;
  DataSocket data_socket_;

	void EnterPassiveMode();
  void DownloadFileWithCheckPoint(const std::string& filename);
	void UploadFileWithCheckPoint(const std::string& filename, int server_file_size);

 public:
  Client(const std::string& ip_address, unsigned int port);
  ~Client();

  void Login(const std::string&, const std::string&);

	bool Rename(const std::string& old_name, const std::string& new_name);

	bool RemoveFile(const std::string& filename);

	bool RemoveDir(const std::string& dirname);

	bool MakeDir(const std::string& dirname);

	std::vector<std::string> GetDirList(const std::string&);

	bool ChangeWorkingDir(const std::string&);

	std::string GetWorkingDir();

  int GetFileSize(const std::string& filename);

  void DownloadFile(const std::string& filename);

  void UploadFile(const std::string& filename);
};

extern "C" DLL_API IClient* GetClient(const std::string ip_address);


