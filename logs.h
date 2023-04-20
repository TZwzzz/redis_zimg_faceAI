#ifndef LOGS_H_
#define LOGS_H_

#include <iostream>
#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"


int logs_init();
std::shared_ptr<spdlog::logger> get_jsonLogs();
std::shared_ptr<spdlog::logger> get_getUrlPicLogs();
std::shared_ptr<spdlog::logger> get_redis_toolLogs();
std::shared_ptr<spdlog::logger> get_faceDetectLogs();

#endif