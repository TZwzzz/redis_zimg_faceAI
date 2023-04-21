#include "getUrlPic.h"

int getUrlPics(const char *filename)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;
    std::ifstream md5_file(filename); // 打开存储MD5值的文件
    std::string line;
    int count = 0; // 初始化图片序号

    curl = curl_easy_init();
    if (curl)
    {
        while (std::getline(md5_file, line))
        {
            std::string md5_value = line.substr(0, 32); // 分离MD5值

            std::string outfilename = std::to_string(count) + "-" + "person" + ".jpg"; // 构造文件名
            std::string url = "http://192.168.156.222:4869/" + md5_value;              // 构造图片链接
            std::cout << url << std::endl;
            fp = fopen(outfilename.c_str(), "wb");            // 打开图片文件
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // 设置curl的URL
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            res = curl_easy_perform(curl);
            fclose(fp);

            count++; // 更新图片序号
        }
        curl_easy_cleanup(curl);
    }
    md5_file.close(); // 关闭文件

    return 0;
}

int getUrlPic(const char *url, const char *SavePicName)
{
    get_getUrlPicLogs()->info("getUrlPic() inParm: url:{} SavePicName:{}", url, SavePicName);
    CURL *curl;
    FILE *fp = NULL;
    CURLcode res;
    // char url[128] = "http://192.168.156.222:4869/";
    // strncat(url,MD5,32);

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(SavePicName, "wb");
        if (fp == NULL)
        {
            fprintf(stderr, "open file %s failed!\n", SavePicName);
            get_getUrlPicLogs()->error("getUrlPic() fopen():open file:{} failed!", SavePicName);
            curl_easy_cleanup(curl);
            return -1;
        }
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    else
    {
        fprintf(stderr, "getUrlPic() curl_easy_init falied!\n");
        get_getUrlPicLogs()->info("getUrlPic() curl_easy_init falied!");
        return -1;
    }
    return 0;
}
