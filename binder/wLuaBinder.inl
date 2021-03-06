//
// Created by wuwang on 19-7-6.
//

#pragma once

#include <iostream>
#include <typeinfo>
#include <cstring>
#include "TemplateHelper.h"

namespace wLua
{

    template <typename T, typename Tuple>
    T LuaPop<T, Tuple>::pop(Tuple &tp, size_t pos, void *data)
    {
        //优化效率，可以按照堆栈中的顺序获取指定索引的数据，最后统一pop
        auto *l = (lua_State *)data;
        const std::type_info &tid = typeid(T);
        if (tid == typeid(bool))
        {
            int r = lua_toboolean(l, -1);
            lua_pop(l, 1);
            std::cout << "pop bool:" << r << std::endl;
            auto ans = static_cast<T *>(static_cast<void *>(&r));
            return *ans;
        }
        else if (tid == typeid(double) || tid == typeid(float))
        {
            lua_Number r = lua_tonumber(l, -1);
            lua_pop(l, 1);
            auto ans = static_cast<T *>(static_cast<void *>(&r));
            return *ans;
        }
        else if (tid == typeid(int) || tid == typeid(long) || tid == typeid(long long) || tid == typeid(unsigned int) || tid == typeid(unsigned long) || tid == typeid(unsigned long long) || tid == typeid(short) || tid == typeid(unsigned short))
        {
            lua_Integer r = lua_tointeger(l, -1);
            lua_pop(l, 1);
            auto ans = static_cast<T *>(static_cast<void *>(&r));
            return *ans;
        }
        else if (tid == typeid(char *) || tid == typeid(const char *))
        {
            const char *r = lua_tostring(l, -1);
            lua_pop(l, 1);
            auto ans = static_cast<T *>(static_cast<void *>(&r));
            return *ans;
        }
        return 0;
    }

    template <typename Tuple>
    std::string LuaPop<std::string, Tuple>::pop(Tuple &tp, size_t pos, void *data)
    {
        auto *l = (lua_State *)data;
        const char *r = lua_tostring(l, -1);
        lua_pop(l, 1);
        return r;
    }

    template <typename T>
    void State::push(T &t)
    {
        const std::type_info &tid = typeid(t);
        std::cout << tid.name() << std::endl;
        //typeid是运行时判断，而类型转换是编译时检查，所以这里不能直接转，否则会编译报错
        if (tid == typeid(nullptr))
        {
            lua_pushnil(l);
            std::cout << "push nil" << std::endl;
        }
        else if (tid == typeid(int) || tid == typeid(long) || tid == typeid(long long) || tid == typeid(unsigned int) || tid == typeid(unsigned long) || tid == typeid(unsigned long long) || tid == typeid(short) || tid == typeid(unsigned short))
        {
            lua_Integer ans = 0;
            memcpy(&ans, (void *)&t, sizeof(T));
            lua_pushinteger(l, ans);
            std::cout << "push integer" << t << "," << ans << std::endl;
        }
        else
        {
            std::cout << "t is pointer" << std::endl;
        }
    }

    template <typename T, typename... Params>
    void State::push(T &t, Params... p)
    {
        push(t);
        push(p...);
    }

    template <typename... Args, typename... Params>
    std::tuple<int, Args...> State::call(std::string name, Params... p)
    {
        int ret = lua_getglobal(l, name.c_str());
        push(p...);
        int retSize = sizeof...(Args);
        lua_call(l, sizeof...(Params), retSize);
        std::tuple<int, Args...> luaRet;
        TupleTraversal<std::tuple<int, Args...>>::traversal(luaRet, ret, l);
        return luaRet;
    }

    template <typename Clazz, typename... Params>
    State &State::register_class(const char *name)
    {
        check(typeid(Clazz).name());
        lua_pushcfunction(l, [](lua_State *l) -> int {
            //先把参数取出来，后面的操作会导致堆栈变化
            std::tuple<Params...> luaRet;
            TupleTraversal<std::tuple<Params...>>::traversal(luaRet, l);
            auto **pData = (Clazz **)lua_newuserdata(l, sizeof(Clazz *));
            *pData = createClazzWithTuple<Clazz>(luaRet);
            luaL_getmetatable(l, typeid(Clazz).name());

            std::cout << typeid(Clazz).name() << ":cons"
                      << ", oc = " << *pData << std::endl;
            //此时new出来的userdata索引为-1，metatable索引为-2
            //这里就是把-1的metatable设置给-2位置的userdata
            lua_setmetatable(l, -2);
            return 1;
        });
        lua_setglobal(l, name);
        luaL_newmetatable(l, typeid(Clazz).name());
        lua_pushstring(l, "__gc");
        lua_pushcfunction(l, [](lua_State *l) -> int {
            std::cout << "Gc Called" << std::endl;
            delete *(Clazz **)lua_topointer(l, 1);
            return 0;
        });
        lua_settable(l, -3);
        lua_pushstring(l, "__index");
        lua_pushcfunction(l, [](lua_State *l) -> int {
            auto *oc = *(Clazz **)lua_topointer(l, 1);
            auto filed = lua_tostring(l, 2);
            auto ret = lua_getglobal(l, STATE_KEY);
            std::cout << "STATE_KEY : " << luaL_typename(l, lua_type(l, ret)) << std::endl;
            auto *state = *(State **)lua_topointer(l, -1);
            lua_pop(l, 1);

            //如果是调用方法，把函数push进去执行
            auto funMap = state->clazzes[typeid(Clazz).name()].funcs;
            if (funMap.find(filed) != funMap.end())
            {
                std::cout << typeid(Clazz).name() << "::" << filed << ", oc = " << oc << std::endl;
                lua_pushcfunction(l, state->clazzes[typeid(Clazz).name()].funcs[filed]);
                return 1;
            }
            //如果是调用字段，把字段push进去执行
            std::cout << "filed topSize : " << filed << ", top = " << lua_gettop(l) << ", "
                      << lua_type(l, 1) << "-" << lua_type(l, 2) << std::endl;
            auto filedMap = state->clazzes[typeid(Clazz).name()].fileds;
            if (filedMap.find(filed) != filedMap.end())
            {
                state->clazzes[typeid(Clazz).name()].fileds[filed](state, (void *)oc, 0);
                return 1;
            }
            //如果是调用数组
            return 1;
        });
        lua_settable(l, -3);
        return *this;
    }

    template <typename Sig>
    State &State::register_func(Sig sig, const char *name)
    {
        using Clazz = typename get_<Sig>::T;
        using FuncRetType = typename get_<Sig>::retType;
        using ParamTp = typename get_<Sig>::ParamTp;
        check(typeid(Clazz).name());
        std::cout << "register func:" << typeid(Clazz).name() << "::" << typeid(Sig).name() << ", Name = " << name << std::endl;
        clazzes[typeid(Clazz).name()].funcAddrs[typeid(Sig).name()] = *(void **)(&sig);
        clazzes[typeid(Clazz).name()].funcs[name] = [](lua_State *l) -> int {
            lua_getglobal(l, STATE_KEY);
            auto state = *(State **)lua_topointer(l, -1);
            lua_pop(l, 1);
            auto func = (Sig &)state->clazzes[typeid(Clazz).name()].funcAddrs[typeid(Sig).name()];
            auto oc = typeid(Clazz) == typeid(void) ? nullptr : *(Clazz **)lua_topointer(l, 1);
            ParamTp luaRet;
            TupleTraversal<ParamTp>::traversal(luaRet, l);
            std::cout << typeid(Clazz).name() << "-" << typeid(Sig).name() << ", oc = " << oc << std::endl;
            return PushAndReturn<ParamTp, Sig, Clazz, FuncRetType>::pushAndRet(luaRet, oc, func, state);
        };
        if (typeid(Clazz) == typeid(void))
        {
            lua_pushcfunction(l, clazzes[typeid(Clazz).name()].funcs[name]);
            lua_setglobal(l, name);
        }
        return *this;
    }

    template <typename Clazz, typename Type>
    State &State::register_field(Type(Clazz::*filed), const char *name)
    {
        using Sig = Type(Clazz::*);
        check(typeid(Clazz).name());
        std::cout << "register_field Sig: " << typeid(Type(Clazz::*)).name() << std::endl;
        clazzes[typeid(Clazz).name()].filedAddrs[typeid(Sig).name()] = *(void **)&filed;
        clazzes[typeid(Clazz).name()].fileds[name] = [](State *state, void *clazz, int index) {
            auto ar = state->clazzes[typeid(Clazz).name()].filedAddrs[typeid(Sig).name()];
            state->push((Clazz *)clazz->**(Sig *)&ar);
        };
        return *this;
    }

    template <size_t t, typename Clazz, typename Type>
    State &State::register_field(Type (Clazz::*filed)[t], const char *name)
    {
        using Sig = Type(Clazz::*)[t];
        check(typeid(Clazz).name());
        std::cout << "register_field array Sig: " << typeid(Sig).name() << std::endl;
        clazzes[typeid(Clazz).name()].filedAddrs[typeid(Sig).name()] = *(void **)&filed;
        clazzes[typeid(Clazz).name()].fileds[typeid(Sig).name()] = [](State *state, void *clazz, int index) {
            auto ar = state->clazzes[typeid(Clazz).name()].filedAddrs[typeid(Sig).name()];
            if (index >= t || index < 0)
            {
                void *d = nullptr;
                state->push(d);
            }
            else
            {
                auto d = ((Clazz *)clazz->**(Sig *)(&ar))[index];
                state->push(d);
            }
        };
        clazzes[typeid(Clazz).name()].funcs[name] = [](lua_State *l) -> int {
            lua_getglobal(l, STATE_KEY);
            auto state = *(State **)lua_topointer(l, -1);
            lua_pop(l, 1);
            auto oc = typeid(Clazz) == typeid(void) ? nullptr : *(Clazz **)lua_topointer(l, 1);
            lua_Integer index = lua_tointeger(l, 2);
            auto fAddr = state->clazzes[typeid(Clazz).name()].fileds[typeid(Sig).name()];
            if (fAddr)
            {
                fAddr(state, oc, (int)index);
                return 1;
            }
            return 0;
        };
        return *this;
    }
} // namespace wLua