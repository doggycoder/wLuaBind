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

    const char * State::STATE_KEY = "__wLuaBinder__State__";

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

        std::cout << "start lua test --------" << std::endl;
        lua_pushstring(l, "Hello , First");
        lua_setglobal(l, "KEY_V");
        lua_pushstring(l, "Hello, Second");
        lua_pushstring(l, "Hello, Third");
        lua_getglobal(l, "KEY_V");
        std::cout << "Print 0: " << lua_tostring(l, -1) << std::endl;
        lua_pop(l,1);
        lua_pushstring(l, "Hello, Forth");
        lua_pushstring(l, "Hello, Five");

        for (int i = 1; i <= lua_gettop(l); ++i) {
            std::cout << "Print 1: " << lua_tostring(l, i) << std::endl;
        }
        std::cout << "end lua test --------" << std::endl;




        auto ** pData = (State**)lua_newuserdata(l, sizeof(State*));
        *pData = this;
        lua_setglobal(l,STATE_KEY);
    }

    State::~State() {
        lua_close(l);
        l = nullptr;
    }

    void State::check(const char *name) {
        auto dt = clazzes.find(name);
        if (dt == clazzes.end()){
            clazzes[name] = RegClazz();
        }
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



    template<>
    void State::push(char* &t){
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

    template<> void State::push(float &t) {
        lua_pushnumber(l, t);
    }

    template<> void State::push(double &t) {
        lua_pushnumber(l, t);
    }


}
