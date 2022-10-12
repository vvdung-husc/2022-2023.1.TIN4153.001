#include <stdio.h>
#include "..\_COMMON\Log.h"
#include "winsock2.h"
#include "windows.h"
#include "string"

std::string getIpAddress(const std::string& domain){
  struct hostent *remoteHost;
  remoteHost = gethostbyname(domain.c_str());
  if (remoteHost == NULL) {
    LOG_ET("gethostbyname() error: %ld\n", WSAGetLastError());
    return std::string();
  }
  struct in_addr addr;
  addr.s_addr = *(u_long *) remoteHost->h_addr_list[0];

  std::string ip(inet_ntoa(addr));
  LOG_IT("IP OF DOMAIN [%s] => [%s]\n",domain.c_str(),ip.c_str());
  return ip;
}

int getContentSite(const std::string& host,int port){
  std::string ip = getIpAddress(host);
  LOG_WT("2. Tao SOCKET ket noi (Client)\n");
  SOCKET sockConnect;
  sockConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockConnect == INVALID_SOCKET) {
    LOG_ET("socket() loi: %ld\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  LOG_DT("socket() OKIE\n");
  LOG_WT("3. Chuan bi server ket noi\n");
  //std::string url = "oj.husc.edu.vn";
  sockaddr_in srvService;
  srvService.sin_family = AF_INET;
  srvService.sin_addr.s_addr = inet_addr(ip.c_str());
  srvService.sin_port = htons(port);
  
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

  LOG_WT("Da ket noi server. Nhan enter de nhan noi dung site: %s \n",host.c_str());
  system("pause");

  std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  
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

  return 0;
}

std::string getContentSite(const std::string& host,int port, std::string* header){
  std::string ip = getIpAddress(host);
  SOCKET sockConnect;
  sockConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockConnect == INVALID_SOCKET) {
    LOG_ET("socket() loi: %ld\n", WSAGetLastError());
    return std::string();
  }

  sockaddr_in srvService;
  srvService.sin_family = AF_INET;
  srvService.sin_addr.s_addr = inet_addr(ip.c_str());
  srvService.sin_port = htons(port);
  
  // Connect to server.
  int iResult = connect(sockConnect, (SOCKADDR *) &srvService, sizeof (srvService));
  if (iResult == SOCKET_ERROR) {
    LOG_ET("connect() error: %ld\n", WSAGetLastError());
    closesocket(sockConnect);
    return std::string();;
  }

  std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  
  if(send(sockConnect, request.c_str(), strlen(request.c_str())+1, 0) < 0){
    LOG_ET("send() failed: %ld\n",WSAGetLastError());
  }
  
  std::string txt;
  char buffer[4097];
  int n, total = 0;
  std::string raw_site;
  while((n = recv(sockConnect, buffer, 4096, 0)) > 0){
    total += n;
    buffer[n] = 0;
    txt.append(buffer);
    //LOG_D("%s",buffer);
  }

  closesocket(sockConnect);

  size_t pos = txt.find("\r\n\r\n");//4 kytu  
  //LOG_ET("POS:%zu\n",pos);
  if (header){
    *header = txt.substr(0,pos);
  }  
  return std::string(txt.substr(pos + 4));//4ky tu "\r\n\r\n"
}

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

  //getContentSite_test();

  //const char* domain = "oj.husc.edu.vn";
  //std::string ip = getIpAddress(domain);
  getIpAddress("oj.husc.edu.vn");
  getIpAddress("google.com");
  getIpAddress("facebook.com");
  getIpAddress("vtv.vn");

  //LOG_DT("NHAN NOI DUNG TRANG WEB\n");
  //system("pause");
  const char* host = "oj.husc.edu.vn";
  std::string header;
  std::string content = getContentSite(host,80,&header);

  LOG_D("[HEADER ]==========>\n%s\n",header.c_str());
  LOG_I("[CONTENT]==========>\n%s\n",content.c_str());

  WSACleanup();

  LOG_D("--------------\n");
  return 0;
}
