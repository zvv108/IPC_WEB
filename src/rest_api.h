

#ifndef __CGI_REST_API_H__
#define __CGI_REST_API_H__

#include "export.h"
#include "http_request.h"
#include "http_response.h"

#include "minilogger/backtrace.h"
#include "minilogger/log.h"

namespace xggd {
namespace cgi {

typedef std::function<void(const HttpRequest &, HttpResponse &)> Handler;

struct HandlerEntry {
  std::string Api;
  Handler handler;
};

class CGI_EXPORT ApiEntry {
public:
  ApiEntry() = default;
  ~ApiEntry() = default;
  ApiEntry(const ApiEntry &) = delete;
  ApiEntry &operator=(const ApiEntry &) = delete;

  static ApiEntry &getInstance() {
    static ApiEntry instance;
    return instance;
  }
  void run();

  std::vector<HandlerEntry> Handlers;
};

} // namespace cgi
} // namespace xggd

#endif // __CGI_REST_API_H__
