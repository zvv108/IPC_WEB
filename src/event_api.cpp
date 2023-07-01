#include "event_api.h"
#include "common.h"
#include <dbserver.h>
#include <fstream>
#include <mediaserver.h>
#include <netserver.h>
#include <time.h>
#include <unistd.h>


#define   RGB_INVADE_MODEL_PATH "/oem/usr/share/mediaserver/model_trt.engine"
#define   IR_INVADE_MODEL_PATH "/oem/usr/share/mediaserver/model_trt.engine"
#define   IR_TEMP_MODEL_PATH "/usr/local/lib/libIRAnalysis.so"

namespace xggd {
namespace cgi {

void ftp_mediaserver_set(std::string type) {

  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}

nlohmann::json regional_invasion_get() {
  char *str;
  nlohmann::json ret;
  nlohmann::json regional_invasion;
  nlohmann::json Sensitivity;

  str = dbserver_event_get((char *)TABLE_REGIONAL_INVASION);
  regional_invasion = nlohmann::json::parse(str).at("jData").at(0);
  regional_invasion.erase("id");
  std::string temp = regional_invasion.at("regionalInvasion");
  nlohmann::json regionalInvasion = nlohmann::json::parse(temp);
  regional_invasion.erase("regionalInvasion");
//  str = dbserver_media_get((char *)TABLE_NORMALIZED_SCREEN_SIZE);
//  normalized_screen_size = nlohmann::json::parse(str).at("jData").at(0);
//  normalized_screen_size.erase("id");
  regional_invasion.emplace("regionalInvasion", regionalInvasion);

  return ret;
}

nlohmann::json regional_tempture_get() {
  char *str;
  nlohmann::json ret;
  nlohmann::json regional_tempture;
//  nlohmann::json ir_screen_size;

  str = dbserver_event_get((char *)TABLE_TEMPERATURE_ALARM);
  regional_tempture = nlohmann::json::parse(str).at("jData").at(0);
  regional_tempture.erase("id");

  ret.emplace("temperatureAlarm", regional_tempture);

  return ret;
}


void alarm_mediaserver_set(std::string type) {
  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}

void alarm_mediaserver_id_set(int id, std::string type) {
  nlohmann::json setting_config;
  if(id==0)
      setting_config.emplace("TYPE", type+"-rgb");
  else
      setting_config.emplace("TYPE", type+"-ir");

  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}


void EventApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  std::string path_api_resource;
  std::string path_specific_resource;
  std::string path_channel_resource;
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
    path_specific_resource =
        path_api_resource.substr(pos_first + 1, path_api_resource.size());
    pos_first = path_specific_resource.find_first_of("/");
    if (pos_first != -1) {
      path_channel_resource = path_specific_resource.substr(
          pos_first + 1, path_specific_resource.size());
    }
  }
  if (Req.Method == "GET") {
      if (!path_specific_resource.compare("regional-invasion")) {      // get regional invasion info

        str = dbserver_event_get((char *)TABLE_REGIONAL_INVASION);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
         minilog_debug("data_config is %s", data_config.dump().c_str());

        for(int id=0; id<2; id++)
        {
            std::string temp = data_config.at(id).at("regionalInvasion");
            nlohmann::json regionalInvasion = nlohmann::json::parse(temp);
            data_config.at(id).erase("regionalInvasion");
            data_config.at(id).emplace("regionalInvasion", regionalInvasion);
        }
//        std::list<nlohmann::json> data_list;
//        data_list.push_back(data_config);
        content.emplace("code",200);
        content.emplace("msg","operation successfully");
        content.emplace("data",data_config);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      }
      else if (!path_specific_resource.compare("tempture-data"))
      {
        str = dbserver_event_get((char *)TABLE_TEMPERATURE_DATA);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(0);
        std::string temp = data_config.at("regional");
        nlohmann::json regional = nlohmann::json::parse(temp);
        data_config.erase("regional");
        data_config.emplace("regional",regional);
        data_config.erase("id");

        content.emplace("code",200);
        content.emplace("msg","operation successfully");
        content.emplace("data",data_config);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);

      }
      else if (!path_specific_resource.compare("tempture-alarm")) {      // get regional invasion info

        str = dbserver_event_get((char *)TABLE_TEMPERATURE_ALARM);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
        char *str_rule =  dbserver_event_get((char *)TABLE_ALARM_RULE);
        nlohmann::json data_config_rule = nlohmann::json::parse(str_rule).at("jData");
        data_config.erase(32);
        int count = 32;
        for(int id=0; id<count; id++)
        {

            std::string temp = data_config.at(id).at("regional");
            nlohmann::json regional = nlohmann::json::parse(temp);
            data_config.at(id).erase("regional");
            data_config.at(id).emplace("regional",regional);
            data_config_rule.at(id).erase("id");
            data_config.at(id).emplace("sAlarmRule", data_config_rule.at(id));
            continue;

        }

        content.emplace("code",200);
        content.emplace("msg","operation successfully");
        content.emplace("data",data_config);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      }  else if (!path_specific_resource.compare("ftp-config")) {      // get regional invasion info

        str = dbserver_event_get((char *)TABLE_FTP_CONFIG);
        nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(0);
        data_config.erase("id");
        content.emplace("code",200);
        content.emplace("msg","operation successfully");
        content.emplace("data",data_config);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      }
    else if (!path_specific_resource.compare("schedules")) {

          str = dbserver_event_get((char *)TABLE_EVENT_SCHEDULES);
          nlohmann::json data_config = nlohmann::json::parse(str).at("jData");
          for(int id=0; id<2; id++)
          {
            std::string temp = data_config.at(id).at("sSchedulesJson");
            nlohmann::json sSchedulesJson = nlohmann::json::parse(temp);
            data_config.at(id).erase("sSchedulesJson");
            data_config.at(id).emplace("sSchedulesJson",sSchedulesJson);
          }
          content.emplace("code",200);
          content.emplace("msg","operation successfully");
          content.emplace("data",data_config);

          Resp.setHeader(HttpStatus::kOk, "OK");
          Resp.setApiData(content);

    }
    else
    {
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
    }
  }
  else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    nlohmann::json event_config = Req.PostObject; /* must be json::object */
    char *config = new char[Req.ContentLength + 1];
    strncpy(config, event_config.dump().c_str(), Req.ContentLength);
    config[Req.ContentLength] = '\0';
    if (!path_specific_resource.compare("schedules"))
    {
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif

          int id = event_config.at("id");
          dbserver_event_set((char *)TABLE_EVENT_SCHEDULES, config, id);
          alarm_mediaserver_id_set(id, "schedules");
          content.emplace("code",200);
          content.emplace("msg","setting successfully");

          Resp.setHeader(HttpStatus::kOk, "OK");
          Resp.setApiData(content);

    }

    else if (!path_specific_resource.compare("regional-invasion")) {
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
      int id =event_config.at("id");
      // set
      dbserver_event_set((char *)TABLE_REGIONAL_INVASION, (char *)event_config.dump().c_str(), id);
      content.emplace("code",200);
      content.emplace("msg","setting successfully");
      alarm_mediaserver_id_set(id, "regionalInvasion");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }
    else if (!path_specific_resource.compare("tempture-data"))
    {
      nlohmann::json region_sub_temp;
      if((int)event_config.at("iEnabled") == 0)
      {
           dbserver_event_set((char *)TABLE_TEMPERATURE_DATA, (char *)event_config.dump().c_str(), 0);
           alarm_mediaserver_set("temperatureData");
           content.emplace("code",200);
           content.emplace("msg","setting successfully");
      }else
      {
          str = dbserver_event_get((char *)TABLE_TEMPERATURE_DATA);
          nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(0);
          std::string temp = data_config.at("regional");
          nlohmann::json regional = nlohmann::json::parse(temp);

          int max_point_num = data_config.at("iMaxPointNum");
          int region_id = regional.size();
          if(region_id < max_point_num)
          {
            region_sub_temp.emplace("id", region_id);
            region_sub_temp.emplace("subRegional", event_config.at("regional"));
            regional.push_back(region_sub_temp);

            data_config.erase("regional");
            data_config.erase("iEnabled");
            data_config.erase("iType");
            data_config.emplace("regional",regional);
            data_config.emplace("iEnabled", event_config.at("iEnabled"));
            data_config.emplace("iType", event_config.at("iType"));

            dbserver_event_set((char *)TABLE_TEMPERATURE_DATA, (char *)data_config.dump().c_str(), 0);
            alarm_mediaserver_set("temperatureData");
            content.emplace("code",200);
            content.emplace("msg","setting successfully");
          }else{

            content.emplace("code",200);
            content.emplace("msg","the drawn area >5, please empty and redraw");
          }
      }


      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    }else if (!path_specific_resource.compare("tempture-alarm")) {
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
      int id =event_config.at("id");
      nlohmann::json alarm_rule_config = event_config.at("sAlarmRule");
      event_config.erase("sAlarmRule");
      dbserver_event_set((char *)TABLE_TEMPERATURE_ALARM, (char *)event_config.dump().c_str(), id);
      dbserver_event_set((char *)TABLE_ALARM_RULE, (char *)alarm_rule_config.dump().c_str(), id);

      content.emplace("code",200);
      content.emplace("msg","setting successfully");
      alarm_mediaserver_set("temperatureAlarm");

      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    } else if (!path_specific_resource.compare("ftp-config")) {     
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
      dbserver_event_set((char *)TABLE_FTP_CONFIG, (char *)event_config.dump().c_str(), 0);
      content.emplace("code",200);
      content.emplace("msg","setting successfully");
      ftp_mediaserver_set("ftp-config");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if (!path_specific_resource.compare("ftptest")) {   
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
      nlohmann::json  temp_data;
      dbserver_event_set((char *)TABLE_FTP_CONFIG, (char *)event_config.dump().c_str(), 1);
      ftp_mediaserver_set("ftptest");
      str = dbserver_event_get((char *)TABLE_FTP_CONFIG);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(1);
      if(data_config.at("iEnabled") == 1)
      {
        temp_data.emplace("iStatus", 1);
        content.emplace("code",200);
        content.emplace("data",temp_data);
        content.emplace("msg"," Connect to the test server success !");
      }else
      {
        temp_data.emplace("iStatus", 0);
        content.emplace("code",200);
        content.emplace("data",temp_data);
        content.emplace("msg"," Connect to the test server failed !");
      }
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if (!path_specific_resource.compare("update_model")) {
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
      int end_position = 0;
      // overwrite
      std::string type = "0";
      std::string model_path;
      if (!Req.Params.empty()) {
        for (auto p : Req.Params) {
          if (p.Key == "model_type") {
              type = p.Value;
              break;
          }
        }
        if(type == "00")
          model_path= RGB_INVADE_MODEL_PATH;
        else if(type == "10")
          model_path= IR_INVADE_MODEL_PATH;
        else if(type == "11")
          model_path= IR_TEMP_MODEL_PATH;
        else
          model_path = "/home/xg/Documents/temp/temp_model.rgb";
        std::ofstream model_file(model_path, std::ofstream::out);
        model_file << Req.Files.at(0).getData();
        end_position = model_file.tellp();
        model_file.close();
        if (end_position == Req.Files.at(0).getDataLength()) {
          nlohmann::json content;
          content.emplace("code",200);
          content.emplace("msg","upload model successfully");
          Resp.setHeader(HttpStatus::kOk, "OK");
          Resp.setApiData(content);
        } else {
          Resp.setErrorResponse(HttpStatus::kBadRequest, "model file upload failed!");
        }

      } else {
        Resp.setErrorResponse(HttpStatus::kBadRequest, "Please identify a model type");
      }

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
