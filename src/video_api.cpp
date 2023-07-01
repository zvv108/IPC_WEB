#include "video_api.h"
#include <dbserver.h>
#include <mediaserver.h>

namespace xggd {
namespace cgi {

void video_setting_item_set(nlohmann::json config, int id) {

    nlohmann::json setting_config;
    if(id == 0)
       setting_config.emplace("TYPE", "video_rgb");
    else if(id==1)
       setting_config.emplace("TYPE", "video_rgb_sub");
    else if(id==2)
       setting_config.emplace("TYPE", "video_ir");
    else
       minilog_debug("video type is error!");

    mediaserver_update_set(id, (char *)setting_config.dump().c_str());
}



void VideoApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  std::string path_api_resource;
  std::string path_stream_resource;
  std::string path_specific_resource;
  std::string path_function_resource;
  char *str;
  nlohmann::json content;
#ifdef ENABLE_JWT
  int user_level = Req.UserLevel;
#endif

  // Get Path Information
  int pos_first = Req.PathInfo.find_first_of("/");
  path_api_resource = Req.PathInfo.substr(pos_first + 1, Req.PathInfo.size());
  pos_first = path_api_resource.find_first_of("/");
  if (pos_first != -1) {
    path_stream_resource =
        path_api_resource.substr(pos_first + 1, path_api_resource.size());
    pos_first = path_stream_resource.find_first_of("/");
    if (pos_first != -1) {
      path_specific_resource = path_stream_resource.substr(
          pos_first + 1, path_stream_resource.size());
      pos_first = path_specific_resource.find_first_of("/");
      if (pos_first != -1) {
        path_function_resource = path_specific_resource.substr(
            pos_first + 1, path_specific_resource.size());
      }
    }
  }

  if (Req.Method == "GET") {
    str = dbserver_video_get();
   nlohmann::json video_all_config = nlohmann::json::parse(str).at("jData");
   nlohmann::json video_current_config_rgb;
   nlohmann::json video_current_config_ir;
   nlohmann::json config_video;
   nlohmann::json data_config;
     if (path_stream_resource.empty()) { // path is video
       data_config = video_all_config;

       for(int id=0; id<3; id++)
       {
         std::string temp = data_config.at(id).at("sResolutionList");
         nlohmann::json sResolutionList = nlohmann::json::parse(temp);
         data_config.at(id).erase("sResolutionList");
         data_config.at(id).emplace("sResolutionList",sResolutionList);
       }

       content.emplace("code",200);
       content.emplace("msg","operation successfully");
       content.emplace("data",data_config);

       Resp.setHeader(HttpStatus::kOk, "OK");
       Resp.setApiData(content);
    }
    else {
      if (path_specific_resource.empty()) { // path example is video/0
        int id = stoi(path_stream_resource);
        video_current_config_rgb = video_all_config.at(0);
        video_current_config_ir = video_all_config.at(1);

        config_video.emplace("rgb_config", video_current_config_rgb);
        config_video.emplace("ir_config", video_current_config_ir);

//        minilog_debug("video_current_config %s\n",video_current_config);
        content = config_video;
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      } else {
        Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
      }
    }
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    nlohmann::json video_config = Req.PostObject; // must be json::object
    int id = video_config.at("id");
    if (path_stream_resource.empty()) { // path is video
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
        dbserver_video_set_id((char *)video_config.dump().c_str(), id);
        video_setting_item_set(video_config, id);

        content.emplace("code",200);
        content.emplace("msg","setting successfully");
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      }
    else {
        Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
      }
    }
  else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}

} // namespace cgi
} // namespace xggd
