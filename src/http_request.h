

#ifndef __CGI_HTTP_REQUEST_H__
#define __CGI_HTTP_REQUEST_H__

#include <string>
#include <vector>

#include "cgicc/Cgicc.h"
#include "export.h"
#include "nlohmann/json.hpp"

namespace xggd {
namespace cgi {

struct CGI_EXPORT QueryParam {
  std::string Key;
  std::string Value;
};

struct CGI_EXPORT HttpRequest {
  std::string Api;
  std::string Method;
  std::string ScriptName;
  std::string PathInfo;
  std::vector<QueryParam> Params;
  std::string ContentType;
  std::string Cookies;
  std::string AuthMethod;
  std::string PostData;
  size_t ContentLength;
  nlohmann::json PostObject;
  int UserLevel;
  std::vector<cgicc::FormFile> Files;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_HTTP_REQUEST_H__