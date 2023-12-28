#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include "Log.hpp"
#include <jsoncpp/json/json.h>
#include <string>
#include <sstream>
#include <memory>

class JSON
{
public:
    // 序列化
    static bool serialize(Json::Value root, std::string &str)
    {
        // 实例化一个 StreamWriteBuilder 对象
        Json::StreamWriterBuilder jsb;
        // 通过 StreamWriteBuilder 类对象生产一个 StreamWrite 对象
        std::unique_ptr<Json::StreamWriter> jsw(jsb.newStreamWriter());
        // 使用 StreamWrite 对象，对Json::Values中的数据序列化
        // 创建字符串流对象接收反序列化后的结果
        std::stringstream ss;
        if(jsw->write(root, &ss) != 0)
        {
            ERR_LOG("序列化失败");
            return false;
        }
        str = ss.str();
        return true;
    }

    // 反序列化
    static bool unserialize(const std::string &str, Json::Value &root)
    {
        // 实例化一个CharReaderBuilder工厂类对象
        Json::CharReaderBuilder jcr;
        // 使用CharReaderBuilder工厂类生产一个CharReader对象
        std::unique_ptr<Json::CharReader> jcb(jcr.newCharReader());
        // 使用CharReader对象进行json格式字符串str的反序列化
        if(!jcb->parse(str.c_str(), str.c_str() + str.size(), &root, nullptr))
        {
            ERR_LOG("反序列化失败");
            return false;
        }
        return true;
    }
};

#endif