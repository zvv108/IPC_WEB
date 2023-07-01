#include "gb28181_api.h"
#include "common.h"
#include <dbserver.h>
#include <fstream>
#include <netserver.h>
#include <sys/statfs.h>
#include <system_manager.h>
#include <mediaserver.h>

namespace xggd{
namespace cgi{

void gb28181_mediaserver_set(int id) {

  nlohmann::json setting_config;
  setting_config.emplace("TYPE", "gb28181");

  mediaserver_update_set(id, (char *)setting_config.dump().c_str());

}


void Gb28181ApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  char *str;
  std::string path_api_resource;
  std::string path_specific_resource;
  std::string para_channel;
  nlohmann::json content;


  /* Get Path Information */
  int pos_first = Req.PathInfo.find_first_of("/");
  path_api_resource = Req.PathInfo.substr(pos_first + 1, Req.PathInfo.size());
  pos_first = path_api_resource.find_first_of("/");
  if (pos_first != -1) {
    path_specific_resource =
        path_api_resource.substr(pos_first + 1, path_api_resource.size());
    pos_first = path_specific_resource.find_first_of("/");
    if (pos_first != -1)
      para_channel = path_specific_resource.substr(
          pos_first + 1, path_specific_resource.size() + 1);
  }



  if (Req.Method == "GET") {
     /*   获取28181配置信息     */
      if (!path_specific_resource.compare("device-info")) {
      nlohmann::json data_config;
      str = dbserver_gb28181_get((char *)TABLE_28181_DEVICE_INFO);
//      minilog_debug("str is : %s",str);
      data_config = nlohmann::json::parse(str).at("jData").at(0);
      data_config.erase("id");
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);

      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    } else if (!path_specific_resource.compare("On-Line")) {

      /*  激活28181/启用28181  */
//      struct statfs diskInfo;
//      statfs("/userdata/", &diskInfo);
//      // Free space for non-root users
//      unsigned long long availableDisk = diskInfo.f_bavail * diskInfo.f_bsize;
//      content.emplace("availableDisk", availableDisk);
//      Resp.setHeader(HttpStatus::kOk, "OK");
//      Resp.setApiData(content);
    }
  }  else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    /* 保存配置信息 */
    nlohmann::json system_config = Req.PostObject; /* must be json::object */
    if (!path_specific_resource.compare("device-info")) {
      /* Set */
      dbserver_gb28181_set((char *)TABLE_28181_DEVICE_INFO,
                           (char *)system_config.dump().c_str(), 0);
      gb28181_mediaserver_set(0);

      content.emplace("code",200);
      content.emplace("msg","setting successfully");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }else {
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
    }
  }else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}
} // namespace cgi
} // namespace xggd
