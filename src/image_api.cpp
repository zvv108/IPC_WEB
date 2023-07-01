#include "image_api.h"
#include <dbserver.h>
#include <mediaserver.h>

namespace xggd {
namespace cgi {

void image_mediaserver_set(int id) {

  nlohmann::json setting_config;
  if(id==0)
       setting_config.emplace("TYPE", "image-adjustment-rgb");
  else setting_config.emplace("TYPE", "image-adjustment-ir");
  mediaserver_update_set(id, (char *)setting_config.dump().c_str());

}

void ImageApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {

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
    /*   获取图像增参数配置信息     */
    if (!path_specific_resource.compare("config")) {
      nlohmann::json data_config;
      str = dbserver_media_get((char *)TABLE_IMAGE_ADJUSTMENT);
      data_config = nlohmann::json::parse(str).at("jData").at(0);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);

      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    } else {
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
    }
  }else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    /* 保存配置信息 */
    nlohmann::json system_config = Req.PostObject; /* must be json::object */
    if (!path_specific_resource.compare("config")) {    

      /* Set */
      int id = system_config.at("id");
      dbserver_media_set(TABLE_IMAGE_ADJUSTMENT, (char *)system_config.dump().c_str(), id);
      image_mediaserver_set(id);
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
} // namespace xggds
