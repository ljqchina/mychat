#include "Protocol.h"
#include "msgtype.h"
#include "errcode.h"

Protocol::Protocol()
{
}

Protocol::~Protocol()
{
}

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
    w.Key("resptext");
    w.String(h.respText.data());
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
        return 0;
    }

	w.EndObject();
	msg = buf.GetString();
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
        return 0;
    }

	w.Key("password");
	w.String(info.password.data());

    w.EndObject();
    msg = buf.GetString();
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

	if(!doc.HasMember("password"))
        return -1;
    info.password = doc["password"].GetString();

    if(!doc.HasMember("termtype"))
        return -1;
    info.termType = doc["termtype"].GetInt();
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
        return 0;
    }

    w.EndObject();
    msg = buf.GetString();
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
	return 0;
}

