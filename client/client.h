#pragma once

#include <string>
#include "ftp_socket.h"

#ifdef _DLL_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

class IClient {
 public:
	// 让子类调用其析构函数，释放子类的资源
	virtual ~IClient() {}

	virtual const std::string ReceiveMessage()=0;
  virtual void Login(const std::string& username, const std::string& password)=0;
};

class Client: public IClient {
 private:
	const std::string ip_address_;
  FTPSocket control_socket_;
  FTPSocket data_socket_;

	void EnterPassiveMode();
  unsigned int ResolveDataSocketPort(const std::string&);

 public:
  Client(const std::string& ip_address, unsigned int port);
  ~Client();

  void Login(const std::string&, const std::string&);
  const std::string ReceiveMessage();
  // void download_file();
};

extern "C" DLL_API IClient* GetClient(const std::string ip_address);


