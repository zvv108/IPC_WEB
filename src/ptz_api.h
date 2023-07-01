
#ifndef IPCWEB_BACKEND_PTZ_API_H
#define IPCWEB_BACKEND_PTZ_API_H

#include "api_handler.h"
namespace xggd {
namespace cgi {

class PTZApiHandler : public ApiHandler {
public:
  PTZApiHandler() = default;
  ~PTZApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd
#endif // IPCWEB_BACKEND_PTZ_API_H
