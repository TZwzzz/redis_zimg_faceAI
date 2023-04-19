#ifndef JSON_H_
#define JSON_H_

#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <pthread.h>
#include "faceDetect.h"
#include "logs.h"

using namespace std;

std::string ret_rect_to_json(const std::vector<ret_rect>& ret_rect);

#endif


