#include "stream_api.h"
#include "common.h"
#include <dbserver.h>
#include <netserver.h>

namespace xggd {
namespace cgi {

void StreamURLApiHandler::handler(const HttpRequest &Req, HttpResponse &Resp) {

  if (Req.Method == "GET") {
    nlohmann::json content;
    char *str = dbserver_stream_url_get();
    nlohmann::json stream_url_config = nlohmann::json::parse(str).at("jData");
    int pos_first = Req.PathInfo.find_first_of("/");
    int pos_last = Req.PathInfo.find_last_of("/");



    char *str_channel_rgb = dbserver_osd_get_by_id((char *)TABLE_OSD, 0);
    char *str_channel_ir = dbserver_osd_get_by_id((char *)TABLE_OSD_IR,0);

    std::string channel_rgb =  nlohmann::json::parse(str_channel_rgb).at("jData")[0].at("sDisplayText");
    std::string channel_ir =  nlohmann::json::parse(str_channel_ir).at("jData")[0].at("sDisplayText");


    for (int i = 0; i < 4; i++) {

      if (i == 0 || i == 1)
      {
        stream_url_config.at(i).emplace("streamChannel", channel_rgb);
      }
      else if (i == 2 || i == 3)
      {
        stream_url_config.at(i).emplace("streamChannel", channel_ir);
      }
      stream_url_config.at(i).erase("id");
    }


      content.emplace("code",200);
      content.emplace("msg","operation successfully");
      content.emplace("data",stream_url_config);

    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);
  } else if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    nlohmann::json content = nlohmann::json::object();
    nlohmann::json stream_url_config =
        Req.PostObject; /* must be json::object */
    int id = 0;
    int pos_first = Req.PathInfo.find_first_of("/");
    int pos_last = Req.PathInfo.find_last_of("/");
    if (pos_first != pos_last)
      id = atoi(Req.PathInfo.substr(pos_last + 1, Req.PathInfo.size()).c_str());

    /* Erase unchanged data */
    char *prev = dbserver_stream_url_get();
    nlohmann::json cfg_old_all = nlohmann::json::parse(prev).at("jData");
    nlohmann::json diff =
        nlohmann::json::diff(cfg_old_all.at(id), stream_url_config);
    for (auto &x : nlohmann::json::iterator_wrapper(cfg_old_all.at(id))) {
      if (diff.dump().find(x.key()) == diff.dump().npos)
        stream_url_config.erase(x.key());
    }

    /* sURL cannot be set */
    if (stream_url_config.dump().find("sURL") != stream_url_config.dump().npos)
      stream_url_config.erase("sURL");

    /* Set new config, only one table is transmitted at a time. */
    if (!stream_url_config.empty())
      dbserver_stream_url_set((char *)stream_url_config.dump().c_str(), id);

    /* Get new info */
    char *str = dbserver_stream_url_get();
    nlohmann::json stream_url_all_config =
        nlohmann::json::parse(str).at("jData");
    content = stream_url_all_config.at(id);
    Resp.setHeader(HttpStatus::kOk, "OK");
    Resp.setApiData(content);
  } else {
    Resp.setErrorResponse(HttpStatus::kNotImplemented, "Not Implemented");
  }
}

} // namespace cgi
} // namespace xggd
