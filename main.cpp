#include "main.h"

// 信号处理函数
void stopRunning(int signum)
{
    g_running = 0;
}

int main(int argc, const char *argv[])
{
    int ret;

    ret = logs_init(); // 初始化log
    if (0 != ret)
    {
        return -1;
    }
    faceDetect_init(0.5); // 人脸检测模型导入
    if (0 != ret)
    {
        return -1;
    }
    signal(SIGINT, stopRunning);                                    // 注册信号处理函数
    string delivered_id;                                            // 消息id
    map<string, string> picfield_string_map, replyfield_string_map; // 存储消息字段

    RedisTool redisTool(REDIS_SERVER_IP, REDIS_SERVER_PORT); // 实例化类
    ret = redisTool.Connect();                               // 连接redis服务器
    if (-1 == ret)
    {
        return -1;
    }
    ret = redisTool.Select(DATABASE_INDEX); // 选择服务器数据库
    if (0 != ret)
    {
        return -1;
    }
    string tempPicName(IOT_TO_PIC_CONSUMER); // 临时存储图片名
    tempPicName += ".jpg";
    string readFlag = "0"; // 读取消息方式 0:已经读取但未处理的消息 >:未读取的消息

    while (g_running)
    {
        // 读取未处理的消息
        ret = redisTool.XgroupRead(IOT_TO_PIC_STREAM, IOT_TO_PIC_GROUP, IOT_TO_PIC_CONSUMER, delivered_id, picfield_string_map, 1, readFlag);
        if (1 == ret) // 1:正常返回应答
        {
            ret = getUrlPic(picfield_string_map[IOT_TO_PIC_KEY1].c_str(), tempPicName.c_str()); // 下载图片
            if (0 != ret)
            {
                break;
            }
            clock_t startTime = clock();
            vector<ret_rect> result = faceDetect(tempPicName.c_str()); // 人脸检测
            clock_t endTime = clock();
            double totalTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
            cout << "Detect Time: " << totalTime << "s" << endl;
            cout << "result.num: " << result.size() << endl;
            // 保存检测结果
            replyfield_string_map[REPLY_KEY1] = picfield_string_map[IOT_TO_PIC_KEY1];
            replyfield_string_map[REPLY_KEY2] = picfield_string_map[IOT_TO_PIC_KEY2];
            replyfield_string_map[REPLY_KEY3] = ret_rect_to_json(result);
            ret = redisTool.Publish(REPLY_STREAM, replyfield_string_map); // 推送消息
            if (0 != ret)
            {
                break;
            }
            ret = redisTool.XACK(IOT_TO_PIC_STREAM, IOT_TO_PIC_GROUP, picfield_string_map["delivered_id"]); // 应答
            if (-1 == ret)
            {
                return -1;
            }
        }
        else if (2 == ret) // 2:表示已经没有已经读取但未处理的消息
        {
            readFlag = ">"; // 修改为读取最新消息
        }
        else if(-1 == ret)
        {
            break;
        }
    }
    
    redisTool.Close();  // 断开redis连接
    system("rm *.jpg"); // 清除所有临时缓存图片
    return ret;
}