#ifndef __CGI_NETWORK_API_H__
#define __CGI_NETWORK_API_H__

#include "api_handler.h"
namespace xggd {
namespace cgi {

class NetworkApiHandler : public ApiHandler {
public:
  NetworkApiHandler() = default;
  ~NetworkApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

class NetworkNTPApiHandler : public ApiHandler {
public:
  NetworkNTPApiHandler() = default;
  ~NetworkNTPApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};


class NetworkPortApiHandler : public ApiHandler {
public:
  NetworkPortApiHandler() = default;
  ~NetworkPortApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_NETWORK_API_H__
