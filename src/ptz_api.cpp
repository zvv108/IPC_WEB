
#include "ptz_api.h"
#include "common.h"
#include <dbserver.h>
#include <fstream>
#include <netserver.h>
#include <sys/statfs.h>
#include <system_manager.h>
#include <mediaserver.h>

namespace xggd{
namespace cgi{

void ptz_id_mediaserver_set(int id, std::string type) {

    nlohmann::json setting_config;
    setting_config.emplace("TYPE", type);
    mediaserver_update_set(id, (char *)setting_config.dump().c_str());

  }


void ptz_mediaserver_set(std::string type) {


  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}


void PTZApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
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
    nlohmann::json data_config;
    if (!path_specific_resource.compare("aux-info")){
      str = dbserver_ptz_get((char *)TABLE_PTZ_AUX_INFO);
      data_config= nlohmann::json::parse(str).at("jData").at(0);
//      data_config.at(1).erase("iwiper");
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    } else if (!path_specific_resource.compare("status")) {
      str = dbserver_ptz_get((char *)TABLE_PTZ_STATUS_INFO);
      data_config= nlohmann::json::parse(str).at("jData").at(0);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }else if(!path_specific_resource.compare("position")){
      str = dbserver_ptz_get((char *)TABLE_PTZ_POSITION_INFO);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
      data_config.erase(255);

      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }else if(!path_specific_resource.compare("path")){
      str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData");

            for(int id=0; id<32; id++)
            {
                std::string temp = data_config.at(id).at("cruises");
                nlohmann::json cruises = nlohmann::json::parse(temp);
                data_config.at(id).erase("cruises");
                data_config.at(id).emplace("cruises",cruises);
              }
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);


  }else if(!path_specific_resource.compare("path-status")){

      str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_CONDUCT);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(0);
      if(data_config.at("istatus")== 0)
      {
        content.emplace("code", 200);
        content.emplace("msg", "no path is in executing");

        nlohmann::json cruise_id;
        cruise_id.emplace("iCruiseID", -1);
        content.emplace("data",  cruise_id);
      }else{
        data_config.erase("id");
        data_config.erase("istatus");
        content.emplace("code", 200);
        content.emplace("msg", "there is a path in executing");
        content.emplace("data",  data_config);
      }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

  }else{
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    /* 保存配置信息 */
    nlohmann::json system_config = Req.PostObject; /* must be json::object */
    if (!path_specific_resource.compare("stride")) {    //步长
#ifdef ENABLE_JWT
//      if (user_level > 1) {
//        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
//        return;
//      }
#endif
      /* Set */
      dbserver_ptz_set((char *)TABLE_PTZ_STATUS_INFO,
                       (char *)system_config.dump().c_str(),0);
      ptz_mediaserver_set("stride");

      content.emplace("code", 200);
      content.emplace("msg", "setting successfully");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }else if(path_specific_resource==""){
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
    }else if(!path_specific_resource.compare("position-add")){

        std::string positon_name = system_config.at("positionName");
        int id = system_config.at("id");
        ptz_id_mediaserver_set(id, "position-add");

        dbserver_ptz_set((char *)TABLE_PTZ_POSITION_INFO,
                         (char *)system_config.dump().c_str(), id);

        content.emplace("code", 200);
        content.emplace("msg", "setting successfully");
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);

     }
    else if(!path_specific_resource.compare("position-delete")){
        system_config.emplace("istatus",0); //删除得把状态置OFF
        system_config.emplace("iudAngle",0);
        system_config.emplace("ilrAngle",0);
        system_config.emplace("irgbFocaLength",1);
        system_config.emplace("iirFocaLength",1);

        int id = system_config.at("id");
        str = dbserver_ptz_get((char *)TABLE_PTZ_POSITION_INFO);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
        if(data_config.at(id).at("istatus")==0)
        {
          content.emplace("code", 402);
          content.emplace("msg", "this positon has not been setting ");
        }else{
          dbserver_ptz_set((char *)TABLE_PTZ_POSITION_INFO, (char *)system_config.dump().c_str(),id);
          ptz_id_mediaserver_set(id, "position-delete");


          // 删除路径中含有该预置位的点
          /*
          char * cruise_str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
          nlohmann::json cruise_content = nlohmann::json::parse(cruise_str).at("jData");
          for(int i=0; i<32; i++)
          {
            if(cruise_content.at(i).at("istatus")==0)
                continue;
            else
            {
                std::string cruises = cruise_content.at(i).at("cruises");
                nlohmann::json cruise_temp = nlohmann::json::parse(cruises.c_str());
                for(int j=0; j<cruise_temp.size(); j++)
                {
                   if((int)cruise_temp.at(j).at("positionID") == id)
                  {
                     cruise_temp.erase(j);
                     j--;
                   }
                }
                nlohmann::json new_config;
                if(cruise_temp.size() == 0)
                {
                  new_config.emplace("istatus",0);
                  new_config.emplace("cruises","[]");
                  dbserver_ptz_set((char *)TABLE_PTZ_CRUISE_INFO, (char *)new_config.dump().c_str(),i);
                }else
                {
                  new_config.emplace("cruises",cruise_temp);
                  dbserver_ptz_set((char *)TABLE_PTZ_CRUISE_INFO, (char *)new_config.dump().c_str(),i);
                }
                continue;
            }

          }
*/


          content.emplace("code", 200);
          content.emplace("msg", "delete positon successfully");
        }

        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);

    }
    else if(!path_specific_resource.compare("position-name")){

      int id = system_config.at("id");
        dbserver_ptz_set((char *)TABLE_PTZ_POSITION_INFO, (char *)system_config.dump().c_str(),id);
        content.emplace("code", 200);
        content.emplace("msg", "change position name successfully");
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      }
    else if(!path_specific_resource.compare("position-conduct")) {

        int id = system_config.at("id");
        str = dbserver_ptz_get((char *)TABLE_PTZ_POSITION_INFO);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
        if (data_config.at(id).at("istatus") == 0) {
          content.emplace("code", 402);
          content.emplace("msg", "this positon has not been setting ");
        } else {
          ptz_id_mediaserver_set(id, "position-conduct");
          content.emplace("code", 200);
          content.emplace("msg", "operation successfully");
        }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }

    else if(!path_specific_resource.compare("path-add")){
       int id = system_config.at("id");
       nlohmann::json cruise = system_config.at("cruises");
        system_config.emplace("istatus",1); //删除得把状态置OFF

        dbserver_ptz_set((char *)TABLE_PTZ_CRUISE_INFO,
                         (char *)system_config.dump().c_str(),id);
        ptz_mediaserver_set("path");

        content.emplace("code", 200);
        content.emplace("msg", "setting successfully");
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);

    } 
    else if(!path_specific_resource.compare("path-change")){

        int id = system_config.at("id");
        nlohmann::json cruise = system_config.at("cruises");
         system_config.emplace("istatus",1); //删除得把状态置OFF

         str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
         nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
         if(data_config.at(id).at("istatus")== 0)
         {
           content.emplace("code", 402);
           content.emplace("msg", "this path has not been setting ");
         }else{
             dbserver_ptz_set((char *)TABLE_PTZ_CRUISE_INFO,
                              (char *)system_config.dump().c_str(),id);
             ptz_mediaserver_set("path");
             content.emplace("code", 200);
             content.emplace("msg", "change successfully");
         }
             Resp.setHeader(HttpStatus::kOk, "OK");
             Resp.setApiData(content);


    }
    else if(!path_specific_resource.compare("path-delete")){
         int id = system_config.at("id");
         system_config.emplace("istatus",0); //删除得把状态置OFF
         system_config.emplace("cruises","[]");   //状态初始化
         str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
         nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
         if(data_config.at(id).at("istatus")== 0)
         {
           content.emplace("code", 402);
           content.emplace("msg", "this path has not been setting ");
         }else{
           dbserver_ptz_set((char *)TABLE_PTZ_CRUISE_INFO, (char *)system_config.dump().c_str(),id);
           ptz_mediaserver_set("path");
           content.emplace("code", 200);
           content.emplace("msg", "delete path successfully");
         }


           Resp.setHeader(HttpStatus::kOk, "OK");
           Resp.setApiData(content);


    }
    else if(!path_specific_resource.compare("path-start")){
      int id = system_config.at("id");
      str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
      if(data_config.at(id).at("istatus")== 0)
      {
        content.emplace("code", 402);
        content.emplace("msg", "this path has not been setting ");
      }else{
        ptz_id_mediaserver_set(id, "path-start");
        content.emplace("code", 200);
        content.emplace("msg", "start path successfully");
      }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }
    else if(!path_specific_resource.compare("path-stop")){
      int id = system_config.at("id");
      str = dbserver_ptz_get((char *)TABLE_PTZ_CRUISE_INFO);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
      if(data_config.at(id).at("istatus")== 0)
      {
        content.emplace("code", 402);
        content.emplace("msg", "this path has not been setting ");
      }else{
        ptz_id_mediaserver_set(id, "path-stop");
        content.emplace("code", 200);
        content.emplace("msg", "stop path successfully");
      }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }
    
    else if(!path_specific_resource.compare("aux-info")){
      int id = system_config.at("id");
      std::string aux_str = system_config.at("aux-type");
      if(aux_str == "wiper-on")
      {
        system_config.erase("aux-type");
        system_config.emplace("iwiperStatus", 1);
      }
      else if(aux_str == "wiper-down")
      {
        system_config.erase("aux-type");
        system_config.emplace("iwiperStatus", 0);
      }
      else if(aux_str == "defogging-on")
      {
        system_config.erase("aux-type");
        system_config.emplace("iDefogStatus", 1);
      }
      else if(aux_str == "defogging-down")
      {
        system_config.erase("aux-type");
        system_config.emplace("iDefogStatus", 0);
      }
      dbserver_ptz_set((char *)TABLE_PTZ_AUX_INFO, (char *)system_config.dump().c_str(),id);
      ptz_id_mediaserver_set(id, aux_str);
      content.emplace("code", 200);
      content.emplace("msg", "setting successfully");

      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if(!path_specific_resource.compare("focus"))
    {
      int id = system_config.at("id");
      std::string control_type = system_config.at("focusType");
      ptz_id_mediaserver_set(id, control_type);
      content.emplace("code", 200);
      content.emplace("msg", "setting  successfully");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if(!path_specific_resource.compare("control"))
    {   //PTZ up down left right ... control
      std::string control_type = system_config.at("controlType");
      ptz_mediaserver_set(control_type);
      content.emplace("code", 200);
      content.emplace("msg", "setting  successfully");
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
