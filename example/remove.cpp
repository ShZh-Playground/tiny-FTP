#include "config.h"
#include "../client/client.h"
#pragma comment(lib, "client.lib")

int main() {
  // Prelog
  auto client = GetClient(ip);
  client->Login(username, password);

  // Remove file
  client->RemoveFile("test.txt");
  // Remove dir
  client->RemoveDir("test");

  // Prolog
  delete client;
}