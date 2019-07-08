//
// Created by wuwang on 19-7-5.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "Test.h"

/**---------------------------------------------------------------*/
/// 辅助类函数
std::string loadString(const std::string &path) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string contents(buffer.str());
    return contents;
}

/**---------------------------------------------------------------*/
/// C调用Lua的示例
/**
1. 使用lua_getglobal来获取函数，然后将其压入栈；
2. 如果这个函数有参数的话，就需要依次将函数的参数也压入栈；
3. 这些准备工作都准备就绪以后，就调用lua_pcall开始调用函数了，调用完成以后，会将返回值压入栈中；
4. 最后取返回值，调用完毕。
*/
void testCCallLua(){
    std::cout << "start test C Call Lua --------------------- " << std::endl;
    int ret;
    lua_State * l = luaL_newstate();
    luaL_openlibs(l);
    std::string content = loadString("../res/test1.lua");
    ret = luaL_dostring(l,content.c_str());
//    ret = luaL_dofile(l, "../res/test1.lua");
    std::cout<<"doFile : "<< ret<< std::endl;
    ret = lua_getglobal(l, "helloAdd");
    std::cout<<"getFunction : "<< ret<< std::endl;
    lua_pushnumber(l, 10);
    lua_pushnumber(l, 5);
    lua_call(l, 2, 1);
    double iResult = lua_tonumber(l, -1);
    lua_pop(l,1);
    std::cout<<"result:" << iResult << std::endl;
    lua_close(l);
}

/**---------------------------------------------------------------*/
/// Lua 调用C的示例
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

/**---------------------------------------------------------------*/
/// Lua 调用C++的示例

class OperateCpp{
private:
    double x{};
    double y{};
    int type{};
public:

    int errorCode = -1;

    OperateCpp() = default;
    ~OperateCpp(){
        std::cout<<"OperateCpp destroy"<<std::endl;
    }

    double multiply(double x, double y){
        return x * y;
    }
};

static int LuaCreateOperateCpp(lua_State * l){
    auto ** pData = (OperateCpp**)lua_newuserdata(l, sizeof(OperateCpp*));
    *pData = new OperateCpp();
    luaL_getmetatable(l, "OperateCppMeta");
    lua_setmetatable(l, -2);
    return 1;
}

static int LuaDestroyOperateCpp(lua_State* L){
    // 释放对象
    delete *(OperateCpp**)lua_topointer(L, 1);
    return 0;
}

static int LuaFuncMultiply(lua_State * l){
    auto * oc = *(OperateCpp **)lua_topointer(l, 1);
    auto x = lua_tonumber(l,2);
    auto y = lua_tonumber(l,3);
    auto ret = oc->multiply(x,y);
    lua_pushnumber(l,ret);
    return 1;
}

static int LuaCallIndex(lua_State * l){
    auto * oc = *(OperateCpp **)lua_topointer(l, 1);
    auto filed = lua_tostring(l,2);
    if(strcmp(filed,"errorCode") == 0){
        lua_pushnumber(l, oc->errorCode);
    }else if(strcmp(filed, "multiply") == 0){
        lua_pushcfunction(l, LuaFuncMultiply);
    }
    return 1;
}

void testLuaCallCpp(){
    std::cout << "start test Lua Call Cpp --------------------- " << std::endl;
    lua_State * l = luaL_newstate();
    luaL_openlibs(l);
//    lua_pushcfunction(l,LuaCreateOperateCpp);
    lua_pushcfunction(l,[](lua_State * l) -> int{
        auto ** pData = (OperateCpp**)lua_newuserdata(l, sizeof(OperateCpp*));
        *pData = new OperateCpp();
        luaL_getmetatable(l, "OperateCppMeta");
        lua_setmetatable(l, -2);
        return 1;
    });
    lua_setglobal(l,"OperateCpp");
    luaL_newmetatable(l, "OperateCppMeta");
    lua_pushstring(l,"__gc");
    lua_pushcfunction(l,LuaDestroyOperateCpp);
    lua_settable(l, -3);
    lua_pushstring(l, "__index");
    lua_pushcfunction(l,LuaCallIndex);
    lua_settable(l,-3);

    std::string content = loadString("../res/test2.lua");
    int ret = luaL_dostring(l,content.c_str());
    std::cout<<"doFile:" << ret << std::endl;
    lua_close(l);
}



