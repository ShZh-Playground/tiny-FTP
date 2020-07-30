#include "pch.h"

#include <iostream>
#include "../client/client.h"
#include "../lib/spdlog/include/spdlog/sinks/basic_file_sink.h"

#pragma comment(lib, "client.lib")

using namespace std;

class ClientTest : public ::testing::Test {
 protected:
  ClientSpace::IClient* client;

  ClientTest() {}
  ~ClientTest() override {}

  void SetUp() override {
    iostream::sync_with_stdio(false);
    // Connect to local Windows IIS FTP server
    // Change to your server ip, username and password
    this->client = GetClient("192.168.176.1");
    client->Login("shzh", "123456");
  }

  void TearDown() override { delete this->client; }
};

/******************************Download test******************************/

TEST_F(ClientTest, TestDownloadTXT) { this->client->DownloadFile("zh.txt"); }

TEST_F(ClientTest, TestDownloadBinary) {
  this->client->DownloadFile("image.jpg");
}

TEST_F(ClientTest, TestDownloadFolder) { this->client->DownloadDir("test"); }

// No need to download unexisted file or folder for the project is using GUI

/*********************************Upload test*********************************/

TEST_F(ClientTest, TestUploadTXT) { this->client->UploadFile("zh.txt"); }

TEST_F(ClientTest, TestUploadBinary) { this->client->UploadFile("image.jpg"); }

TEST_F(ClientTest, TestUploadFolder) { this->client->UploadDir("test"); }

// No need to upload unexisted file or folder for the project is using GUI

/*********************************Remove test*********************************/

TEST_F(ClientTest, TestRemoveFile) { this->client->RemoveFile("fuck.txt"); }

TEST_F(ClientTest, TestRemoveFolder){ this->client->RemoveDir("test1"); }

/***********************Get server information test**************************/

TEST_F(ClientTest, TestGetFileSize) {
  ASSERT_EQ(this->client->GetFileSize("test1"), 26);
}

// No need to get folder size, we don't event know folder size in modern ftp
// client

// Test GetWorkingDir and ChangeWorkingDir at the same time
TEST_F(ClientTest, GetWorkingDir) {
  this->client->ChangeWorkingDir("test");
  ASSERT_STREQ(this->client->GetWorkingDir().c_str(), "/test");
}

TEST_F(ClientTest, GetDirList) {
  auto path_list = this->client->GetDirList("");
  for (auto path : path_list) {
    std::cout << "Name: " << path.name_ << "; IsDir: " << std::boolalpha
              << path.is_dir_ << std::endl;
  }
}

/*********************************Other test*********************************/

TEST_F(ClientTest, TestMakeDir) { this->client->MakeDir("unittest"); }

TEST_F(ClientTest, TestRenameFile) {
  this->client->Rename("test.txt", "tset.txt");
}

TEST_F(ClientTest, TestRenameFolder) { this->client->Rename("test", "test1"); }

/*********************************Tests end*********************************/

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "ClientTest.TestRemoveFolder";
  return RUN_ALL_TESTS();
}