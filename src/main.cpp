// Copyright 2019 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "rest_api.h"

using namespace rockchip::cgi;
int main(int argc, char **argv) {
  //printf("Content-Type: text/html\n\n");
  printf("Content-Type: application/json;charset-utf8\n\n");
  __minilog_log_init(argv[0], NULL, false, true, "cgi", "1.0");
  ApiEntry::getInstance().run();
  return 0;
}
