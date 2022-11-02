#include <stdio.h>
#include "..\_COMMON\Log.h"
#include "..\_COMMON\Utils.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "windows.h"
#include "string"
#include <fstream>
#include "uri.h"
#include <algorithm>
#include <regex>

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

int getContentSite(const std::string& host,int port, std::string& path){  
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
  if (path.empty()) path = "/";
  std::string request = StringFormat("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",path.c_str(),host.c_str());
  
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

std::string getContentSite(const std::string& host,int port,std::string path, std::string* header){
  std::string ip = getIpAddress(host);
  SOCKET sockConnect;
  sockConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockConnect == INVALID_SOCKET) {
    LOG_ET("socket() loi: %ld\n", WSAGetLastError());
    return std::string();
  }

  sockaddr_in srvService;
  srvService.sin_family = AF_INET;
  //srvService.sin_addr.s_addr = inet_addr(ip.c_str());
  inet_pton(AF_INET,ip.c_str(),&srvService.sin_addr.s_addr);
  srvService.sin_port = htons(port);
  
  // Connect to server.
  int iResult = connect(sockConnect, (SOCKADDR *) &srvService, sizeof (srvService));
  if (iResult == SOCKET_ERROR) {
    LOG_ET("connect() error: %ld\n", WSAGetLastError());
    closesocket(sockConnect);
    return std::string();;
  }
  if (path.empty()) path = "/";
  //std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  std::string request = StringFormat("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",path.c_str(),host.c_str());
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
  std::string content = txt.substr(pos + 4);//4ky tu "\r\n\r\n"
  size_t t1 = content.find("<!DOCTYPE html>");
  if (t1 != std::string::npos){
    size_t t2 = content.rfind("</html>");
    if (t2 == std::string::npos) return std::string(content.substr(t1));
    return std::string(content.substr(t1,t2 - t1 + 7));
  }
  return std::string(content);//4ky tu "\r\n\r\n"
}

void saveHost2Html(const std::string& host,int port,std::string path, const std::string& file){  
  std::string header;
  std::string content = getContentSite(host,port,path,&header);
  LOG_D("[HEADER ]==========>\n%s\n",header.c_str());
  LOG_W("CONTENT SIZE:%zu SAVE[%s]\n",content.size(),file.c_str());
  std::ofstream fs;
  fs.open(file.c_str(),std::ofstream::out | std::ofstream::trunc);
  if (!fs.is_open()) {
    LOG_ET("fs.open() Error\n");
    return;
  }
  fs.write(content.c_str(),content.size());
  fs.close();
}

bool save2JPG(const std::string& host,int port,std::string path, FILE* f){
  std::string ip = getIpAddress(host);
  SOCKET sockConnect;
  sockConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockConnect == INVALID_SOCKET) {
    LOG_ET("socket() loi: %ld\n", WSAGetLastError());
    return false;
  }

  sockaddr_in srvService;
  srvService.sin_family = AF_INET;
  //srvService.sin_addr.s_addr = inet_addr(ip.c_str());
  inet_pton(AF_INET,ip.c_str(),&srvService.sin_addr.s_addr);
  srvService.sin_port = htons(port);
  
  // Connect to server.
  int iResult = connect(sockConnect, (SOCKADDR *) &srvService, sizeof (srvService));
  if (iResult == SOCKET_ERROR) {
    LOG_ET("connect() error: %ld\n", WSAGetLastError());
    closesocket(sockConnect);
    return false;
  }
  if (path.empty()) path = "/";
  //std::string request = "GET / HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";
  std::string request = StringFormat("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",path.c_str(),host.c_str());
  if(send(sockConnect, request.c_str(), strlen(request.c_str())+1, 0) < 0){
    LOG_ET("send() failed: %ld\n",WSAGetLastError());
  }
  
  std::string header;
  char buffer[4097];
  int n, total = 0, bytes = 0;
  std::string raw_site;
  bool isHeader_completed = false;
  while((n = recv(sockConnect, buffer, 4096, 0)) > 0){
    total += n;

    if (!isHeader_completed){
      
      buffer[n] = 0;
      
      std::string t(buffer);
      size_t pos = t.find("\r\n\r\n");//4 kytu 
      if (pos != std::string::npos){
        isHeader_completed = true;

        header.append(t.substr(0,pos));
        bytes += (total - ((int)pos + 4));
        LOG_D("[HEADER ]==========>\n%s\n",header.c_str());
        int size_header = (int)header.size();
        LOG_D("HEADER SIZE:%d bytes:%d Total:%d\n",size_header,bytes,total);
        if (f) fwrite (&buffer[pos + 4] , sizeof(char), (size_t)bytes, f);
      }
      else{
        header.append(buffer);
      }            
    }
    else{
      bytes += n;
      if (f) fwrite (buffer , sizeof(char), (size_t)n, f);
    }
  }//while((n = recv(sockConnect, buffer, 4096, 0)) > 0){

  closesocket(sockConnect);
  
  LOG_WT("IMAGE SIZE:%d/%d\n",bytes,total);
  return true;
}

void saveURL2JPG(const std::string& urlJPG, const std::string& file){
  //thử tìm hiểu đoạn code để save file ảnh
  LOG_I("Bài tập: Lưu file ảnh từ website\n");
  LOG_I("URL [%s]\n",urlJPG.c_str());  
  LOG_E("1. Tìm hiểu các tài liệu về lập trình lưu file JPG (nhị phân) trên google\n");
  LOG_E("2. Giải thích cách làm trong file WORD và đưa lên github trong thư mục của từng thành viên\n");

  FILE *f = fopen(file.c_str(),"wb");

  Uri u = Uri::Parse(urlJPG);
  save2JPG(u.Host.c_str(),u.getPort(),u.getPath(),f);

  fclose(f);
}

void showUri(const std::string& url){
  Uri u = Uri::Parse(url);
  LOG_I("URL [%s]\n",url.c_str());
  LOG_W("protocol[%s]\nhost[%s]\nport[%d]\npath[%s]\nquery[%s]\n",
    u.Protocol.c_str(),
    u.Host.c_str(),
    u.getPort(),
    u.getPath().c_str(),
    u.QueryString.c_str());
}

void test_uri(){
  showUri("https://stackoverflow.com/questions/2616011/easy-way-to-parse-a-url-in-c-cross-platform");
  showUri("http://localhost:80/foo.html?&q=1:2:3");
  showUri("https://localhost:80/foo.html?&q=1");
  showUri("localhost/foo");
  showUri("https://localhost/foo");
  showUri("localhost:8080");
  showUri("localhost?&foo=1");
  showUri("localhost?&foo=1:2:3"); 
  showUri("https://github.com/vvdung-husc/2022-2023.1.TIN4153.001");
  showUri("http://tuyensinh.husc.edu.vn/");  

  LOG_D("");
}

int main(int argc, char const *argv[])
{
  //test_uri();
  //return 0;

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
  // getIpAddress("oj.husc.edu.vn");
  // getIpAddress("google.com");
  // getIpAddress("facebook.com");
  // getIpAddress("vtv.vn");

  //LOG_DT("NHAN NOI DUNG TRANG WEB\n");
  //system("pause");
  //const char* host = "tuyensinh.husc.edu.vn";
  // std::string header;
  // std::string content = getContentSite(host,80,&header);

  // LOG_D("[HEADER ]==========>\n%s\n",header.c_str());
  // LOG_I("[CONTENT]==========>\n%s\n",content.c_str());

  //showUri("http://tuyensinh.husc.edu.vn/category/quyche/");
  //std::string url = "http://daotao.hutech.edu.vn/Upload/file/HuongDanHuyHP/HUONG%20DAN%20HUY%20HP%202022.doc.docx";
  std::string url = "http://iuh.edu.vn/Resource/Upload2/Image/album/toan%20canh%20xl.JPG";
  Uri u = Uri::Parse(url);
  //saveHost2Html(u.Host.c_str(),u.getPort(),"/","test.html");
  //saveHost2Html(u.Host.c_str(),u.getPort(),u.getPath(),"quyche.html");
  //saveHost2Html(u.Host.c_str(),u.getPort(),"/","iuh.html");
  //saveHost2Html(u.Host.c_str(),u.getPort(),u.getPath(),"thisinh.html");
   
  //showUri("http://iuh.edu.vn/Resource/Upload2/Image/album/toan%20canh%20xl.JPG");
  //showUri(url);
  
  std::string fname = "image.jpg";
  size_t pos = url.rfind("/");
  if (pos != std::string::npos){
    fname = url.substr(pos + 1);
    fname = std::regex_replace(fname, std::regex("%20"), "_");
    //std::replace( fname.begin(),fname.end(), (char)0x20, '_');
  }
  

  saveURL2JPG(url, fname.c_str());  
  
  WSACleanup();

  

  LOG_D("--------------\n");
  return 0;
}
