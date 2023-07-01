#include "system_api.h"
#include "common.h"
#include <dbserver.h>
#include <fstream>
#include <netserver.h>
#include <sys/statfs.h>
#include <system_manager.h>
#include <mediaserver.h>



namespace xggd {
namespace cgi {


void system_mediaserver_set(std::string type) {

  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(0, (char *)setting_config.dump().c_str());

}


std::string create_fireware_location() {
  int file_id;
  int exist = 1;
  char *str;
  std::ofstream file;
  // Make sure files are not duplicated
  srand((int)time(NULL));
  while (exist) {
    file_id = rand() % 1000;
    file.open("/data/" + file_id);
    if (!file) // open fail
      exist = 0;
    file.close();
  }
  // Create new file
  std::string file_name = "/data/" + std::to_string(file_id);
  std::ofstream new_file(file_name.c_str());
  new_file.close();
  // Get ip address and location
  std::string ipv4_address = ipv4_address_get();
  std::string location = "http://" + ipv4_address +
                         "/cgi-bin/entry.cgi/system/firmware-upgrade?id=" +
                         std::to_string(file_id);

  return location;
}

nlohmann::json is_register_user(nlohmann::json put_info) {
  nlohmann::json res;
  std::string user_name = put_info.at("sUserName");
  std::string password = put_info.at("sPassword");
  char *str;

  int id_mark;
  str = dbserver_system_get((char *)TABLE_SYSTEM_USER);
  minilog_debug("TABLE_SYSTEM_USER str: %s", str);
  nlohmann::json user_info = nlohmann::json::parse(str).at("jData");
  for (auto &x : nlohmann::json::iterator_wrapper(user_info)) {
    id_mark = x.value().at("id");
    std::string db_name = x.value().at("sUserName");
    if (!user_name.compare(db_name)) {
      std::string db_password = x.value().at("sPassword");
      if (!password.compare(db_password)) {
        int auth = x.value().at("iUserLevel");
        res.emplace("status", id_mark);
        res.emplace("auth", auth);
        return res;
      } else {
        res.emplace("status", -1);
        res.emplace("auth", 0);
        return res;
      }
    }
  }
  // |status| - 1 = id for add
  id_mark = -2 - id_mark;
  res.emplace("status", id_mark);
  res.emplace("auth", 0);
  return res;
}



nlohmann::json secret_identify(nlohmann::json put_info) {
  nlohmann::json res;
  std::string secretQuestion = put_info.at("secretQuestion");
  std::string anwser = put_info.at("anwser");
  char *str;

  str = dbserver_system_get((char *)TABLE_SYSTEM_SECRET_QUESTION);
  minilog_debug("TABLE_SYSTEM_USER str: %s", str);
  nlohmann::json user_info = nlohmann::json::parse(str).at("jData");
  for (auto &x : nlohmann::json::iterator_wrapper(user_info)) {
    std::string question_str = x.value().at("secretQuestion");
    if (!secretQuestion.compare(question_str))
    {
      std::string answer_str = x.value().at("anwser");
      if (!anwser.compare(answer_str))
            continue;
      else
        return -1;
    }
  }
  return 0;
}



void create_new_user(nlohmann::json user_info) {
  int user_id = 0;
  char *str;
  str = dbserver_sql((char *)"select * from SystemUser ORDER BY id DESC LIMIT 1",
                     (char *)DBSERVER_SYSTEM_INTERFACE);
  nlohmann::json user_last = nlohmann::json::parse(str).at("jData");
  if (!user_last.empty()) {
    user_id = user_last.at(0).at("id");
    user_id++;
  }
  dbserver_system_set((char *)TABLE_SYSTEM_USER,
                      (char *)user_info.dump().c_str(), user_id);
}

// -1:admin password wrong, >=0:ok, -2:find no item have the same user name
int user_alter_verify(nlohmann::json put_info) {
  char *str;
  int user_id = 0;
//  std::string in_admin_password = put_info.at("sPassword");
//  str = dbserver_sql(
//      (char *)"select sPassword from SystemUser WHERE sUserName='admin'",
//      (char *)DBSERVER_SYSTEM_INTERFACE);
//  nlohmann::json admin_json = nlohmann::json::parse(str).at("jData").at(0);
//  std::string db_admin_password = admin_json.at("sPassword");
//  if (in_admin_password.compare(db_admin_password)) {
//    return -1;
//  }
  std::string in_user_name = put_info.at("UserName");
  std::string cmd =
      "select id from SystemUser WHERE sUserName='" + in_user_name + "'";
  str = dbserver_sql((char *)cmd.c_str(), (char *)DBSERVER_SYSTEM_INTERFACE);
  nlohmann::json new_json = nlohmann::json::parse(str).at("jData");
  if (!new_json.empty()) {
    user_id = new_json.at(0).at("id");
  } else {
    user_id = -2;
  }
  return user_id;
}


void SystemApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  char *str;
  std::string path_api_resource;
  std::string path_specific_resource;
  std::string para_channel;
  nlohmann::json content;
#ifdef ENABLE_JWT
  int user_level = Req.UserLevel;
#endif

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
    if (!path_specific_resource.compare("device-info")) {
      str = dbserver_system_get((char *)TABLE_SYSTEM_DEVICE_INFO);
      nlohmann::json data_config = nlohmann::json::parse(str).at("jData").at(0);
      data_config.erase("id");
      data_config.erase("ideviceID");
      data_config.erase("serialPort");
      data_config.erase("iserialBaud");
      int64_t start_time = std::stoi((std::string)data_config.at("StartTimeInt"));
      int64_t diff_time = data_config.at("iDiffTimeInt");
      int64_t current_time = time(0);
      int64_t running_time = current_time - start_time + diff_time;
      data_config.erase("StartTimeInt");
      data_config.erase("iDiffTimeInt");
      data_config.erase("RunningTime");
      data_config.emplace("RunningTime", std::to_string(running_time)+"s");


      content.emplace("msg","operation successfully");
      content.emplace("code",200);
      content.emplace("data",data_config);
//      content = nlohmann::json::parse(str).at("jData");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
    else if (path_specific_resource.find("para") != std::string::npos) {
      if (para_channel.empty()) {
        Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented,the url is not exist");
      } else {
        content = "{\"auth\":4,\"item\":[{\"auth\":4,\"name\":\"preview\"},{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"delete\"}],\"name\":\"videoRecord\"},{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"delete\"}],\"name\":\"pictureRecord\"}],\"name\":\"download\"},{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":1,\"name\":\"modify\"}],\"name\":\"ListManagement\"},{\"auth\":1,\"name\":\"AddOne\"},{\"auth\":1,\"name\":\"BatchInput\"}],\"name\":\"MemberList\"},{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"modify\"}],\"name\":\"SnapShot\"}],\"name\":\"SnapShot\"},{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"modify\"}],\"name\":\"Control\"}],\"name\":\"Control\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"ParaConfig\"}],\"name\":\"Config\"}],\"name\":\"face\"},{\"auth\":-1,\"item\":[{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"FacePara\"},{\"auth\":1,\"name\":\"ROI\"}],\"name\":\"Config\"}],\"name\":\"face-para\"},{\"auth\":-1,\"item\":[{\"auth\":4,\"item\":[{\"auth\":4,\"item\":[{\"auth\":1,\"name\":\"modify\"}],\"name\":\"MemberList\"},{\"auth\":1,\"name\":\"AddOne\"},{\"auth\":1,\"name\":\"BatchInput\"},{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"modify\"}],\"name\":\"SnapShot\"},{\"auth\":4,\"item\":[{\"auth\":0,\"name\":\"modify\"}],\"name\":\"Control\"}],\"name\":\"Manage\"}],\"name\":\"face-manage\"},{\"auth\":1,\"item\":[{\"auth\":1,\"item\":[{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"basic\"},{\"auth\":1,\"name\":\"time\"}],\"name\":\"Settings\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"upgrade\"},{\"auth\":-1,\"name\":\"log\"}],\"name\":\"Maintain\"},{\"auth\":-1,\"item\":[{\"auth\":-1,\"name\":\"authentication\"},{\"auth\":-1,\"name\":\"ipAddrFilter\"},{\"auth\":-1,\"name\":\"securityService\"}],\"name\":\"Security\"},{\"auth\":0,\"name\":\"User\"}],\"name\":\"System\"},{\"auth\":1,\"item\":[{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"TCPIP\"},{\"auth\":-1,\"name\":\"DDNS\"},{\"auth\":-1,\"name\":\"PPPoE\"},{\"auth\":1,\"name\":\"Port\"},{\"auth\":-1,\"name\":\"uPnP\"}],\"name\":\"Basic\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"Wi-Fi\"},{\"auth\":-1,\"name\":\"SMTP\"},{\"auth\":-1,\"name\":\"FTP\"},{\"auth\":-1,\"name\":\"eMail\"},{\"auth\":-1,\"name\":\"Cloud\"},{\"auth\":-1,\"name\":\"Protocol\"},{\"auth\":-1,\"name\":\"QoS\"},{\"auth\":-1,\"name\":\"Https\"}],\"name\":\"Advanced\"}],\"name\":\"Network\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"Encoder\"},{\"auth\":1,\"name\":\"AdvancedEncoder\"},{\"auth\":1,\"name\":\"ROI\"},{\"auth\":1,\"name\":\"RegionCrop\"}],\"name\":\"Video\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"AudioParam\"}],\"name\":\"Audio\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"DisplaySettings\"},{\"auth\":1,\"name\":\"OSDSettings\"},{\"auth\":1,\"name\":\"PrivacyCover\"},{\"auth\":1,\"name\":\"PictureMask\"}],\"name\":\"Image\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"MotionDetect\"},{\"auth\":1,\"name\":\"IntrusionDetection\"},{\"auth\":-1,\"name\":\"AlarmInput\"},{\"auth\":-1,\"name\":\"AlarmOutput\"},{\"auth\":-1,\"name\":\"Abnormal\"}],\"name\":\"Event\"},{\"auth\":1,\"item\":[{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"VideoPlan\"},{\"auth\":1,\"name\":\"ScreenshotPlan\"},{\"auth\":1,\"name\":\"ScreenshotPara\"}],\"name\":\"PlanSettings\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"HardDiskManagement\"},{\"auth\":-1,\"name\":\"NAS\"},{\"auth\":-1,\"name\":\"CloudStorage\"}],\"name\":\"StorageManage\"}],\"name\":\"Storage\"},{\"auth\":1,\"item\":[{\"auth\":1,\"name\":\"MarkCover\"},{\"auth\":-1,\"name\":\"MaskArea\"},{\"auth\":-1,\"name\":\"RuleSettings\"},{\"auth\":-1,\"name\":\"AdvancedCFG\"}],\"name\":\"Intel\"},{\"auth\":-1,\"item\":[{\"auth\":-1,\"name\":\"GateConfig\"},{\"auth\":-1,\"name\":\"ScreenConfig\"}],\"name\":\"Peripherals\"}],\"name\":\"config\"},{\"auth\":4,\"name\":\"about\"}],\"name\":\"header\"}"_json;

        if (!content.empty()) {
          Resp.setHeader(HttpStatus::kOk, "OK\n\n\n");
          Resp.setApiData(content);
        } else {
          Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented,the url is not exist");
        }
      }
    }
    else if (!path_specific_resource.compare("login")) {
      /* path is system/login */
      nlohmann::json data_config;
      str = dbserver_sql((char *)"SELECT id, sUserName, iUserLevel FROM SystemUser",
                         (char *)DBSERVER_SYSTEM_INTERFACE);
      data_config = nlohmann::json::parse(str).at("jData");
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }  else if (!path_specific_resource.compare("secret-question")) {
      /* path is system/login */
      nlohmann::json data_config;
      str = dbserver_system_get((char *)TABLE_SYSTEM_SECRET_QUESTION);
      data_config = nlohmann::json::parse(str).at("jData");
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }else {
      Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented,the url is not exist");
    }
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    nlohmann::json system_config = Req.PostObject; /* must be json::object */
    if (!path_specific_resource.compare("device-info")) {
#ifdef ENABLE_JWT
        if (user_level > 1) {
           Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
           return;
        }
#endif
        /* Set */
        dbserver_system_set((char *)TABLE_SYSTEM_DEVICE_INFO,
                            (char *)system_config.dump().c_str(), 0);

        system_mediaserver_set("system");
        content.emplace("msg","setting successfully");
        content.emplace("code",200);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
    }  else if (!path_specific_resource.compare("secret-question")) {


       for(int id =0; id<3; id++) {
        nlohmann::json data_config = system_config[id];
        dbserver_system_set((char *)TABLE_SYSTEM_SECRET_QUESTION,
                                    (char *)data_config.dump().c_str(), id);

        }
        content.emplace("msg","setting successfully");
        content.emplace("code",200);
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
    } else if (!path_specific_resource.compare("reset-passwd")) {

      int res = 0;
      for(int id =0; id<3; id++) {
        nlohmann::json data_config = system_config.at("secret").at(id);
        res = secret_identify(data_config);
        if (res<0)
            break;
      }
      if(res<0)
      {
        content.emplace("msg","the secret anwser is not correct");
        content.emplace("code",400);
      }else
      {
        std::string new_pw =system_config.at("newPassword");;
        nlohmann::json modify_json;
        modify_json.emplace("sPassword", new_pw);
        dbserver_system_set((char *)TABLE_SYSTEM_USER,
                            (char *)modify_json.dump().c_str(), 0);
        content.emplace("msg","reset password successfully");
        content.emplace("code",200);
      }

      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }else if (!path_specific_resource.compare("reboot")) {   //重启
      system_mediaserver_set("system_reboot");
      netserver_reboot();
      nlohmann::json content;
      content.emplace("code", 200);
      content.emplace("msg", "the system will reboot now, relogin after 2-4 minutes");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);

    } else if (!path_specific_resource.compare("factory-reset")) {   //恢复出厂设置
#ifdef ENABLE_JWT
      if (user_level > 1) {
        Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
        return;
      }
#endif
//      system_factory_reset();
    }

    else if (path_specific_resource.find("login") != std::string::npos) {    //登陆
       nlohmann::json data;
       nlohmann::json data_config;
      if (para_channel.empty()) { /* path is login */
          std::string username = system_config.at("sUserName");
          std::string sPassword = system_config.at("sPassword");
          data = is_register_user(system_config);
          int auth = data.at("auth");
          int status = data.at("status");

        if (status < 0)
        {
          content.emplace("msg", "username or password is wrong ");
          content.emplace("code", 402);
        }else
        {
          int level=1;
          if(!username.compare("admin"))
                level=0;

          data_config.emplace("iUserLevel",level);
          content.emplace("msg", "login successfully ");
          content.emplace("code", 200);
          content.emplace("data",data_config);


        std::string token;
        long expire_time = EXPIRE_SECONDS;
        if (status >= 0) {

              for (auto p : Req.Params) {
            if (p.Key == "expire") {
              std::string expire_stirng = p.Value;
              if (!expire_stirng.compare("day")) {
                expire_time = 86400;
              } else if (!expire_stirng.compare("week")) {
                expire_time = 604800;
              } else if (!expire_stirng.compare("month")) {
                expire_time = 2592000;
              }
              break;
            }
          }
//           minilog_debug("expire_time is :%d\n",expire_time);
          token = jwt_token_get(username, auth, expire_time);
          minilog_debug("token is :%s\n",token.c_str());
          Resp.setCookie("token", token, expire_time);
        }
        content.at("data").emplace("token",token );

        str = dbserver_ptz_get((char *)TABLE_PTZ_STATUS_INFO);
        nlohmann::json data_temp= nlohmann::json::parse(str).at("jData").at(0);
        content.at("data").emplace("iptzDirection",(int)data_temp.at("iptzDirection") );

        }
        Resp.setHeader(HttpStatus::kOk, "OK\n\n\n");
        Resp.setApiData(content);
      } else if (!para_channel.compare("modify")) { /* path is login/modify */
#ifdef ENABLE_JWT
        if (user_level > 0) {
          Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
          return;
        }
#endif
        int res = user_alter_verify(system_config);
        if (res >= 0) {
          std::string new_name = system_config.at("newUserName");
          std::string new_pw = system_config.at("newPassword");
//          int user_level = system_config.at("iUserLevel");
          nlohmann::json modify_json;
          modify_json.emplace("sPassword", new_pw);
          modify_json.emplace("sUserName", new_name);
          dbserver_system_set((char *)TABLE_SYSTEM_USER,
                              (char *)modify_json.dump().c_str(), res);
          content.emplace("msg", "modify successfully ");
          content.emplace("code", 200);
          system_mediaserver_set("user");

        }else{
          content.emplace("code", 402);
          content.emplace("msg", "username dont exsit");
        }
        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);

      } else if (!para_channel.compare("add")) { /* path is login/add */    //增加用户
#ifdef ENABLE_JWT
        if (user_level > 0) {
          Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
          return;
        }
#endif
        int res = user_alter_verify(system_config);
        if (res == -2) {
          std::string new_name = system_config.at("UserName");
          std::string new_pw = system_config.at("Password");
          nlohmann::json add_json;
          add_json.emplace("sUserName", new_name);
          add_json.emplace("sPassword", new_pw);
          create_new_user(add_json);

          content.emplace("code", 200);
          content.emplace("msg","add newusername  successfully");
          system_mediaserver_set("user");
        }else if( res >0 )
        {
          nlohmann::json data_config;
          data_config.emplace("userid",res);
          content.emplace("msg","the username already exsit!");
          content.emplace("code",402);
          content.emplace("data",data_config);
        }

        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      } else if (!para_channel.compare("delete")) { /* path is login/delete */
#ifdef ENABLE_JWT
        if (user_level > 0) {
          Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
          return;
        }
#endif
        int res = user_alter_verify(system_config);
        if (res > 0) {
          dbserver_system_user_delete(res);
          content.emplace("code", 200);
          content.emplace("msg", "delete username successfully");
          system_mediaserver_set("user");
        }else
        {
          content.emplace("code", 402);
          content.emplace("msg", "username dont exsit");
        }

        Resp.setHeader(HttpStatus::kOk, "OK");
        Resp.setApiData(content);
      } else {
        Resp.setErrorResponse(HttpStatus::kBadRequest, "Not Implemented,the url is not exist");
      }
    } else {
      Resp.setErrorResponse(HttpStatus::kBadRequest, "Not Implemented,the url is not exist,the url is not exist");
    }
  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented,the url is not exist");
  }
}

} // namespace cgi
} // namespace xggd
