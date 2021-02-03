#include "config.h"
#include "../client/client.h"

#include <iostream>
#pragma comment(lib, "client.lib")

int main() {
  // Prelog
  auto client = GetClient(ip);
  client->Login(username, password);

  // Get directory information
  auto path_list = client->GetDirList("");
  for (auto path : path_list) {
    std::cout << "Name: " << path.name_ << "; IsDir: " << std::boolalpha
      << path.is_dir_ << std::endl;
  }

  // Get size of file
  std::cout << client->GetFileSize("test.txt") << std::endl;

  // Make directory
  client->MakeDir("test");

  // Rename file or folder
  client->Rename("test.txt", "text.txt");

  // Prolog
  delete client;
}