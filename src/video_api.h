#ifndef __CGI_VIDEO_API_H__
#define __CGI_VIDEO_API_H__

#include "api_handler.h"
namespace xggd {
namespace cgi {

class VideoApiHandler : public ApiHandler {
public:
  VideoApiHandler() = default;
  ~VideoApiHandler() = default;

  virtual void handler(const HttpRequest &Req, HttpResponse &Resp) final;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_VIDEO_API_H__
