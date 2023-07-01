#include "common.h"

namespace xggd {
namespace cgi {

std::string ipv4_address_get() {
  char pService[255];
  char *str = netserver_get_service((char *)"ethernet");
  if (!strcmp(str, "[ ]"))
    str = netserver_get_service((char *)"wifi");

  nlohmann::json service = nlohmann::json::parse(str).at(0).at("sService");
  strcpy(pService, std::string(service.get<std::string>()).c_str());
  str = netserver_get_config((char *)pService);
  nlohmann::json cfg = nlohmann::json::parse(str);
  std::string ipv4_address = cfg.at(0).at("ipv4").at("sV4Address");
//  minilog_debug("ipv4_address1111111 is _:%s",ipv4_address.c_str());
  return ipv4_address;
}

std::string current_time_get() {
  char current_time[30];
  time_t rawtime;
  time(&rawtime);
  struct tm *time_info = localtime(&rawtime);
  strftime(current_time, 30, "%FT%T", time_info);
  std::string current_time_s = current_time;

  return current_time_s;
}

time_t standard_to_timestamp(std::string standard_time) {
  time_t stamp_time = 0;
  struct tm *timeinfo;

  // get current timeinfo and modify it to the user's choice
  time(&stamp_time);
  timeinfo = localtime(&stamp_time);
  timeinfo->tm_year = stoi(standard_time.substr(0, 4)) - 1900;
  timeinfo->tm_mon = stoi(standard_time.substr(5, 2)) - 1;
  timeinfo->tm_mday = stoi(standard_time.substr(8, 2));
  timeinfo->tm_hour = stoi(standard_time.substr(11, 2));
  timeinfo->tm_min = stoi(standard_time.substr(14, 2));
  timeinfo->tm_sec = stoi(standard_time.substr(17, 2));
  stamp_time = mktime(timeinfo);

  return stamp_time;
}


int64_t standard_to_timestamp_int(std::string standard_time) {
  int64_t stamp_time = 0;
  int tm_year = stoi(standard_time.substr(0, 4)) - 1970;
  int tm_mon = stoi(standard_time.substr(5, 2)) - 1;
  int tm_mday = stoi(standard_time.substr(8, 2));
  int tm_hour = stoi(standard_time.substr(11, 2));
  int tm_min = stoi(standard_time.substr(14, 2));
  int tm_sec = stoi(standard_time.substr(17, 2));
  stamp_time = tm_year*31536000 + tm_mon*2592000 + tm_mday*86400 + tm_hour*3600 + tm_min*60 +tm_sec;

  return stamp_time;
}




std::string jwt_token_get(std::string username, int auth, long expiretime) {
  std::string token =
      jwt::create()
          .set_issuer("auth0")
          .set_type("JWS")
          .set_payload_claim("username", jwt::claim(username))
          .set_payload_claim("auth", jwt::claim(std::to_string(auth)))
          .set_issued_at(std::chrono::system_clock::now())
          .set_expires_at(std::chrono::system_clock::now() +
                          std::chrono::seconds{expiretime})
          .sign(jwt::algorithm::hs256{SECRET});
  return token;
}

std::string jwt_token_verify(std::string token, HttpRequest &Req) {
  auto decoded = jwt::decode(token);
  std::string payload = decoded.get_payload();
  nlohmann::json payload_json = nlohmann::json::parse(payload);
  int expire_time = payload_json.at("exp");
  std::string new_token = "";
  int now_time =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::string auth = payload_json.at("auth");
  if ((expire_time - now_time) < REACTIVE_TIME) {
    std::string user_name = payload_json.at("username");
    std::string cmd = "select id from SystemUser WHERE sUserName='" +
                      user_name + "' AND iUserLevel=" + auth;
    char *str = dbserver_sql((char *)cmd.c_str(), (char *)DBSERVER_SYSTEM_INTERFACE);
    nlohmann::json check_json = nlohmann::json::parse(str).at("jData");
    if (!check_json.empty()) {
      new_token = jwt_token_get(user_name, stoi(auth), EXPIRE_SECONDS);
    } else {
      new_token = jwt_token_get(user_name, stoi(auth), 0);
    }
    // minilog_debug("create token is %s", new_token.c_str());
  }
  Req.UserLevel = stoi(auth);
  auto verifier = jwt::verify()
                      .allow_algorithm(jwt::algorithm::hs256{SECRET})
                      .with_issuer("auth0");
  verifier.verify(decoded);
  return new_token;
}

unsigned char FromHex(unsigned char x) {
  unsigned char y;
  if (x >= 'A' && x <= 'Z')
    y = x - 'A' + 10;
  else if (x >= 'a' && x <= 'z')
    y = x - 'a' + 10;
  else if (x >= '0' && x <= '9')
    y = x - '0';
  else
    assert(0);
  return y;
}

std::string DecodeURI(std::string &str) {
  std::string strTemp = "";
  size_t length = str.length();
  for (size_t i = 0; i < length; i++) {
    if (str[i] == '+')
      strTemp += ' ';
    else if (str[i] == '%') {
      assert(i + 2 < length);
      unsigned char high = FromHex((unsigned char)str[++i]);
      unsigned char low = FromHex((unsigned char)str[++i]);
      strTemp += high * 16 + low;
    } else
      strTemp += str[i];
  }
  return strTemp;
}

} // namespace cgi
} // namespace xggd
