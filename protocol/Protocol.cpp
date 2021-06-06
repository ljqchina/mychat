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

int Protocol::PackRegisterReq(const RegisterInfo &ri, std::string &msg)
{
    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> w(buf);

    w.StartObject();
    
    PackHeader(w, ri.header);

    w.Key("userid");
    w.String(ri.userId.data());

    //如果是响应报文，则仅设置响应字段
    if(ri.header.msgType == USER_REG_RESP)
    {
        PackHeaderResp(w, ri.header);
        w.EndObject();
        msg = buf.GetString();
        return 0;
    }

    w.Key("nickname");
    w.String(ri.nickName.data());

    w.Key("password");
    w.String(ri.password.data());

    w.Key("sex");
    w.Int(ri.sex);

    w.Key("age");
    w.Int(ri.age);

    w.EndObject();
    msg = buf.GetString();
    return 0;
}

int Protocol::ParseRegisterReq(RegisterInfo &ri, const std::string &msg)
{
    rapidjson::Document doc;
    if(doc.Parse(msg.data()).HasParseError())
        return -1;

    int msgType;
    std::string version;
    if(ParseHeader(doc, ri.header) != 0)
    {
        fprintf(stderr, "Parse Header error from json string:\n");
        fprintf(stderr, "%s\n", msg.data());
        return -1;
    }

    if(!doc.HasMember("userid"))
        return -1;
    ri.userId = doc["userid"].GetString();

    if(!doc.HasMember("nickname"))
        return -1;
    ri.nickName = doc["nickname"].GetString();

    if(!doc.HasMember("password"))
        return -1;
    ri.password = doc["password"].GetString();

    if(!doc.HasMember("sex"))
        return -1;
    ri.sex = doc["sex"].GetInt();

    if(!doc.HasMember("age"))
        return -1;
    ri.age = doc["age"].GetInt();
    return 0; 
}

