#ifndef __M_USER_H__
#define __M_USER_H__

#include "Util.hpp"
#include <mutex>
#include <assert.h>

class User_Table
{
private:
    MYSQL *_mysql;     // 数据库操作句柄
    std::mutex _mutex; // 互斥锁

public:
    User_Table(const std::string &host,
               const std::string &username,
               const std::string &password,
               const std::string &dbname,
               uint16_t port = 3306)
    {
        // 创建操作句柄
        _mysql = MYSQL_Uitl::Create_MYSQL(host, username, password, dbname);
        assert(_mysql != nullptr);
    }

    // 新增用户
    bool insert(Json::Value &root)
    {
        std::string sql = "insert into user values(null, '%s', password('%s'), 0, 0, 0);";
        if (root["password"].isNull() || root["username"].isNull())
        {
            DBG_LOG("输入账户或密码");
            return false;
        }

        char buff[4096];
        sprintf(buff, sql.c_str(), root["username"].asCString(), root["password"].asCString());
        if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
        {
            DBG_LOG("新增用户失败");
            return false;
        }

        return true;
    }

    // 登录验证，并返回详细的用户信息
    bool login(Json::Value &user)
    {
        std::string sql = "select * from user where username='%s', password=password('%s');";
        char buff[4096];
        sprintf(buff, sql.c_str(), user["username"].asCString(), user["password"].asCString());

        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
            {
                DBG_LOG("登录失败");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DBG_LOG("没有该用户");
                return false;
            }
        }

        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DBG_LOG("这个用户不是唯一的");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["score"] = (Json::UInt64)std::stol(row[3]);
        user["total_count"] = std::stoi(row[4]);
        user["win_count"] = std::stoi(row[5]);
        mysql_free_result(res);
        return true;
    }

    // 通过用户名获取用户信息
    bool select_by_name(const std::string &name, Json::Value &user)
    {
        std::string sql = "select * from user where username='%s';";
        char buff[4096];
        sprintf(buff, sql.c_str(), name);
        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
            {
                DBG_LOG("通过用户名获取用户信息失败");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DBG_LOG("没有该用户");
                return false;
            }
        }

        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DBG_LOG("这个用户不是唯一的");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["username"] = name;
        user["score"] = (Json::UInt64)std::stol(row[3]);
        user["total_count"] = std::stoi(row[4]);
        user["win_count"] = std::stoi(row[5]);
        mysql_free_result(res);
        return true;
    }

    // 通过用户id获取用户信息
    bool select_by_id(uint64_t id, Json::Value &user)
    {
        std::string sql = "select * from user where id=%d;";
        char buff[4096];
        sprintf(buff, sql.c_str(), id);
        MYSQL_RES *res = nullptr;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
            {
                DBG_LOG("通过用户id获取用户信息失败");
                return false;
            }
            res = mysql_store_result(_mysql);
            if (res == nullptr)
            {
                DBG_LOG("没有该用户");
                return false;
            }
        }

        int row_num = mysql_num_rows(res);
        if (row_num != 1)
        {
            DBG_LOG("这个用户不是唯一的");
            return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        user["id"] = (Json::UInt64)std::stol(row[0]);
        user["username"] = row[1];
        user["score"] = (Json::UInt64)std::stol(row[3]);
        user["total_count"] = std::stoi(row[4]);
        user["win_count"] = std::stoi(row[5]);
        mysql_free_result(res);
        return true;
    }

    // 胜利时分数增加
    bool win(uint64_t id)
    {
        std::string sql = "update user set score=score+30, total_count=total_count+1, win_count=win_count+1 where id=%d;";
        char buff[4096];
        sprintf(buff, sql.c_str(), id);
        if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
        {
            DBG_LOG("修改胜利者分数失败");
            return false;
        }
        return true;
    }

    // 失败时分数减少
    bool lose(uint64_t id)
    {
        std::string sql = "update user set score=score-30, total_count=total_count+1 where id=%d;";
        char buff[4096];
        sprintf(buff, sql.c_str(), id);
        if (!MYSQL_Uitl::mysql_exec(_mysql, buff))
        {
            DBG_LOG("修改失败者分数失败");
            return false;
        }
        return true;
    }
};

#endif