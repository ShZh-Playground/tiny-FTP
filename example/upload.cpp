#include "config.h"
#include "../client/client.h"
#pragma comment(lib, "client.lib")

int main() {
  // Prelog
  auto client = GetClient(ip);
  client->Login(username, password);

  // Upload text
  client->UploadFile("test.txt");
  // Upload binary
  client->UploadFile("test.jpg");
  // Upload folder
  client->UploadDir("test");

  // Prolog
  delete client;
}