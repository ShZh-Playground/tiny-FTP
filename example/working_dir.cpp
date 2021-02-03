#include "config.h"
#include "../client/client.h"

#include <iostream>
#pragma comment(lib, "client.lib")

int main() {
  // Prelog
  auto client = GetClient(ip);
  client->Login(username, password);

  // Change working directory
  client->ChangeWorkingDir("test");
  // Show current working directory
  std::cout << client->GetWorkingDir() << std::endl;

  // Prolog
  delete client;
}