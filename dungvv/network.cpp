#include <stdio.h>
#include "..\_COMMON\Log.h"
#include "winsock2.h"
#include "windows.h"

int main(int argc, char const *argv[])
{
  WSADATA wsaData;
  WORD wVersion = MAKEWORD(2,2);
  int err = WSAStartup(wVersion,&wsaData);
  if (err != 0) {
    LOG_ET("WSAStartup() error: %d\n", err);
    return 1;
  }
  LOG_WT("Winsock successful");
  

  LOG_D("");
  return 0;
}
