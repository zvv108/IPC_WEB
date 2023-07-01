#ifndef __CGI_EVENT_API_H__
#define __CGI_EVENT_API_H__

#include "api_handler.h"
namespace xggd {
namespace cgi {

class EventApiHandler : public ApiHandler {
public:
  EventApiHandler() = default;
  ~EventApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_EVENT_API_H__
