#include "config.h"
#include "../client/client.h"
#pragma comment(lib, "client.lib")

int main() {
  // Prelog
  auto client = GetClient(ip);
  client->Login(username, password);

  // Download text
  client->DownloadFile("test.txt");
  // Download binary
  client->DownloadFile("test.jpg");
  // Download folder
  client->DownloadDir("test");

  // Prolog
  delete client;
}