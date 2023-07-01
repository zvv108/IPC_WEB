#ifndef __CGI_STREAM_API_H__
#define __CGI_STREAM_API_H__

#include "api_handler.h"
namespace xggd {
namespace cgi {

class StreamURLApiHandler : public ApiHandler {
public:
  StreamURLApiHandler() = default;
  ~StreamURLApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_STREAM_API_H__
