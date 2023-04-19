#include "redis_tool.hpp"

int RedisTool::Connect()
{
	if (connected)
	{
		get_redis_toolLogs()->warn("Connect() already connect");
		return 1;
	}
	pthread_mutex_lock(&redis_tool_mutex); // lock
	if (pRedisContext != NULL)
	{
		redisFree(pRedisContext);
	}
	struct timeval timeout = {2, 0};
	pRedisContext = (redisContext *)redisConnectWithTimeout(address.c_str(), port, timeout);
	if ((NULL == pRedisContext) || (pRedisContext->err))
	{
		get_redis_toolLogs()->error("Connect():redisConnectWithTimeout error!");
		redisFree(pRedisContext);
		pRedisContext = NULL;
		pthread_mutex_unlock(&redis_tool_mutex); // unlock
		return -1;
	}
	std::cout << "Connect complete!\r\n"
			  << std::endl;
	std::cout << "address:" << address << std::endl;
	std::cout << "port:" << port << std::endl;
	get_redis_toolLogs()->info("Connect():Connect complete! address:{} port:{}", address, port);
	// struct timeval cmd_timeout = {30, 0};
	// if (pRedisContext) {
	// 	redisSetTimeout(pRedisContext, cmd_timeout);
	// 	fprintf(stderr, "RedisAdapter::Set cmd timeout 30s!\n");
	// }

	// redisReply *pRedisReply = (redisReply*)redisCommand(pRedisContext, "AUTH %s", password.c_str() );
	// if (pRedisReply == NULL) {
	// 	fprintf(stderr,"AUTH command error\r\n");
	// 	pthread_mutex_unlock(&redis_tool_mutex);//unlock
	//     return -1;
	// }

	// int ret = -1;
	// if (pRedisReply->type == REDIS_REPLY_ERROR  && strcmp(pRedisReply->str, "ERR Client sent AUTH, but no password is set") == 0)
	// {
	//     ret = 0;
	//     connected = true;
	// }
	// else if (pRedisReply->type != REDIS_REPLY_STATUS)
	// {
	//     ret = -1;
	// }
	// else if (strcmp(pRedisReply->str, "OK") == 0)
	// {
	//     ret = 0;
	//     connected = true;
	// 	printf("OK\r\n");
	// }
	// else
	// {
	//     ret = -1;
	// }
	// freeReplyObject(pRedisReply);
	pthread_mutex_unlock(&redis_tool_mutex); // unlock
	return 0;
}

int RedisTool::Close()
{
	pthread_mutex_lock(&redis_tool_mutex); // lock
	if (pRedisContext != NULL)
	{
		redisFree(pRedisContext);
		pRedisContext = NULL;
		std::cout << "Closing connection to Redis" << std::endl;
		get_redis_toolLogs()->info("Close():Closing connection to Redis");
	}
	connected = false;
	pthread_mutex_unlock(&redis_tool_mutex); // unlock
	return 0;
}

int RedisTool::Select(int database_index)
{
	pthread_mutex_lock(&redis_tool_mutex); // lock
	get_redis_toolLogs()->info("Select() inParm:{}", database_index);
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "SELECT %d", database_index);
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "SELECT command error\r\n");
		get_redis_toolLogs()->error("Select():SELECT command error");
		pthread_mutex_unlock(&redis_tool_mutex); // unlock
		return -1;
	}
	std::cout << "Select() pRedisReply:" << pRedisReply->str << std::endl;
	get_redis_toolLogs()->info("Select():redisCommand return:{}", pRedisReply->str);
	pthread_mutex_unlock(&redis_tool_mutex); // unlock
	return 0;
}

// put a meassage map into specific channel
int RedisTool::Publish(const std::string channel, std::map<std::string, std::string> field_string_map)
{
	get_redis_toolLogs()->info("Publish() inParm:channel:{},field_string_map:{}", channel, field_string_map.size());
	if (!pRedisContext)
	{
		fprintf(stderr, "Publish() pRedisContext is NULL");
		get_redis_toolLogs()->error("Publish() pRedisContext is NULL");
		return -1;
	}
	std::string field_string;
	std::string command = "XADD";
	std::string format = "*";

	int command_length = field_string_map.size();
	const char **commandList = new const char *[3 + 2 * command_length];
	commandList[0] = command.c_str();
	commandList[1] = channel.c_str();
	commandList[2] = format.c_str();

	size_t *lengthList = new size_t[3 + 2 * command_length];
	lengthList[0] = command.length();
	lengthList[1] = channel.length();
	lengthList[2] = format.length();

	int index = 3;
	for (std::map<std::string, std::string>::const_iterator iter = field_string_map.begin(); iter != field_string_map.end(); ++iter)
	{
		commandList[index] = iter->first.c_str();
		lengthList[index] = iter->first.length();
		index++;
		commandList[index] = iter->second.c_str();
		lengthList[index] = iter->second.length();
		index++;
	}
	redisReply *pRedisReply = (redisReply *)redisCommandArgv(pRedisContext, index, (const char **)commandList, (const size_t *)lengthList);
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "Publish() error:pRedisReply == NULL");
		get_redis_toolLogs()->error("Publish() pRedisReply == NULL");
		return -1;
	}
	if (pRedisReply->type == REDIS_REPLY_NIL)
	{
		fprintf(stderr, "Publish() errpr:pRedisReply->type == REDIS_REPLY_NIL");
		get_redis_toolLogs()->error("Publish() pRedisReply->type == REDIS_REPLY_NIL");
		freeReplyObject(pRedisReply);
		return -1;
	}
	else
	{
		fprintf(stdout, "Publish() %s:type:%d str:%s\n", channel.c_str(), pRedisReply->type, pRedisReply->str);
		get_redis_toolLogs()->info("Publish() redisCommandArgv() return:stream:{} type:{} str:{}", channel, pRedisReply->type, pRedisReply->str);
		freeReplyObject(pRedisReply);
		return 0;
	}
}

// read channel messages
int RedisTool::XREAD(const std::string channel, int timeout)
{
	get_redis_toolLogs()->info("XREAD() inParm:channel:{} timeout:{}", channel, timeout);
	if (!pRedisContext)
	{
		fprintf(stderr, "XREAD() pRedisContext is NULL");
		get_redis_toolLogs()->error("XREAD() pRedisContext is NULL");
		return -1;
	}
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XREAD BLOCK %d STREAMS %s $", timeout, channel.c_str());
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "XREAD() error:pRedisReply == NULL");
		get_redis_toolLogs()->error("XREAD() pRedisReply == NULL");
		return -1;
	}
	if (pRedisReply->type != REDIS_REPLY_INTEGER)
	{
		fprintf(stderr, "XREAD() errpr:pRedisReply->type == REDIS_REPLY_INTEGER");
		get_redis_toolLogs()->error("XREAD() pRedisReply->type == REDIS_REPLY_INTEGER");
		freeReplyObject(pRedisReply);
		return -1;
	}
	else if (pRedisReply->integer > 0)
	{
		freeReplyObject(pRedisReply);
		return 1;
	}
	else
	{
		freeReplyObject(pRedisReply);
		return 0;
	}
}

// create a group able to consume messages of a channel
int RedisTool::XgroupCreate(std::string channel, std::string group)
{
	get_redis_toolLogs()->info("XgroupCreate() inParm:channel:{} group:{}", channel, group);
	int ret = -1;
	if (!pRedisContext)
	{
		fprintf(stderr, "XgroupCreate() pRedisContext is NULL");
		get_redis_toolLogs()->error("XgroupCreate() pRedisContext is NULL");
		return -1;
	}
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XGROUP CREATE %s %s 0", channel.c_str(), group.c_str());
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "XgroupCreate() error:pRedisReply == NULL");
		get_redis_toolLogs()->error("XgroupCreate() pRedisReply == NULL");
		return -1;
	}
	else if (strcmp(pRedisReply->str, "OK") != 0)
	{
		fprintf(stderr, "XgroupCreate() Create Group %s of %s ERROR!\n", group.c_str(), channel.c_str());
		get_redis_toolLogs()->error("XgroupCreate() Create Group {} of {} ERROR!\n", group, channel);
		freeReplyObject(pRedisReply);
		return -1;
	}
	else
	{
		fprintf(stdout, "XgroupCreate() Create Group %s of %s successfully!\n", group.c_str(), channel.c_str());
		get_redis_toolLogs()->error("XgroupCreate() Create Group {} of {} successfully!\n", group, channel);
		freeReplyObject(pRedisReply);
		ret = 1;
	}
	return ret;
}

// using consuming group read messages of a channel and put key&value into MsgMap
int RedisTool::XgroupRead(std::string channel, std::string group, std::string consumer, std::string &delivered_id, std::map<std::string, std::string> &MsgMap, int number, std::string readFlag)
{
	int ret = -1;
	if (!pRedisContext)
	{
		fprintf(stderr, "XgroupRead() pRedisContext is NULL");
		get_redis_toolLogs()->error("XgroupRead() pRedisContext is NULL");
		return -1;
	}
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XREADGROUP GROUP %s %s count %d STREAMS %s %s", group.c_str(), consumer.c_str(), number, channel.c_str(), readFlag.c_str());
	// fprintf(stderr, "RedisAdapter::Init XREADGROUP GROUP %s %s count %d STREAMS %s %s %s!\n",group.c_str(),consumer.c_str(),number,channel.c_str(),readFlag.c_str(), delivered_id.c_str());
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "XgroupRead() error:redisCommand pRedisReply == NULL:ERROR:%s", pRedisContext->errstr);
		get_redis_toolLogs()->error("XgroupRead() error:redisCommand pRedisReply == NULL:ERROR:{}", pRedisContext->errstr);
		return -1;
	}
	if (pRedisReply->type == REDIS_REPLY_NIL)
	{
		// fprintf(stderr, "RedisAdapter::Group %s get messages of %s null,keep waiting!\n",group.c_str(),channel.c_str());
		freeReplyObject(pRedisReply);
		return 0;
	}
	else if (pRedisReply->type != REDIS_REPLY_ARRAY)
	{
		fprintf(stderr, "XgroupRead() :Group %s get messages of %s ERROR,type:%d,reply :%s!\n", group.c_str(), channel.c_str(), pRedisReply->type, pRedisReply->str);
		get_redis_toolLogs()->error("XgroupRead() Group {} get messages of {} ERROR,type:{} reply:{}", group, channel, pRedisReply->type, pRedisReply->str);
		freeReplyObject(pRedisReply);
		return -1;
	}
	else
	{
		redisReply *msg_struct_0 = pRedisReply->element[0]->element[1];
		if (msg_struct_0->elements == 0)
		{
			fprintf(stdout, "XgroupRead() Group %s get messages of %s null,please use '>' to read\n", group.c_str(), channel.c_str());
			get_redis_toolLogs()->warn("XgroupRead() Group {} get messages of {} null,please use '>' to read\n", group, channel);
			return 2;
		}
		std::string deliverkey = "delivered_id";
		delivered_id = msg_struct_0->element[0]->element[0]->str;
		MsgMap[deliverkey] = delivered_id;
		redisReply *msg_struct_1 = msg_struct_0->element[0]->element[1];
		for (int i = 0; i < msg_struct_1->elements; i += 2)
		{
			std::string key = msg_struct_1->element[i]->str;
			std::string value = msg_struct_1->element[i + 1]->str;
			MsgMap[key] = value;
			fprintf(stderr, "XgroupRead() Group %s get messages of stream:%s key:%s value:%s\n", group.c_str(), channel.c_str(), key.c_str(), value.c_str());
			get_redis_toolLogs()->info("XgroupRead() Group:{} get messages of {} key:{} value:{}\n", group.c_str(), channel.c_str(), key.c_str(), value.c_str());
		}
		ret = 1;
	}
	freeReplyObject(pRedisReply);
	return ret;
}

int RedisTool::XACK(std::string channel, std::string group, std::string delivered_id)
{
	int ret = -1;
	if (!pRedisContext)
	{
		fprintf(stderr, "XACK() pRedisContext is NULL");
		get_redis_toolLogs()->error("XACK() pRedisContext is NULL");
		return -1;
	}
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XACK %s %s %s", channel.c_str(), group.c_str(), delivered_id.c_str());
	if (pRedisReply == NULL)
	{
		fprintf(stderr, "XACK() error:redisCommand pRedisReply == NULL:ERROR:%s", pRedisContext->errstr);
		get_redis_toolLogs()->error("XACK() error:redisCommand pRedisReply == NULL:ERROR:{}", pRedisContext->errstr);
		return -1;
	}
	if (pRedisReply->type == REDIS_REPLY_INTEGER)
	{
		ret = pRedisReply->integer;
		fprintf(stdout, "XACK() redisCommand() return:pRedisPly->type:REDIS_REPLY_INTEGER pRedisReply->integer:%lld\n", pRedisReply->integer);
		get_redis_toolLogs()->info("XACK() redisCommand() return:pRedisPly->type:REDIS_REPLY_INTEGER pRedisReply->integer:{}", pRedisReply->integer);
	}
	else if (pRedisReply->type == REDIS_REPLY_ERROR && strcmp(pRedisReply->str, "ERR Unknown or disabled command 'XACK'") == 0)
	{
		fprintf(stderr, "XACK() Group %s ack message %s of %s error, because this message has already been acked!\n", group.c_str(), delivered_id.c_str(), channel.c_str());
		get_redis_toolLogs()->error("XACK() Group {} ack message {} of {} error, because this message has already been acked!", group, delivered_id, channel);
	}
	else
	{
		fprintf(stderr, "XACK() Group %s ack message %s of %s error, type : %d, str:%s!\n", group.c_str(), delivered_id.c_str(), channel.c_str(), pRedisReply->type, pRedisReply->str);
		get_redis_toolLogs()->error("XACK() Group {} ack message {} of {} error, type:{} str:{}!", group, delivered_id, channel, pRedisReply->type, pRedisReply->str);
	}

	freeReplyObject(pRedisReply);
	return ret;
}

int RedisTool::NextMsgId(const std::string channel, std::string start, std::string end, int count = 1)
{
	int ret = -1;
	if (!pRedisContext)
		return -1;
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XRANGE %s %s %s COUNT %d", channel.c_str(), start.c_str(), end.c_str(), count);
	if (pRedisReply == NULL)
		return -1;
	if (pRedisReply->type == REDIS_REPLY_NIL)
	{
		ret = 0;
	}
	else if (pRedisReply->type == REDIS_REPLY_ARRAY)
	{
		ret = atoi(pRedisReply->element[0]->str);
	}
	freeReplyObject(pRedisReply);
	return ret;
}

int RedisTool::XDEL(std::string channel, std::string delivered_id)
{
	int ret = -1;
	if (!pRedisContext)
		return -1;
	redisReply *pRedisReply = (redisReply *)redisCommand(pRedisContext, "XDEL %s %s", channel.c_str(), delivered_id.c_str());
	if (pRedisReply == NULL)
		return -1;
	else if (pRedisReply->type == REDIS_REPLY_INTEGER)
		ret = pRedisReply->integer;
	return ret;
}

int RedisTool::ConsumeMsg(std::string channel, std::string group, std::string consumer, std::string &delivered_id, std::map<std::string, std::string> &MsgMap, int number, int reconn)
{
	int ret = -1;
	int retry = 0;
	while (ret < 0)
	{
		if (!GetConnected())
		{
			Connect();
		}
		ret = XgroupRead(channel, group, consumer, delivered_id, MsgMap, number);
		if (ret < 0)
		{
			SetConnected(false);
			Close();
		}
		retry++;
		if (retry > reconn)
			break;
	}
	return ret;
}

int RedisTool::DeleteMsg(std::string channel, std::string delivered_id, int reconn)
{
	int ret = -1;
	int retry = 0;
	while (ret < 0)
	{
		if (!GetConnected())
		{
			Connect();
		}
		ret = XDEL(channel, delivered_id);
		if (ret < 0)
		{
			SetConnected(false);
			Close();
		}
		retry++;
		if (retry > reconn)
			break;
	}
	return ret;
}

int RedisTool::ACK(std::string channel, std::string group, std::string delivered_id, int reconn)
{
	int ret = -1;
	int retry = 0;
	while (ret < 0)
	{
		if (!GetConnected())
		{
			Connect();
		}
		ret = XACK(channel, group, delivered_id);
		if (ret < 0)
		{
			SetConnected(false);
			Close();
		}
		retry++;
		if (retry > reconn)
			break;
	}
	return ret;
}
