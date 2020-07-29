#pragma once

#include <string>
#include <vector>
#include "../share/fs/path_info.h"
#include "../share/socket/control_socket.h"
#include "../share/socket/data_socket.h"
#include "../share/socket/ftp_socket.h"

#ifdef _DLL_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

namespace ClientSpace {
	using namespace std;
	using namespace Socket;
	using namespace FileSystem;

	// 利用COM的思想，使用接口导出其子类
	class IClient {
	 public:
		virtual ~IClient() {}

		virtual void Help() = 0;
		virtual bool Login(const string& username,
											 const string& password) = 0;
		virtual vector<PathInfo> GetDirList(const string&) = 0;
		virtual bool MakeDir(const string& dirname) = 0;
		virtual void UploadDir(const string& dirname) = 0;
		virtual void DownloadDir(const string& dirname) = 0;

		virtual bool ChangeWorkingDir(const string&) = 0;
		virtual string GetWorkingDir() = 0;

		virtual void DownloadFile(const string& filename) = 0;
		virtual void UploadFile(const string& filename) = 0;
		virtual bool RemoveFile(const string& filename) = 0;
		virtual bool Rename(const string& old_name,
												const string& new_name) = 0;
		virtual int GetFileSize(const string& filename) = 0;
	};

	class Client : public IClient {
	 private:
		const string ip_address_;
		ControlSocket control_socket_;
		DataSocket data_socket_;

		void EnterPassiveMode();
		const string PrintMessage();
		const string SendControlMessage(const string&);
		void DownloadFileWithCheckPoint(const string& filename);
		void UploadFileWithCheckPoint(const string& filename,
																	int server_file_size);

	 public:
		Client(const string& ip_address, unsigned int port);
		~Client();

		void Help();

		bool Login(const string&, const string&);

		bool Rename(const string& old_name, const string& new_name);

		bool RemoveFile(const string& filename);

		bool RemoveDir(const string& dirname);

		bool MakeDir(const string& dirname);

		vector<PathInfo> GetDirList(const string&);

		void DownloadDir(const string& dirname);

		void UploadDir(const string& dirname);

		bool ChangeWorkingDir(const string&);

		string GetWorkingDir();

		int GetFileSize(const string& filename);

		void DownloadFile(const string& filename);

		void UploadFile(const string& filename);
	};
}


extern "C" DLL_API ClientSpace::IClient* GetClient(const std::string ip_address);
