#include "Protocol.h"
#include "msgtype.h"
#include "errcode.h"

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

//打包增加长度信息
void Protocol::PackLength(std::string &msg)
{
	char s[10];
	sprintf(s, "%04ld", msg.size());
	msg.insert(0, s); 
}

//解析头部字段从json字符串
int Protocol::ParseHeader(const std::string &msg, Header &h)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(!doc.HasMember("msgtype") || !doc["msgtype"].IsInt())
        return -1;
    h.msgType = doc["msgtype"].GetInt();

    if(!doc.HasMember("msgid"))
        return -1;
    h.msgId = doc["msgid"].GetString();

    if(!doc.HasMember("version"))
        return -1;
    h.version = doc["version"].GetString();
    return 0;
}

//解析头部字段从json对象
int Protocol::ParseHeader(const rapidjson::Document &doc, Header &h)
{
    if(!doc.HasMember("msgtype") || !doc["msgtype"].IsInt())
        return -1;
    h.msgType = doc["msgtype"].GetInt();

    if(!doc.HasMember("msgid"))
        return -1;
    h.msgId = doc["msgid"].GetString();

    if(!doc.HasMember("version"))
        return -1;
    h.version = doc["version"].GetString();
    return 0;
}

//解析头部响应字段从json对象
int Protocol::ParseHeaderResp(const rapidjson::Document &doc, Header &h)
{
    if(!doc.HasMember("respcode") || !doc["respcode"].IsInt())
        return -1;
    h.respCode = doc["respcode"].GetInt();
    return 0;
}

//公共头字段
int Protocol::PackHeader(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h)
{
    w.Key("msgtype");
    w.Int(h.msgType);

    w.Key("msgid");
    w.String(h.msgId.data());

    w.Key("version");
    w.String(h.version.data());

   return 0; 
}

//如果是响应报文,则增加响应字段
int Protocol::PackHeaderResp(rapidjson::Writer<rapidjson::StringBuffer> &w, const Header &h)
{
    w.Key("respcode");
    w.Int(h.respCode);
   return 0; 
}

//打包心跳包消息
int Protocol::PackHeart(const HeartInfo &info, std::string &msg)
{
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();

    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

    //如果是响应报文，则仅设置响应字段
    if(info.header.msgType == SYS_HEART_RESP)
    {
        PackHeaderResp(w, info.header);
        w.EndObject();
        msg = buf.GetString();
		PackLength(msg);
        return 0;
    }

	w.EndObject();
	msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析心跳包消息
int Protocol::ParseHeart(HeartInfo &info, const std::string &msg)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();
    return 0;
}

//打包注册消息
int Protocol::PackRegister(const RegisterInfo &info, std::string &msg)
{
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

    //如果是响应报文，则仅设置响应字段
    if(info.header.msgType == USER_REG_RESP)
    {
        PackHeaderResp(w, info.header);
        w.EndObject();
        msg = buf.GetString();
		PackLength(msg);
        return 0;
    }

    w.Key("nickname");
    w.String(info.nickName.data());

    w.Key("password");
    w.String(info.password.data());

    w.Key("sex");
    w.Int(info.sex);

    w.Key("age");
    w.Int(info.age);

    w.EndObject();
    msg = buf.GetString();
	PackLength(msg);
    return 0;
}

//解析注册消息
int Protocol::ParseRegister(RegisterInfo &info, const std::string &msg)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

	//如果是响应报文，则需要解析响应字段
    if(info.header.msgType == USER_REG_RESP)
    {
		ParseHeaderResp(doc, info.header);
		return 0;
	}

    if(!doc.HasMember("nickname"))
        return -1;
    info.nickName = doc["nickname"].GetString();

    if(!doc.HasMember("password"))
        return -1;
    info.password = doc["password"].GetString();

    if(!doc.HasMember("sex"))
        return -1;
    info.sex = doc["sex"].GetInt();

    if(!doc.HasMember("age"))
        return -1;
    info.age = doc["age"].GetInt();
    return 0; 
}

//打包登录消息
int Protocol::PackLogin(const LoginInfo &info, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

	w.Key("termtype");
	w.Int(info.termType);

    //如果是响应报文，则仅设置响应字段
    if(info.header.msgType == USER_LOGIN_RESP)
    {
        PackHeaderResp(w, info.header);
        w.EndObject();
        msg = buf.GetString();
		PackLength(msg);
        return 0;
    }

	w.Key("password");
	w.String(info.password.data());

    w.EndObject();
    msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析登录消息
int Protocol::ParseLogin(LoginInfo &info, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

    if(!doc.HasMember("termtype"))
        return -1;
    info.termType = doc["termtype"].GetInt();

	//如果是响应报文，则需要解析响应字段
    if(info.header.msgType == USER_LOGIN_RESP)
    {
		ParseHeaderResp(doc, info.header);
	}

	if(!doc.HasMember("password"))
        return -1;
    info.password = doc["password"].GetString();
	return 0;
}

//打包注销消息
int Protocol::PackLogout(const LogoutInfo &info, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

	w.Key("termtype");
	w.Int(info.termType);

    //如果是响应报文，则仅设置响应字段
    if(info.header.msgType == USER_LOGOUT_RESP)
    {
        PackHeaderResp(w, info.header);
        w.EndObject();
        msg = buf.GetString();
		PackLength(msg);
        return 0;
    }

    w.EndObject();
    msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析注销消息
int Protocol::ParseLogout(LogoutInfo &info, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

    if(!doc.HasMember("termtype"))
        return -1;
    info.termType = doc["termtype"].GetInt();

	//如果是响应报文，则需要解析响应字段
    if(info.header.msgType == USER_LOGOUT_RESP)
    {
		ParseHeaderResp(doc, info.header);
	}
	return 0;
}

#if 0
//打包离线消息
int Protocol::PackOfflineMsg(const std::vector<OfflineInfo> &v, const std::string &userId_to, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

	Header h;
	h.msgType = USER_OFFLINEMSG;
	h.version = MYCHAT_VERSION;
	h.msgId = "";

	//设置报文头部
    w.StartObject();
    PackHeader(w, h);
	
	//设置目的用户id
	w.Key("userid_to");
	w.String(userId_to.data());
	
	//设置离线消息数组,系统通知消息与用户离线消息都可发送
	w.Key("msgs");
	w.StartArray();
	for(auto a : v)
	{
		w.StartObject();
		w.Key("userid");
		w.String(a.userId.data());

		w.Key("content");
		w.String(a.content.data());

		w.Key("datetime");
		w.String(a.datetime.data());

		w.Key("type");
		w.Int(a.type);

		//如果离线消息是添加好友响应消息,则设置flag字段
		if(a.type == USER_ADDFRIEND_RESP)
		{
			int flag;
			w.Key("flag");
			flag = atoi(a.resv.data());
			w.Int(flag);
		}
		w.EndObject();
	}
	w.EndArray();

    w.EndObject();
    msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析离线消息
int Protocol::ParseOfflineMsg(std::vector<OfflineInfo> &v, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

	Header h;
	if(ParseHeader(doc, h) != 0)
    {
        fprintf(stderr, "%s Parse Header error from json string:\n", __func__);
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }
	
    if(!doc.HasMember("userid_to"))
        return -1;
	std::string userId_to = doc["userid_to"].GetString();

    if(!doc.HasMember("msgs") || !doc["msgs"].IsArray())
		return -1;

	const rapidjson::Value& array = doc["msgs"];
	size_t len = array.Size();
	for(size_t i = 0; i < len; i++)
	{
		OfflineInfo oi;
		const rapidjson::Value& msgObj = array[i];
		oi.userId = msgObj["userid"].GetString();
		oi.content = msgObj["content"].GetString();
		oi.datetime = msgObj["datetime"].GetString();
		oi.msgType = msgObj["type"].GetInt();
		v.push_back(oi);
	}
	return 0;
}
#endif

//打包离线消息
int Protocol::PackOfflineMsg(std::string &msg)
{
	PackLength(msg);
	return 0;
}

//打包添加好友消息,服务器收到后直接转发
int Protocol::PackAddFriend(const AddFriendInfo &info, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

    w.Key("userid_to");
    w.String(info.userId_to.data());

	if(info.header.msgType == USER_ADDFRIEND_RESP)
	{
		w.Key("flag");
		w.Int(info.flag);
		w.EndObject();
		msg = buf.GetString();
		PackLength(msg);
		return 0;
	}

    w.Key("content");
    w.String(info.content.data());

	w.EndObject();
	msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析添加好友消息,服务器收到后直接转发
int Protocol::ParseAddFriend(AddFriendInfo &info, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

    if(!doc.HasMember("userid_to"))
        return -1;
    info.userId_to = doc["userid_to"].GetString();

	if(info.header.msgType == USER_ADDFRIEND_RESP)
	{
		if(!doc.HasMember("flag"))
			return -1;
		info.flag = doc["flag"].GetInt();
	}
	return 0;
}

//打包删除好友消息
int Protocol::PackDelFriend(const DelFriendInfo &info, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

    w.Key("userid_to");
    w.String(info.userId_to.data());

	//如果是响应报文，则仅设置响应字段
    if(info.header.msgType == USER_DELFRIEND_RESP)
    {
        PackHeaderResp(w, info.header);
    }
	w.EndObject();
	msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析删除好友消息,服务器收到后直接转发到目标用户
int Protocol::ParseDelFriend(DelFriendInfo &info, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

    if(!doc.HasMember("userid_to"))
        return -1;
    info.userId_to = doc["userid_to"].GetString();

	//如果是响应报文，则需要解析响应字段
    if(info.header.msgType == USER_DELFRIEND_RESP)
    {
		ParseHeaderResp(doc, info.header);
	}
	return 0;
}

//打包聊天消息
int Protocol::PackChat(const ChatInfo &info, std::string &msg)
{
	rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    PackHeader(w, info.header);

    w.Key("userid");
    w.String(info.userId.data());

    w.Key("userid_to");
    w.String(info.userId_to.data());

    w.Key("content");
    w.String(info.content.data());

	//如果是响应报文，则仅设置响应字段
	/*
    if(info.header.msgType == USER_CHAT_RESP)
    {
        PackHeaderResp(w, info.header);
    }
	*/
	//打包表情图片信息

	w.EndObject();
	msg = buf.GetString();
	PackLength(msg);
	return 0;
}

//解析聊天消息
int Protocol::ParseChat(ChatInfo &info, const std::string &msg)
{
	rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    if(ParseHeader(doc, info.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    info.userId = doc["userid"].GetString();

    if(!doc.HasMember("userid_to"))
        return -1;
    info.userId_to = doc["userid_to"].GetString();

	//如果是响应报文，则需要解析响应字段
	/*
    if(info.header.msgType == USER_DELFRIEND_RESP)
    {
		ParseHeaderResp(doc, info.header);
	}
	*/
    if(!doc.HasMember("content"))
        return -1;
    info.userId_to = doc["content"].GetString();
	return 0;
}

