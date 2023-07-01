#include "osd_api.h"
#include <dbserver.h>
#include "common.h"
#include <fstream>
#include <mediaserver.h>

namespace xggd {
namespace cgi {

void osd_mediaserver_set(int id, std::string type) {

  nlohmann::json setting_config;
  setting_config.emplace("TYPE", type);
  mediaserver_update_set(id, (char *)setting_config.dump().c_str());

}

nlohmann::json osd_overlays_get( int id) {
  char *str;
  nlohmann::json osd_overlays;
  nlohmann::json osd_data;
  nlohmann::json db_osd;
  nlohmann::json normalized_screen_size;
  if(id == 0)
  {
    str = dbserver_media_get((char *)TABLE_NORMALIZED_SCREEN_SIZE);
    normalized_screen_size = nlohmann::json::parse(str).at("jData").at(0);
    normalized_screen_size.erase("id");

    str = dbserver_osd_get();
    osd_data = nlohmann::json::parse(str);
  }else{
    str = dbserver_media_get((char *)TABLE_IR_SCREEN_SIZE);
    normalized_screen_size = nlohmann::json::parse(str).at("jData").at(0);
    normalized_screen_size.erase("id");

    str = dbserver_osd_ir_get();
    osd_data = nlohmann::json::parse(str);
  }


  // channel_name
  db_osd = osd_data.at(0);
  nlohmann::json channel_name = db_osd;
  for (auto &x : nlohmann::json::iterator_wrapper(db_osd)) {
    if ((x.key() != "iPositionX") && (x.key() != "iPositionY") &&
        (x.key() != "iEnabled") && (x.key() != "sDisplayText")) {
      channel_name.erase(x.key());
    }
  }
  channel_name.emplace("sChannelName", channel_name.at("sDisplayText"));
  channel_name.emplace("iChannelNameOverlayEnabled",
                       channel_name.at("iEnabled"));
  channel_name.erase("sDisplayText");
  channel_name.erase("iEnabled");
  // date_time
  db_osd = osd_data.at(1);
  nlohmann::json date_time = db_osd;
  for (auto &x : nlohmann::json::iterator_wrapper(db_osd)) {
    if ((x.key() != "iPositionX") && (x.key() != "iPositionY") &&
        (x.key() != "iEnabled") && (x.key() != "sDateStyle") &&
        (x.key() != "sTimeStyle") && (x.key() != "iDisplayWeekEnabled")) {
      date_time.erase(x.key());
    }
  }
  date_time.emplace("iDateTimeOverlayEnabled", date_time.at("iEnabled"));
  date_time.erase("iEnabled");
  // character
  nlohmann::json all_character;
  for (int i = 2; i < 6; i++) {            //可增加4个
    db_osd = osd_data.at(i);
    nlohmann::json character = db_osd;
    for (auto &x : nlohmann::json::iterator_wrapper(db_osd)) {
      if ((x.key() != "iPositionX") && (x.key() != "iPositionY") &&
          (x.key() != "iEnabled") && (x.key() != "sDisplayText") && (x.key() != "id"))
      {
        character.erase(x.key());
      }
    }
    character.erase("id");
    character.emplace("id", i - 2);
    character.emplace("iTextOverlayEnabled", character.at("iEnabled"));
    character.erase("iEnabled");
    all_character.push_back(character);
  }
  // overlays
  osd_overlays.emplace("normalizedScreenSize", normalized_screen_size);
 // osd_overlays.emplace("attribute", attribute);
  osd_overlays.emplace("channelNameOverlay", channel_name);
  osd_overlays.emplace("dateTimeOverlay", date_time);
  osd_overlays.emplace("characterOverlay", all_character);

  return osd_overlays;
}



nlohmann::json osd_privacy_mask_get() {
  char *str;
  nlohmann::json osd_privacy_mask;
  nlohmann::json all_privacy_mask;

  str = dbserver_media_get((char *)TABLE_NORMALIZED_SCREEN_SIZE);
  nlohmann::json normalized_screen_size =
      nlohmann::json::parse(str).at("jData").at(0);
  normalized_screen_size.erase("id");
  str = dbserver_osd_get();
  nlohmann::json db_osd = nlohmann::json::parse(str);

  for (int i = 6; i < 10; i++) {
    nlohmann::json privacy_mask = db_osd.at(i);
    for (auto &x : nlohmann::json::iterator_wrapper(db_osd.at(i))) {
      if ((x.key() != "iPositionX") && (x.key() != "iPositionY") &&
          (x.key() != "iEnabled") && (x.key() != "id") &&
          (x.key() != "iWidth") && (x.key() != "iHeight")) {
        privacy_mask.erase(x.key());
      }
    }
    privacy_mask.erase("id");
    privacy_mask.emplace("id", i - 6);
    privacy_mask.emplace("iMaskWidth", privacy_mask.at("iWidth"));
    privacy_mask.emplace("iMaskHeight", privacy_mask.at("iHeight"));
    privacy_mask.emplace("iPrivacyMaskEnabled", privacy_mask.at("iEnabled"));
    privacy_mask.erase("iWidth");
    privacy_mask.erase("iHeight");
    privacy_mask.erase("iEnabled");
    all_privacy_mask.push_back(privacy_mask);
  }
  osd_privacy_mask.emplace("normalizedScreenSize", normalized_screen_size);
  osd_privacy_mask.emplace("privacyMask", all_privacy_mask);

  return osd_privacy_mask;
}

void osd_overlays_set(nlohmann::json osd_overlays) {
  for (auto &x : nlohmann::json::iterator_wrapper(osd_overlays)) {

    if (x.key() == KEY_NORMALIZED_SCREEN_SIZE) {
      auto val = x.value(); /* string or int */
      dbserver_media_set((char *)TABLE_NORMALIZED_SCREEN_SIZE,
                                     (char *)val.dump().c_str(), 0);
    }
    else if (x.key() == KEY_OSD_CHANNEL_NAME_OVERLAY) {    //通道显示
      nlohmann::json channel_name =
          osd_overlays.at(KEY_OSD_CHANNEL_NAME_OVERLAY);

      channel_name.emplace("sDisplayText", channel_name.at("sChannelName"));
      channel_name.emplace("iEnabled", channel_name.at("iChannelNameOverlayEnabled"));

      channel_name.erase("sChannelName");
      channel_name.erase("iChannelNameOverlayEnabled");
      dbserver_media_set((char *)TABLE_OSD, (char *)channel_name.dump().c_str(), 0);
    } else if (x.key() == KEY_OSD_DATE_TIME_OVERLAY) {      //时间显示

      nlohmann::json date_time = osd_overlays.at(KEY_OSD_DATE_TIME_OVERLAY);

      date_time.emplace("iEnabled", date_time.at("iDateTimeOverlayEnabled"));
      date_time.erase("iDateTimeOverlayEnabled");

      dbserver_media_set((char *)TABLE_OSD, (char *)date_time.dump().c_str(), 1);
    } else if (x.key() == KEY_OSD_CHARACTER_OVERLAY) {     //osd叠加
      nlohmann::json all_character = osd_overlays.at(KEY_OSD_CHARACTER_OVERLAY);
      for (auto &x : nlohmann::json::iterator_wrapper(all_character)) {
        nlohmann::json character = x.value();
        int id = character.at("id");
        character.erase("id");
        character.emplace("id", id + 2);
        character.emplace("iEnabled", character.at("iTextOverlayEnabled"));
        character.erase("iTextOverlayEnabled");
        dbserver_media_set((char *)TABLE_OSD, (char *)character.dump().c_str(), id + 2);
      }
    }
  }
}

void osd_overlays_ir_set(nlohmann::json osd_overlays) {
  for (auto &x : nlohmann::json::iterator_wrapper(osd_overlays)) {

    if (x.key() == KEY_NORMALIZED_SCREEN_SIZE) {
      auto val = x.value(); /* string or int */
      dbserver_media_set((char *)TABLE_IR_SCREEN_SIZE,
                         (char *)val.dump().c_str(), 0);
    }
    //    else if (x.key() == KEY_OSD_ATTRIBUTE) {
    //      nlohmann::json attribute = osd_overlays.at(KEY_OSD_ATTRIBUTE);
    //      for (int i = 0; i < 10; i++) {
    //        dbserver_media_set((char *)TABLE_OSD_IR, (char *)attribute.dump().c_str(), i);
    //      }
    //
    //    }
    else if (x.key() == KEY_OSD_CHANNEL_NAME_OVERLAY) {    //通道显示
      nlohmann::json channel_name =
          osd_overlays.at(KEY_OSD_CHANNEL_NAME_OVERLAY);

      channel_name.emplace("sDisplayText", channel_name.at("sChannelName"));
      channel_name.emplace("iEnabled", channel_name.at("iChannelNameOverlayEnabled"));

      channel_name.erase("sChannelName");
      channel_name.erase("iChannelNameOverlayEnabled");
      dbserver_media_set((char *)TABLE_OSD_IR, (char *)channel_name.dump().c_str(), 0);
    } else if (x.key() == KEY_OSD_DATE_TIME_OVERLAY) {      //时间显示

      nlohmann::json date_time = osd_overlays.at(KEY_OSD_DATE_TIME_OVERLAY);

      date_time.emplace("iEnabled", date_time.at("iDateTimeOverlayEnabled"));
      date_time.erase("iDateTimeOverlayEnabled");

      dbserver_media_set((char *)TABLE_OSD_IR, (char *)date_time.dump().c_str(), 1);
    } else if (x.key() == KEY_OSD_CHARACTER_OVERLAY) {     //osd叠加
      nlohmann::json all_character = osd_overlays.at(KEY_OSD_CHARACTER_OVERLAY);
      for (auto &x : nlohmann::json::iterator_wrapper(all_character)) {
        nlohmann::json character = x.value();
        int id = character.at("id");
        character.erase("id");
        character.emplace("id", id + 2);
        character.emplace("iEnabled", character.at("iTextOverlayEnabled"));
        character.erase("iTextOverlayEnabled");
        dbserver_media_set((char *)TABLE_OSD_IR, (char *)character.dump().c_str(), id + 2);
      }
    }
  }
}



void osd_privacy_mask_set(nlohmann::json osd_privacy_mask_config) {
  for (auto &x : nlohmann::json::iterator_wrapper(osd_privacy_mask_config)) {

    if (x.key() == KEY_NORMALIZED_SCREEN_SIZE) {
      auto val = x.value(); /* string or int */
      dbserver_media_set((char *)TABLE_NORMALIZED_SCREEN_SIZE,
                                     (char *)val.dump().c_str(), 0);
    } else if (x.key() == KEY_OSD_PRIVACY_MASK) {
      nlohmann::json all_privacy_mask =
          osd_privacy_mask_config.at(KEY_OSD_PRIVACY_MASK);
      for (auto &x : nlohmann::json::iterator_wrapper(all_privacy_mask)) {
        nlohmann::json privacy_mask = x.value();
        int id = privacy_mask.at("id");
        privacy_mask.erase("id");
        privacy_mask.emplace("id", id + 6);
        privacy_mask.emplace("iWidth", privacy_mask.at("iMaskWidth"));
        privacy_mask.emplace("iHeight", privacy_mask.at("iMaskHeight"));
        privacy_mask.emplace("iEnabled", privacy_mask.at("iPrivacyMaskEnabled"));
        privacy_mask.erase("iMaskWidth");
        privacy_mask.erase("iMaskHeight");
        privacy_mask.erase("iPrivacyMaskEnabled");
        dbserver_media_set((char *)TABLE_OSD, (char *)privacy_mask.dump().c_str(), id + 6);
      }
    }
  }
}

void osd_set_mediaserver(nlohmann::json osd_change) {
  char *str = dbserver_osd_get();
  nlohmann::json db_osd = nlohmann::json::parse(str);

  for (auto &x : nlohmann::json::iterator_wrapper(osd_change)) {
    if (x.key() == KEY_OSD_ATTRIBUTE) {
      for (int i = OSD_DB_REGION_ID_CHANNEL; i <= OSD_DB_REGION_ID_TEXT7; i++) {
        nlohmann::json config = db_osd.at(i);
        mediaserver_set((char *)TABLE_OSD, i, (char *)config.dump().c_str());
      }
    } else if (x.key() == KEY_OSD_DATE_TIME_OVERLAY) {
      nlohmann::json config = db_osd.at(OSD_DB_REGION_ID_DATETIME);
      mediaserver_set((char *)TABLE_OSD, OSD_DB_REGION_ID_DATETIME,
                      (char *)config.dump().c_str());
    } else if (x.key() == KEY_OSD_CHANNEL_NAME_OVERLAY) {
      nlohmann::json config = db_osd.at(OSD_DB_REGION_ID_CHANNEL);
      mediaserver_set((char *)TABLE_OSD, OSD_DB_REGION_ID_CHANNEL,
                      (char *)config.dump().c_str());
    } else if (x.key() == KEY_OSD_CHARACTER_OVERLAY) {
      for (int i = OSD_DB_REGION_ID_TEXT0; i <= OSD_DB_REGION_ID_TEXT7; i++) {
        nlohmann::json config = db_osd.at(i);
        mediaserver_set((char *)TABLE_OSD, i, (char *)config.dump().c_str());
      }
    } else if (x.key() == KEY_OSD_PRIVACY_MASK) {
      for (int i = OSD_DB_REGION_ID_MASK0; i <= OSD_DB_REGION_ID_MASK3; i++) {
        nlohmann::json config = db_osd.at(i);
        mediaserver_set((char *)TABLE_OSD, i, (char *)config.dump().c_str());
      }
    } else if (x.key() == KEY_OSD_IMAGE_OVERLAY) {
      nlohmann::json config = db_osd.at(OSD_DB_REGION_ID_IMAGE);
      mediaserver_set((char *)TABLE_OSD, OSD_DB_REGION_ID_IMAGE, (char *)config.dump().c_str());
    }
  }
}

void OSDApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {
  std::string path_api_resource;
  std::string path_specific_resource;
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
  }

  if (Req.Method == "GET") {
    if (!path_specific_resource.compare("overlays/0")) {    //osd 配置信息
      nlohmann::json data_config= osd_overlays_get(0);
//      std::list<nlohmann::json> data_list;
//      data_list.push_back(data_config);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
//    } else if (!path_specific_resource.compare("image")) {
    }else if (!path_specific_resource.compare("overlays/1")) {    //osd 配置信息

      nlohmann::json data_config= osd_overlays_get(1);
//      std::list<nlohmann::json> data_list;
//      data_list.push_back(data_config);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
      //    } else if (!path_specific_resource.compare("image")) {
    }
    else if (!path_specific_resource.compare("privacy-mask")) {    //区域遮挡
      nlohmann::json data_config = osd_privacy_mask_get();
//      std::list<nlohmann::json> data_list;
//      data_list.push_back(data_config);
      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",data_config);
    }

    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
#ifdef ENABLE_JWT
     if (user_level > 1) {
      Resp.setErrorResponse(HttpStatus::kUnauthorized, "Unauthorized");
      return;
    }
#endif
    nlohmann::json diff;
    nlohmann::json cfg_old;
    nlohmann::json osd_config = Req.PostObject; /* must be json::object */
    if (!path_specific_resource.compare("overlays/0")) {   //rgb osd set
      /* Erase unchanged data */
      cfg_old = osd_overlays_get(0);
      diff = nlohmann::json::diff(cfg_old, osd_config);  //取出不同的key-value
      for (auto &x : nlohmann::json::iterator_wrapper(cfg_old)) {
        if (diff.dump().find(x.key()) == diff.dump().npos)  //npos表示不存在的位置
           osd_config.erase(x.key());
      }
      /* Set */
      if (!osd_config.empty())
         osd_overlays_set(osd_config);
      content.emplace("code",200);
      content.emplace("msg","setting successfully");
//      osd_set_mediaserver(osd_config);
      osd_mediaserver_set(0, "overlays-rgb");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }else if (!path_specific_resource.compare("overlays/1")) {   //ir osd set
      /* Erase unchanged data */
      cfg_old = osd_overlays_get(1);
      diff = nlohmann::json::diff(cfg_old, osd_config);  //取出不同的key-value
      for (auto &x : nlohmann::json::iterator_wrapper(cfg_old)) {
        if (diff.dump().find(x.key()) == diff.dump().npos)  //npos表示不存在的位置
           osd_config.erase(x.key());
      }
      /* Set */
      if (!osd_config.empty())
         osd_overlays_ir_set(osd_config);
      content.emplace("code",200);
      content.emplace("msg","setting successfully");
      osd_mediaserver_set(0, "overlays-ir");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }
        else if (!path_specific_resource.compare("privacy-mask")) {    //mask set
      /* Erase unchanged data */
      cfg_old = osd_privacy_mask_get();
      diff = nlohmann::json::diff(cfg_old, osd_config);
      for (auto &x : nlohmann::json::iterator_wrapper(cfg_old)) {
        if (diff.dump().find(x.key()) == diff.dump().npos)
          osd_config.erase(x.key());
      }
      /* Set */
      if (!osd_config.empty())
        osd_privacy_mask_set(osd_config);
      /* Get new info */
      content.emplace("code",200);
      content.emplace("msg","setting successfully");
//      content = osd_privacy_mask_get();
//      osd_set_mediaserver(osd_config);
      osd_mediaserver_set(0,"privacy-mask");
      Resp.setHeader(HttpStatus::kOk, "OK");
      Resp.setApiData(content);
    }

  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}

} // namespace cgi
} // namespace xggd
