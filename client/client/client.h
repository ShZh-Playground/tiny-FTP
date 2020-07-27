#pragma once

#include <string>
#include <vector>
#include "../socket/ftp_socket.h"
#include "../socket/data_socket.h"

#ifdef _DLL_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

// 利用COM的思想，使用接口导出其子类
class IClient {
 public:
	// 让子类调用其析构函数，释放子类的资源
	virtual ~IClient() {}

	virtual const std::string ReceiveMessage()=0;
  virtual void Login(const std::string& username, const std::string& password)=0;
  virtual std::vector<std::string> GetDirList() = 0;
	virtual unsigned int GetFileSize(const std::string& filename)=0;
	virtual void DownloadFile(const std::string& filename)=0;
  virtual void UploadFile(const std::string& filename)=0;
};

class Client: public IClient {
 private:
	const std::string ip_address_;
  FTPSocket control_socket_;
  DataSocket data_socket_;

	void EnterPassiveMode();
  unsigned int ResolveDataSocketPort(const std::string&);

 public:
  Client(const std::string& ip_address, unsigned int port);
  ~Client();

  void Login(const std::string&, const std::string&);
  const std::string ReceiveMessage();

	std::vector<std::string> GetDirList();
  unsigned int GetFileSize(const std::string& filename);

  void DownloadFile(const std::string& filename);
  void UploadFile(const std::string& filename);
};

extern "C" DLL_API IClient* GetClient(const std::string ip_address);


