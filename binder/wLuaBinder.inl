//
// Created by wuwang on 19-7-6.
//

#pragma once

#include <iostream>
#include "lua.hpp"
#include <typeinfo>
#include <cstring>
#include "TemplateHelper.h"

namespace wLua{

    template <typename T>
    void State::push(T& t){
        const std::type_info &tid = typeid(t);
        std::cout  <<tid.name() << std::endl;
        //typeid是运行时判断，而类型转换是编译时检查，所以这里不能直接转，否则会编译报错
        if(tid.__is_pointer_p()){
            std::cout<<"t is pointer"<<std::endl;
        }else{
            if(tid == typeid(nullptr)){
                lua_pushnil(l);
                std::cout << "push nil" << std::endl;
            }else if(tid == typeid(int)
                || tid == typeid(long)
                || tid == typeid(long long)
                || tid == typeid(unsigned int)
                || tid == typeid(unsigned long)
                || tid == typeid(unsigned long long)
                || tid == typeid(short)
                || tid == typeid(unsigned short)){
                lua_Integer ans = 0;
                memcpy(&ans, (void *)&t, sizeof(t));
                lua_pushinteger(l, ans);
                std::cout << "push integer" << t << "," << ans << std::endl;
            }
        }
    }

    template <typename T,typename ... Params>
    void State::push(T& t,Params ... p){
        push(t);
        push(p...);
    }

    template <typename T>
    T State::pop(){
        const std::type_info& tid = typeid(T);
        if(tid == typeid(bool)){
            int r = lua_toboolean(l,-1);
            lua_pop(l,1);
            auto ans = static_cast<T*>(static_cast<void *>(&r));
            return *ans;
        }else if(tid == typeid(double) || tid == typeid(float)){
            lua_Number r = lua_tonumber(l,-1);
            auto ans = static_cast<T*>(static_cast<void *>(&r));
            return *ans;
        }else if(tid == typeid(int)
                 || tid == typeid(long)
                 || tid == typeid(long long)
                 || tid == typeid(unsigned int)
                 || tid == typeid(unsigned long)
                 || tid == typeid(unsigned long long)
                 || tid == typeid(short)
                 || tid == typeid(unsigned short)){
            lua_Integer r = lua_tointeger(l,-1);
            lua_pop(l,1);
            auto ans = static_cast<T*>(static_cast<void *>(&r));
            std::cout<<"pop int:" << r <<std::endl;
            return *ans;
        }else if(tid == typeid(std::string)){
            const char * r = lua_tostring(l,-1);
            lua_pop(l,1);
            auto ans = static_cast<T*>(static_cast<void *>(&r));
            return *ans;
        }else if(tid == typeid(char *) || tid == typeid(const char *)){
            const char * r = lua_tostring(l,-1);
            lua_pop(l,1);
            auto ans = static_cast<T*>(static_cast<void *>(&r));
            std::cout<<"pop string:" << r <<std::endl;
            return *ans;
        }
        return 0;
    }

    template <typename ... Args,typename ... Params>
    std::tuple<int,Args...> State::call(std::string name,Params ... p){
        int ret = lua_getglobal(l, name.c_str());
        push(p...);
        int retSize = sizeof...(Args);
        lua_call(l, sizeof...(Params), retSize);
        std::tuple<int,Args...> luaRet;
        TupleTraversal<std::tuple<int,Args...>>::traversal(luaRet, this, ret);
        return luaRet;
    }

}