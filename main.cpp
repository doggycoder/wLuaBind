#include <iostream>
#include <fstream>
#include <sstream>
#include "lua.hpp"

std::string loadString(const std::string &path){
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    return contents;
}

double cFuncAdd(double a, double b){
    return a+b;
}

// 返回值表示函数返回值的个数
int luaBindCFuncAdd(lua_State * l){
    double a = luaL_checknumber(l, 1);
    double b = luaL_checknumber(l, 2);
    lua_pushnumber(l, cFuncAdd(a, b));
    return 1;
}

void testCCallLua(){
    std::cout << "start test C Call Lua --------------------- " << std::endl;
    int ret;
    lua_State * l = luaL_newstate();
    luaL_openlibs(l);
    std::string content = loadString("/home/wuwang/code/wLuaBind/res/test1.lua");
    ret = luaL_dostring(l,content.c_str());
    std::cout<<"doFile : "<< ret<< std::endl;
    ret = lua_getglobal(l, "helloAdd");
    std::cout<<"getFunction : "<< ret<< std::endl;
    lua_pushnumber(l, 10);
    lua_pushnumber(l, 5);
    lua_call(l, 2, 1);
    double iResult = lua_tonumber(l, -1);
    std::cout<<"result:" << iResult << std::endl;
    lua_close(l);
}

void testLuaCallC(){
    std::cout << "start test Lua Call C --------------------- " << std::endl;
    lua_State * l = luaL_newstate();
    luaL_openlibs(l);
    lua_pushcfunction(l,luaBindCFuncAdd);
    lua_setglobal(l,"cFuncAdd");
    int ret = luaL_dostring(l,"print('cFuncAdd ret :', cFuncAdd(98,9))");
    std::cout<<"doFile:" << ret << std::endl;
    lua_close(l);
}

int main() {
    testCCallLua();
    testLuaCallC();
    return 0;
}