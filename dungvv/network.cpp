#include <stdio.h>
#include "..\_COMMON\Log.h"
#include "winsock2.h"
#include "windows.h"
#include "string"

int main(int argc, char const *argv[])
{
  LOG_IT("1. Khoi tao WinSocket\n");
  WSADATA wsaData;
  WORD wVersion = MAKEWORD(2,2);
  int err = WSAStartup(wVersion,&wsaData);
  if (err != 0) {
    LOG_ET("WSAStartup() loi: %d\n", err);
    return 1;
  }
  LOG_DT("Winsock khoi tao thanh cong\n");
  LOG_IT("2. Tao SOCKET ket noi (Client)\n");
  SOCKET sockConnect;
  sockConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockConnect == INVALID_SOCKET) {
    LOG_ET("socket() loi: %ld\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }
  LOG_DT("socket() OKIE\n");
  LOG_IT("3. Chuan bi server ket noi\n");
  std::string url = "oj.husc.edu.vn";
  sockaddr_in srvService;
  srvService.sin_family = AF_INET;
  srvService.sin_addr.s_addr = inet_addr("222.255.148.164");
  srvService.sin_port = htons(80);
  
  // Connect to server.
  int iResult = connect(sockConnect, (SOCKADDR *) &srvService, sizeof (srvService));
  if (iResult == SOCKET_ERROR) {
    LOG_ET("connect() error: %ld\n", WSAGetLastError());
    iResult = closesocket(sockConnect);
    if (iResult == SOCKET_ERROR)
        wprintf(L"closesocket() error: %ld\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  LOG_WT("Connected to server. Press Enter to get content %s \n",url.c_str());
  system("pause");

  std::string request = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
  
  if(send(sockConnect, request.c_str(), strlen(request.c_str())+1, 0) < 0){
    LOG_ET("send() failed: %ld\n",WSAGetLastError());
  }
  
  char buffer[4096];
  int n, total = 0;
  std::string raw_site;
  while((n = recv(sockConnect, buffer, sizeof(buffer)+1, 0)) > 0){
    total += n;
    buffer[n] = 0;
    LOG_D("%s",buffer);
  }
  LOG_WT("Tong so bytes nhan duoc: %d\n",total);
  LOG_WT("Press Enter to exit \n");
  system("pause");

  iResult = closesocket(sockConnect);
  if (iResult == SOCKET_ERROR) {
    wprintf(L"closesocket() error: %ld\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  WSACleanup();

  LOG_D("");
  return 0;
}
