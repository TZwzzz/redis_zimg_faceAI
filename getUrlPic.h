#ifndef GETURLPIC_H_
#define GETURLPIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "logs.h"

int getUrlPics(const char *filename);
int getUrlPic(const char *url,const char *SavePicName);

#endif