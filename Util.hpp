#ifndef __M_UTIL_H__
#define __M_UTIL_H__

#include "Log.hpp"
#include <fstream>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>
#include <string>
#include <sstream>
#include <memory>
#include <unistd.h>

class MYSQL_Uitl
{
public:
    // 创建MySQL操作句柄并连接MySQL
    static MYSQL *Create_MYSQL(const std::string &host,
                               const std::string &username,
                               const std::string &password,
                               const std::string &dbname,
                               uint16_t port = 3306)
    {
        // 创建句柄
        MYSQL *mysql = mysql_init(nullptr);
        if (mysql == nullptr)
        {
            ERR_LOG("创建MySQL句柄失败");
            return nullptr;
        }

        // 连接服务器
        if (mysql_real_connect(mysql, host.c_str(), username.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0) == nullptr)
        {
            ERR_LOG("连接MySQL服务器失败");
            mysql_close(mysql);
            return nullptr;
        }

        // 设置客户端字符集
        if (mysql_set_character_set(mysql, "utf8") != 0)
        {
            ERR_LOG("设置MySQL客户端字符集失败");
            mysql_close(mysql);
            return nullptr;
        }

        return mysql;
    }

    // 执行mysql指令
    static bool mysql_exec(MYSQL *mysql, const std::string &sql)
    {
        if (mysql_query(mysql, sql.c_str()) != 0)
        {
            ERR_LOG("执行 %s 该指令失败，失败原因为 : %s\n", sql.c_str(), mysql_errno(mysql));
            return false;
        }
        return true;
    }

    // 销毁MySQL句柄
    static void mysql_destroy(MYSQL *mysql)
    {
        if (mysql != NULL)
            mysql_close(mysql);
    }
};

class JSON_Uitl
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
        if (jsw->write(root, &ss) != 0)
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
        if (!jcb->parse(str.c_str(), str.c_str() + str.size(), &root, nullptr))
        {
            ERR_LOG("反序列化失败");
            return false;
        }
        return true;
    }
};

class String_Uitl
{
public:
    // 分割字符串
    static int split(const std::string &str, const std::string &sep, std::vector<std::string> &res)
    {
        size_t pos, idx = 0;
        while (idx < str.size())
        {
            pos = str.find(sep, idx);
            if (pos == std::string::npos)
            {
                res.push_back(str.substr(idx));
                break;
            }
            if (pos == idx)
            {
                idx += sep.size();
                continue;
            }
            res.push_back(str.substr(idx, pos));
            idx = pos + sep.size();
        }
        return res.size();
    }
};

class File_Util
{
public:
    // 读取文件所有内容
    static bool read(const std::string &filename, std::string &body)
    {
        // 打开文件
        std::ifstream ifs(filename, std::ios::binary);
        if (!ifs.is_open())
        {
            ERR_LOG("打开 %s 此文件错误", filename.c_str());
            return false;
        }

        // 获取文件大小
        size_t fsize = 0;
        ifs.seekg(0, std::ios::end);
        fsize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        body.resize(fsize);

        // 读取数据
        ifs.read(&body[0], fsize);
        if (!ifs.good())
        {
            ERR_LOG("读取 %s 此文件错误", filename.c_str());
            ifs.close();
            return false;
        }

        // 关闭文件
        ifs.close();
        return true;
    }
};

#endif