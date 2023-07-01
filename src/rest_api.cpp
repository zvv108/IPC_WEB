#include "rest_api.h"

#include <functional>
#include <sstream>

#include "cgicc/Cgicc.h"
#include "common.h"
#include "event_api.h"
#include "image_api.h"
#include "network_api.h"
#include "osd_api.h"
#include "stream_api.h"
#include "system_api.h"
#include "gb28181_api.h"
#include "ptz_api.h"
#include "video_api.h"

namespace xggd {
namespace cgi {
inline namespace detail {

std::vector<QueryParam> parseParams(const std::string &QueryString) {
  const std::string ParamDelimiter = "&";
  const std::string ValueDelimiter = "=";
  std::vector<QueryParam> Params;
  QueryParam Param;
  size_t PosStart = 0;
  size_t PosEnd;
  size_t ValStart;
  size_t DelimLen = ParamDelimiter.length();
  std::string Token;

  while ((PosEnd = QueryString.find(ParamDelimiter, PosStart)) !=
         std::string::npos) {
    Token = QueryString.substr(PosStart, PosEnd - PosStart);
    PosStart = PosEnd + DelimLen;
    if ((ValStart = Token.find(ValueDelimiter, 0)) != std::string::npos) {
      Param.Key = Token.substr(0, ValStart);
      ValStart += ValueDelimiter.length();
      Param.Value = Token.substr(ValStart);
      Params.push_back(Param);
    }
  }
  Token = QueryString.substr(PosStart);
  if ((ValStart = Token.find(ValueDelimiter, 0)) != std::string::npos) {
    Param.Key = Token.substr(0, ValStart);
    ValStart += ValueDelimiter.length();
    Param.Value = Token.substr(ValStart);
    Params.push_back(Param);
  }

  return Params;
}

std::string checkToken(HttpRequest &Req) {
  if (Req.PathInfo.compare("/system/login") && Req.PathInfo.find("system/para/webPage") == std::string::npos) 
  {
    if (Req.Cookies.empty())
      throw jwt::token_verification_exception("not found cookie");
    minilog_debug("cookies is: %s\n", Req.Cookies.c_str());
    std::string cookie = Req.Cookies;
    int pos = cookie.find("token=");
    cookie = cookie.substr(pos + 6);
    int pos_end = cookie.find(";");
    std::string token;
    if (pos_end == -1)
      token = cookie;
    else
      token = cookie.substr(0, pos_end);
    std::string new_token = jwt_token_verify(token, Req);
    minilog_debug("new_token : %s\n", new_token.c_str());
    return new_token;
  }
  return "";
}

HttpStatus parseRequest(HttpRequest &Req) {
  cgicc::Cgicc cgi;

  Req.PathInfo = cgi.getEnvironment().getPathInfo();
  Req.ScriptName = cgi.getEnvironment().getScriptName();
  Req.Cookies = cgi.getEnvironment().getCookies();
  minilog_debug("Req.PathInfo:%s\n",Req.PathInfo.c_str());
  minilog_debug("Req.ScriptName:%s\n",Req.ScriptName.c_str());
  minilog_debug("Req.gCookies:%s\n",Req.Cookies.c_str());

  assert(!Req.PathInfo.empty());

  int pos_first = Req.PathInfo.find_first_of("/");
  int pos_last = Req.PathInfo.find_last_of("/");
  if (pos_first != pos_last) {
    std::string path = Req.PathInfo.substr(pos_first + 1, Req.PathInfo.size());
    int pos_second = path.find_first_of("/");
    Req.Api = Req.PathInfo.substr(pos_first, pos_second + 1).c_str();
  } else {
    Req.Api = Req.PathInfo;
  }

  Req.AuthMethod = cgi.getEnvironment().getAuthType();

  Req.Method = cgi.getEnvironment().getRequestMethod();
  if ((Req.Method == "POST") || (Req.Method == "PUT")) {
    Req.Params = parseParams(cgi.getEnvironment().getQueryString());
    Req.ContentLength = cgi.getEnvironment().getContentLength();
    if (Req.ContentLength > 0) {
      Req.ContentType = cgi.getEnvironment().getContentType();
      if (std::string::npos != Req.ContentType.find("application/json")) {
        std::string PostData = cgi.getEnvironment().getPostData();
         minilog_debug("PostData:%s\n",PostData);
        Req.PostObject = nlohmann::json::parse(PostData);
      } else if (std::string::npos !=
                 Req.ContentType.find("multipart/form-data")) {
        Req.Files = cgi.getFiles();
      } else if (std::string::npos != Req.ContentType.find("text/plain")) {
        Req.PostData = cgi.getEnvironment().getPostData();
      } else {
        return HttpStatus::kBadRequest;
      }
    }
  } else if (Req.Method == "GET") {
    Req.Params = parseParams(cgi.getEnvironment().getQueryString());
  } else if (Req.Method == "DELETE") {
    Req.Params = parseParams(cgi.getEnvironment().getQueryString());
  } else {
    return HttpStatus::kBadRequest;
  }
  return HttpStatus::kOk;
}

} // namespace detail

void ApiEntry::run() {
  using namespace std::placeholders;

  // A workaround for unexpected log messages in linked libraries
  // TODO: Find a better way for this feature requirement
  // APPLICATION NOTES:
  //   1. Don't use std::cerr, it will sync C stderr
  //   2. Don't use fprintf(stderr/stdout...)/printf
  //   3. Alternative logging library in this application:
  //     #include <minilogger/log.h>

  std::ios::sync_with_stdio(false);

  HttpRequest Req;
  HttpResponse Resp;
  const std::string network_api = "network";
  const std::string network_ntp_api = "network-ntp";
  const std::string network_ddns_api = "network-ddns";
  const std::string network_pppoe_api = "network-pppoe";
  const std::string network_port_api = "network-port";
  const std::string video_api = "video";
  const std::string stream_url_api = "stream-url";
  const std::string image_api = "image";
  const std::string system_api = "system";
  const std::string gb28181_api = "28181";
  const std::string ptz_api = "ptz";

  const std::string osd_api = "osd";
  const std::string event_api = "event";

  HandlerEntry e;
  e.Api = network_api;
  e.handler =
      std::bind(&NetworkApiHandler::handler, NetworkApiHandler(), _1, _2);
  Handlers.push_back(e);
  e.Api = network_ntp_api;
  e.handler =
      std::bind(&NetworkNTPApiHandler::handler, NetworkNTPApiHandler(), _1, _2);
  Handlers.push_back(e);

  e.Api = network_port_api;
  e.handler = std::bind(&NetworkPortApiHandler::handler,
                        NetworkPortApiHandler(), _1, _2);
  Handlers.push_back(e);

  e.Api = video_api;
  e.handler = std::bind(&VideoApiHandler::handler, VideoApiHandler(), _1, _2);
  Handlers.push_back(e);

  e.Api = stream_url_api;
  e.handler =
      std::bind(&StreamURLApiHandler::handler, StreamURLApiHandler(), _1, _2);
  Handlers.push_back(e);
  e.Api = image_api;
  e.handler = std::bind(&ImageApiHandler::handler, ImageApiHandler(), _1, _2);
  Handlers.push_back(e);
  e.Api = system_api;
  e.handler = std::bind(&SystemApiHandler::handler, SystemApiHandler(), _1, _2);
  Handlers.push_back(e);
  e.Api = gb28181_api;
  e.handler = std::bind(&Gb28181ApiHandler::handler, Gb28181ApiHandler(), _1, _2);
  Handlers.push_back(e);
  e.Api = ptz_api;
  e.handler = std::bind(&PTZApiHandler::handler, PTZApiHandler(), _1, _2);
  Handlers.push_back(e);

  e.Api = osd_api;
  e.handler = std::bind(&OSDApiHandler::handler, OSDApiHandler(), _1, _2);
  Handlers.push_back(e);

  e.Api = event_api;
  e.handler = std::bind(&EventApiHandler::handler, EventApiHandler(), _1, _2);
  Handlers.push_back(e);


  std::string new_token;
  Resp.setHeader(HttpStatus::kOk);
  try {
    if (HttpStatus::kOk == parseRequest(Req)) {
#ifdef ENABLE_JWT
      new_token = checkToken(Req);
      minilog_debug("level is %d\n", Req.UserLevel);
#endif
      for (auto h : Handlers) {
        if (!Req.Api.compare(1, 20, h.Api, 0, 20)) {   //比较内容相同
#ifdef MEDIASERVER_ROCKFACE
          minilog_debug("Req.PathInfo: %s, mediaserver rockface enable\n", Req.PathInfo.c_str());
#else
          minilog_debug("Req.PathInfo: %s, mediaserver rockface disable\n", Req.PathInfo.c_str());
#endif
          h.handler(Req, Resp);
        }
      }
    } else {
      Resp.setErrorResponse(HttpStatus::kBadRequest, "Error parsing request!");
    }
  } catch (jwt::token_verification_exception &e) {
    minilog_error("Unauthorized, reason: %s", e.what());
    Resp.setErrorResponse(HttpStatus::kUnauthorized, e.what());
  } catch (std::exception &e) {
    minilog_error("Fatal failure, reason: %s", e.what());
    Resp.setErrorResponse(HttpStatus::kInternalServerError, e.what());
  }

  if (new_token.size() > 0) {
    Resp.setCookie("token", new_token, EXPIRE_SECONDS);
  }

  std::ostringstream os;

  Resp.render(os);
  std::cout << os.str();
  minilog_debug("os.str(): %s\n",(os.str()).c_str());

}

} // namespace cgi
} // namespace xggd
