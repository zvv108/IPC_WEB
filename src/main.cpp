#include "rest_api.h"

using namespace xggd::cgi;
int main(int argc, char **argv) {
  //printf("Content-Type: text/html\n\n");
  printf("Content-Type: application/json;charset-utf8\n\n");
  __minilog_log_init(argv[0], NULL, false, true, "cgi", "1.0");
  ApiEntry::getInstance().run();
  return 0;
}
