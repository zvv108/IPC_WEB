//
// Created by zvv on 22-11-15.
//

#ifndef IPCWEB_BACKEND_GB28181_API_H
#define IPCWEB_BACKEND_GB28181_API_H

#include "api_handler.h"
namespace xggd {
namespace cgi {

class Gb28181ApiHandler : public ApiHandler {
public:
  Gb28181ApiHandler() = default;
  ~Gb28181ApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // IPCWEB_BACKEND_GB28181_API_H
