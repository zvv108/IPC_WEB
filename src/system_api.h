#ifndef __CGI_SYSTEM_API_H__
#define __CGI_SYSTEM_API_H__

#include "api_handler.h"
namespace xggd {
namespace cgi {

class SystemApiHandler : public ApiHandler {
public:
  SystemApiHandler() = default;
  ~SystemApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_SYSTEM_API_H__
