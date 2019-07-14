//
// Created by wuwang on 19-7-6.
//

#include "wLuaBinder.h"
#include "lua.hpp"

/***
 * 需要格外注意栈的顺序：
 * 返回值和函数调用都是从左到右依次入栈的，所以出栈的时候，从栈顶pop，则需要注意，先出栈的是右边的参数/返回值。
 * */
namespace wLua{

    State* State::create(wLua::LuaLib type) {
        return new State(type);
    }

    State::State(LuaLib type) {
        l = luaL_newstate();
        if (type == eLL_all){
            luaL_openlibs(l);
        }else{
            auto chk = [&](LuaLib t,int(*func)(lua_State *)) -> int {
                if (type & t){ return func(l);}
                return -1;
            };
            chk(eLL_base,luaopen_base);
            chk(eLL_io,luaopen_io);
            chk(eLL_bit32, luaopen_bit32);
            chk(eLL_string, luaopen_string);
            chk(eLL_os, luaopen_os);
            chk(eLL_package, luaopen_package);
            chk(eLL_math, luaopen_math);
            chk(eLL_coroutine, luaopen_coroutine);
            chk(eLL_table, luaopen_table);
            chk(eLL_debug, luaopen_debug);
            chk(eLL_utf8, luaopen_utf8);
        }
    }

    State::~State() {
        lua_close(l);
        l = nullptr;
    }

    int State::dostring(std::string &lua) {
        return luaL_dostring(l, lua.c_str());
    }

    int State::dostring(std::string lua) {
        return luaL_dostring(l, lua.c_str());
    }

    int State::dofile(std::string &path) {
        return luaL_dofile(l, path.c_str());
    }

    int State::dofile(std::string path) {
        return luaL_dofile(l, path.c_str());
    }



    template<> void State::push(char* &t){
        lua_pushstring(l,t);
    }

    template<> void State::push(const char* &t){
        lua_pushstring(l,t);
    }

    template<> void State::push(std::string &t){
        lua_pushstring(l,t.c_str());
    }

    template<> void State::push(bool &t) {
        lua_pushboolean(l, t);
    }


}
