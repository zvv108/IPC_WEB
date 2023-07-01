#include "network_api.h"
#include "common.h"
#include <dbserver.h>
#include <netserver.h>
#include <stdio.h>
#include <sys/time.h>
#include <mediaserver.h>

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>


#define _SVID_SOURCE
#define CAP_SYS_TIME
namespace xggd {
namespace cgi {





// 读取指定行数据
void ReadLineData(char* fileName, int lineNum, char* data)
{
    std::ifstream in;
    in.open(fileName);

    int line = 1;
    while (in.getline(data, 1024))
    {
        if (lineNum == line)
        {
                break;
        }
        line ++ ;
    }

    in.close();
}

// 字符串转string类型
std::string CharToStr(char * contentChar)
{
    std::string tempStr;
    for (int i=0; contentChar[i]!='\0'; i++)
    {
        tempStr+=contentChar[i];
    }
    return tempStr;
}



// 删除指定行
void DelLineData(char* fileName, int lineNum)
{
    std::ifstream in;
    in.open(fileName);

    std::string strFileData = "";
    int line = 1;
    char lineData[1024] = {0};
    while(in.getline(lineData, sizeof(lineData)))
    {
        if (line == lineNum)
        {
            strFileData += "\n";
        }
        else
        {
            strFileData += CharToStr(lineData);
            strFileData += "\n";
        }
        line++;
    }
    in.close();

    //写入文件
    std::ofstream out;
    out.open(fileName);
    out.flush();
    out<<strFileData;
    out.close();
}

// 修改行数据
void ModifyLineData(char* fileName, int lineNum, char* lineData)
{
    std::ifstream in;
    in.open(fileName);

    std::string strFileData = "";
    int line = 1;
    char tmpLineData[1024] = {0};
    while(in.getline(tmpLineData, sizeof(tmpLineData)))
    {
        if (line == lineNum)
        {
            strFileData += CharToStr(lineData);
            strFileData += "\n";
        }
        else
        {
            strFileData += CharToStr(tmpLineData);
            strFileData += "\n";
        }
        line++;
    }
    in.close();

    //写入文件
    std::ofstream out;
    out.open(fileName);
    out.flush();
    out<<strFileData;
    out.close();
}






std::vector<std::string> split_c(std::string strSource, std::string strSplit,int nSkip)
{
    std::vector<std::string> vecStr;
    std::vector<std::string>::size_type sPos = nSkip;
    std::vector<std::string>::size_type ePos = strSource.find(strSplit,sPos);
    while(ePos != std::string::npos)
    {
        if(sPos!=ePos)vecStr.push_back(strSource.substr(sPos,ePos - sPos));
        sPos = ePos + strSplit.size();
        ePos = strSource.find(strSplit,sPos);
    }
    if(sPos < strSource.size())vecStr.push_back(strSource.substr(sPos,strSource.size() - sPos));

    return vecStr;
}

int port_occupied(int port)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if(fd <0)
    minilog_debug("socket error");
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
  if(bind(fd, (struct sockaddr *)(&addr), sizeof(sockaddr_in)) < 0)
  {
    minilog_debug("port %d has been used.\n", port);
    close(fd);
    return -1;
  }
  close(fd);
  return 1;
}

void network_mediaserver_set(std::string type) {

  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}

nlohmann::json network_get_config(std::string interface) {
  char *str = netserver_get_networkip((char *)interface.c_str());

  nlohmann::json config = nlohmann::json::parse(str).at(0);
  nlohmann::json config_ipv4 = config.at("ipv4");

  std::string dns1 = "";
  std::string dns2 = "";
  std::string duplex_s = "";
  int duplex = config.at("link").at("iDuplex");
  int speed = config.at("link").at("iNicSpeed");
  std::string v4_method = config.at("dbconfig").at("sV4Method");
  std::string sNicSpeed = config.at("dbconfig").at("sNicSpeed");

  config.at("link").erase("sNicSpeed");
  config.at("link").emplace("sNicSpeed", sNicSpeed);
  if (!v4_method.compare("manual"))
  {
    dns1 = config.at("dbconfig").at("sDNS1");
    dns2 = config.at("dbconfig").at("sDNS2");
    config.at("link").erase("sDNS1");
    config.at("link").erase("sDNS2");
    config.at("link").emplace("sDNS1", dns1);
    config.at("link").emplace("sDNS2", dns2);

  }

  dns1 = config.at("dbconfig").at("sDNS1");
  dns2 = config.at("dbconfig").at("sDNS2");
  config_ipv4.emplace("sDNS1", dns1);
  config_ipv4.emplace("sDNS2", dns2);
  config.erase("dbconfig");
  config.at("ipv4").emplace("sV4Method", v4_method);

  str = dbserver_network_ip_get("eth1");
  nlohmann::json ip_config = nlohmann::json::parse(str).at("jData").at(0);
  config_ipv4.erase("sV4Gateway");
  config_ipv4.emplace("sV4Gateway", (std::string)(ip_config.at("sV4Gateway")));
//  return config;
  return config_ipv4;
}



void NetworkApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {

  char *str;
  char pService[255];
  nlohmann::json content;
  std::string path_api_resource;
  std::string path_specific_resource;
#ifdef ENABLE_JWT
  int user_level = Req.UserLevel;
#endif

  // Get Path Information
  int pos_first = Req.PathInfo.find_first_of("/");
  path_api_resource = Req.PathInfo.substr(pos_first + 1, Req.PathInfo.size());
  pos_first = path_api_resource.find_first_of("/");
  if (pos_first != -1) {
    path_specific_resource =
        path_api_resource.substr(pos_first + 1, path_api_resource.size());
  }

  if (Req.Method == "GET") {
    if (!path_specific_resource.compare("lan")) {
      nlohmann::json data_config =  network_get_config("eth1");

      content.emplace("data",data_config);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");

    }
    if (!content.empty()) {
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
#ifdef ENABLE_JWT
    if (user_level > 1) {
      Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
      return;
    }
#endif
    nlohmann::json cfg_new = Req.PostObject; // must be json::object
    std::string interface;
    // Set network/wlan or network/lan
    if (path_specific_resource.find("lan") != std::string::npos) {
      interface = "eth1";
      nlohmann::json cfg_old = network_get_config(interface);
      std::string Address_old = (std::string)(cfg_old.at("sV4Address"));
      std::string Address_new = (std::string)(cfg_new.at("sV4Address"));

      std::string Address = cfg_new.at("sV4Address").dump();
      Address.erase(0, 1).erase(Address.end() - 1, Address.end());
      std::string Netmask = cfg_new.at("sV4Netmask").dump();
      Netmask.erase(0, 1).erase(Netmask.end() - 1, Netmask.end());
      std::string Gateway = cfg_new.at("sV4Gateway").dump();
      Gateway.erase(0, 1).erase(Gateway.end() - 1, Gateway.end());


      nlohmann::json ip_to_media;
      ip_to_media.emplace("sIPaddress", Address_new);
      ip_to_media.emplace("iStatus", 0);
      dbserver_network_ip_set_id((char *)ip_to_media.dump().c_str(), 1);

      if(strcmp(Address_new.c_str(), Address_old.c_str()))
      {
          network_mediaserver_set("pingtest");
          char* str = dbserver_network_ip_get_id();
          nlohmann::json ip_config = nlohmann::json::parse(str).at("jData").at(1);
          if((int)(ip_config.at("iStatus")) ==1)
          {
            content.emplace("code",203);
            content.emplace("msg", "This IP address is already occupied. Setting IP failed ");

          }
          else
          {
            dbserver_network_ip_set_id((char *)ip_to_media.dump().c_str(), 0);

            dbserver_network_ipv4_set(
                (char *)interface.c_str(), (char *)"manual", (char *)Address.c_str(),
                (char *)Netmask.c_str(), (char *)Gateway.c_str());

            char buffer[512];
            ReadLineData("/etc/network/interfaces.d/lan", 12, buffer);
            ModifyLineData("/etc/network/interfaces.d/lan", 12, const_cast<char*>(("address "+ Address_new).c_str()));
            std::string dns1 = "";
            std::string dns2 = "";
            dns1 = cfg_new.at("sDNS1").dump();
            dns2 = cfg_new.at("sDNS2").dump();
            dns1.erase(0, 1).erase(dns1.end() - 1, dns1.end());
            dns2.erase(0, 1).erase(dns2.end() - 1, dns2.end());
            dbserver_network_dns_set((char *)interface.c_str(),
                                     (char *)dns1.c_str(), (char *)dns2.c_str());


            netserver_reboot();
            content.emplace("code",200);
            content.emplace("msg","setting successfully，the system will reboot now to make changes effective, relogin after 8 minutes");
          }

      }else
      {
          dbserver_network_ipv4_set(
              (char *)interface.c_str(), (char *)"manual", (char *)Address.c_str(),
              (char *)Netmask.c_str(), (char *)Gateway.c_str());
          content.emplace("code",200);
          content.emplace("msg","setting successfully");
      }



    }else if (!path_specific_resource.compare("pingtest")) {
      std::string Address_new =(std::string)(cfg_new.at("sV4Address"));

      nlohmann::json temp_data;
      nlohmann::json ip_to_media;
      ip_to_media.emplace("sIPaddress", Address_new);
      ip_to_media.emplace("iStatus", 0);
      dbserver_network_ip_set_id((char *)ip_to_media.dump().c_str(), 1);

      network_mediaserver_set("pingtest");
      char* str = dbserver_network_ip_get_id();
      nlohmann::json ip_config = nlohmann::json::parse(str).at("jData").at(1);
      if((int)(ip_config.at("iStatus")) ==1)
      {

        temp_data.emplace("iStatus", 1);
        content.emplace("code", 200);
        content.emplace("data", temp_data);
        content.emplace("msg", "This IP address is already occupied. ");
      }else
      {
        temp_data.emplace("iStatus", 0);
        content.emplace("code", 200);
        content.emplace("data", temp_data);
        content.emplace("msg", "This IP address is not occupied. ");
      }

    }
    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);
  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}

void NetworkNTPApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  char *str;
  std::string path_api_resource;
  std::string path_specific_resource;
  nlohmann::json content;

  /* Get Path Information */
  int pos_first = Req.PathInfo.find_first_of("/");
  path_api_resource = Req.PathInfo.substr(pos_first + 1, Req.PathInfo.size());
  pos_first = path_api_resource.find_first_of("/");
  if (pos_first != -1) {
    path_specific_resource =
        path_api_resource.substr(pos_first + 1, path_api_resource.size());
  }

  if (Req.Method == "GET") {
    if (path_specific_resource.empty()) {
      str = dbserver_ntp_get();
      content = nlohmann::json::parse(str).at("jData").at(0);
      content.erase("id");
      content.erase("iAutoDst");
      content.erase("sTimeZone");
      content.erase("sTimeZoneFile");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    } else if (path_specific_resource.find("zone") != std::string::npos) {
      str = dbserver_zone_get();
      content = nlohmann::json::parse(str).at("jData");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    } else if (path_specific_resource.find("time") != std::string::npos) {
      nlohmann::json data_config ;

      str = dbserver_ntp_get();
      data_config = nlohmann::json::parse(str).at("jData").at(0);
      data_config.erase("id");
      data_config.erase("iAutoDst");
      data_config.erase("sTimeZone");
      data_config.erase("sTimeZoneFile");

      data_config.emplace("time", current_time_get());

      content.emplace("msg","operation successfully");
      content.emplace("code",200);
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    } else {
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
    }
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
#ifdef ENABLE_JWT
//      if (user_level > 1) {
//        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
//        return;
//      }
#endif
    if (path_specific_resource.empty()) {                        //时区
      if (!Req.PostObject.empty()) {
        nlohmann::json ntp_config = Req.PostObject;

        std::string zone;
        std::string zone_file;
        std::string service = ntp_config.at("sNtpServers").dump();
        service.erase(0, 1).erase(service.end() - 1, service.end()); // erase \"
        std::string zone_file_dst = ntp_config.at("sTimeZoneFileDst").dump();
        zone_file_dst.erase(0, 1).erase(zone_file_dst.end() - 1,
                                        zone_file_dst.end());


        if(!zone_file_dst.compare("posix/Asia/Shanghai"))
        {
          zone = "ChinaStandardTime-8";
          zone_file ="posix/Etc/GMT-8";
          system("sudo cp /usr/share/zoneinfo/Asia/Singapore /etc/localtime");

        }else if(!zone_file_dst.compare("posix/US/Hawaii"))
        {
          zone = "CHawaiianStandardTime10";
          zone_file ="posix/Etc/GMT+10";
          system("sudo cp /usr/share/zoneinfo/US/Hawaii /etc/localtime");


        }else if(!zone_file_dst.compare("posix/US/Pacific"))
        {
          zone = "PacificStandardTime8DaylightTime,M3.2.0,M11.1.0";
          zone_file ="posix/Etc/GMT+8";
          system("sudo cp /usr/share/zoneinfo/US/Pacific /etc/localtime");

        }else if(!zone_file_dst.compare("posix/Europe/London"))
        {
          zone = "GMTStandardTime0DaylightTime,M3.5.0/1,M10.5.0";
          zone_file ="posix/Etc/GMT-0";
          system("sudo cp /usr/share/zoneinfo/Europe/London /etc/localtime");

        }else if(!zone_file_dst.compare("posix/Asia/Tokyo"))
        {
          zone = "TokyoStandardTime-9";
          zone_file ="posix/Etc/GMT-9";
          system("sudo cp /usr/share/zoneinfo/Asia/Tokyo /etc/localtime");

        }
        // automatic daylight saving time
        //        int auto_dst = atoi(ntp_config.at("iAutoDst").dump().c_str());
        int auto_dst = 0;
        int update_time = atoi(ntp_config.at("iRefreshTime").dump().c_str());
        int auto_mode = atoi(ntp_config.at("iAutoMode").dump().c_str());
        dbserver_ntp_set((char *)service.c_str(), (char *)zone.c_str(),
                         (char *)zone_file.c_str(),
                         (char *)zone_file_dst.c_str(), &auto_dst, &auto_mode,
                         &update_time);
      }

      network_mediaserver_set("time");
      nlohmann::json data_config ;
      data_config.emplace("time", current_time_get());

      content.emplace("msg","setting successfully");
      content.emplace("code",200);
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if (path_specific_resource.find("time") != std::string::npos)
    {
      nlohmann::json ntp_config = Req.PostObject;
      int auto_dst=0;
      int auto_mode=0;
      int update_time = 60;
      std::string sTimeZoneFileDst = ntp_config.at("sTimeZoneFileDst");
      dbserver_ntp_set("time.nist.gov", "ChinaStandardTime-8",
                        "posix/Etc/GMT-8",
                       (char *)sTimeZoneFileDst.c_str(), &auto_dst, &auto_mode,
                       &update_time);
      nlohmann::json time_config = Req.PostObject;
      std::string time_s = time_config.at("time");   //前端设置的时间

      int64_t time_old = time(0);
      std::vector<std::string> time_s_temp =split_c(time_s, " ",0);
      std::string time_s_str = time_s_temp[0] +"T" +time_s_temp[1];

      std::string sys_settime = "echo xg | sudo -S date -s "+time_s_str +" >/var/IPC-temp.log";


      system(sys_settime.c_str());
      system("echo xg | sudo -S hwclock -w -u");
      network_mediaserver_set("time");

      nlohmann::json data_config ;
      data_config.emplace("time", current_time_get());

      content.emplace("msg","operation successfully");
      content.emplace("code",200);
      content.emplace("data",data_config);

      int64_t diff_time = time(0) - time_old;


      char *device_str = dbserver_system_get((char *)TABLE_SYSTEM_DEVICE_INFO);
      nlohmann::json device_config = nlohmann::json::parse(device_str).at("jData").at(0);
      int64_t priv_difftime = device_config.at("iDiffTimeInt");

      int64_t diff_timr_convert = priv_difftime - diff_time;

      nlohmann::json time_diff_config ;
      time_diff_config.emplace("iDiffTimeInt", diff_timr_convert),
      dbserver_system_set((char *)TABLE_SYSTEM_DEVICE_INFO,
                          (char *)time_diff_config.dump().c_str(), 0);
      if(diff_time > 86000)
      {
          system("echo xg |sudo killall srs");
      }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }
  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}



void NetworkPortApiHandler::handler(const HttpRequest &Req,
                                    HttpResponse &Resp) {
  if (Req.Method == "GET") {
    nlohmann::json content;
    nlohmann::json data_config;
    char *str = dbserver_port_get();
    nlohmann::json port_all_config = nlohmann::json::parse(str).at("jData");
    nlohmann::json port_current_config;
    int pos_first = Req.PathInfo.find_first_of("/");
    int pos_last = Req.PathInfo.find_last_of("/");

    if (pos_first != pos_last) {
      int id =
          atoi(Req.PathInfo.substr(pos_last + 1, Req.PathInfo.size()).c_str());
      port_current_config = port_all_config.at(id);
      content = port_current_config;
    } else {
      data_config = port_all_config.at(0);
      data_config.erase("id");
//      data_config.emplace("WEBSOCKET", 1983);
      content.emplace("code", 200);
      content.emplace("msg", "operation successfully");
      content.emplace("data", data_config);
//      content = port_all_config;
    }
    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
#ifdef ENABLE_JWT
//      if (user_level > 1) {
//        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
//        return;
//      }
#endif
    nlohmann::json content;
    nlohmann::json port_config = Req.PostObject;

    char *prev = dbserver_port_get();
    nlohmann::json cfg_old_all = nlohmann::json::parse(prev).at("jData").at(0);

    if(std::stoi((std::string)(cfg_old_all.at("HTTP"))) ==std::stoi((std::string)(port_config.at("HTTP"))))
       port_config.erase("HTTP");
    if(std::stoi((std::string)(cfg_old_all.at("RTSP"))) == std::stoi((std::string)(port_config.at("RTSP"))))
       port_config.erase("RTSP");
    if(std::stoi((std::string)(cfg_old_all.at("SDKSERVER"))) == std::stoi((std::string)(port_config.at("SDKSERVER"))))
       port_config.erase("SDKSERVER");
    int ret = 0;
    if (!port_config.empty())
    {
      for (auto &x : nlohmann::json::iterator_wrapper(port_config))
      {
          int16_t port_mew = std::stoi(port_config.at(x.key()).get<std::string>());
          if(port_mew ==80 && x.key()=="HTTP")
              continue;
          if(port_mew == 8080 || port_mew > 65535)
          {
              ret=-1;
              break;
          }
          ret = port_occupied(port_mew);
          if(ret <0)
              break;
          continue;
        }
       if(ret < 0)
      {
         content.emplace("code", 203);
         content.emplace("msg", "The port is occupied, setting port failed !");
      }else
      {
         dbserver_port_set((char *)port_config.dump().c_str(), 0);
         network_mediaserver_set("port");

         content.emplace("code", 200);
         content.emplace("msg", "setting successfully");
      }
    }else
    {
      content.emplace("code", 200);
      content.emplace("msg", "setting successfully");
    }


    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);


  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}

} // namespace cgi
} // namespace xggd
