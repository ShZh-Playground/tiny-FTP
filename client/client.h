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
	virtual std::string receive_message()=0;	
};

class Client: public IClient {
 private:
  FTPSocket control_socket_;
  FTPSocket data_socket_;

 public:
  Client(const std::string ip_address, unsigned int port);
  ~Client();

  // void login(const std::string username=nullptr, const std::string
  // password=nullptr);
  std::string receive_message();
  // void download_file();
};

extern "C" DLL_API IClient* GetClient(const std::string ip_address);


