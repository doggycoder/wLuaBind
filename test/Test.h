//
// Created by wuwang on 19-7-5.
//
#pragma once

#include "lua.hpp"
///

double cFuncAdd(double a, double b);

int luaBindCFuncAdd(lua_State * l);

void testCCallLua();

void testLuaCallC();

void testLuaCallCpp();
