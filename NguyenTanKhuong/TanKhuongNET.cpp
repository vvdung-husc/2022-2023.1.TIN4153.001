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

  getIpAddress("oj.husc.edu.vn");
  getIpAddress("google.com");
  getIpAddress("facebook.com");
  getIpAddress("vtv.vn");
  getIpAddress("husc.edu.vn");
  getIpAddress("github.com");
  getIpAddress("zalo.me");


  WSACleanup();

  LOG_D("");
  return 0;
}
