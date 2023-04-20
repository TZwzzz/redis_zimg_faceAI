#ifndef MAIN_H_
#define MAIN_H_

#include <signal.h>
#include "faceDetect.h"
#include "getUrlPic.h"
#include "redis_tool.hpp"
#include "json.h"
#include "logs.h"

#define DATABASE_INDEX      3
#define IOT_TO_PIC_STREAM   "picAI"
#define IOT_TO_PIC_GROUP    "g_picAI"
#define IOT_TO_PIC_CONSUMER "iot_to_pic_consumer"
#define IOT_TO_PIC_KEY1     "url"
#define IOT_TO_PIC_KEY2     "aiType"

#define REPLY_STREAM        "picReply"
#define REPLY_GROUP         "g_picReply"
#define REPLY_CONSUMER      "reply_consumer"
#define REPLY_KEY1          "url"
#define REPLY_KEY2          "aiType"
#define REPLY_KEY3          "result"

#define REDIS_SERVER_IP     "127.0.0.1"
#define REDIS_SERVER_PORT   6379

using namespace std;

volatile sig_atomic_t g_running = 1;

#endif