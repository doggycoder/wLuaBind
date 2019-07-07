//
// Created by wuwang on 19-7-6.
//

#pragma once

#include <string>
#include <tuple>

class lua_State;

namespace wLua {

    enum LuaLib{
        eLL_base = 0x01,
        eLL_io = 0x02,
        eLL_bit32 = 0x04,
        eLL_string = 0x08,
        eLL_os = 0x10,
        eLL_package = 0x20,
        eLL_math = 0x40,
        eLL_coroutine = 0x80,
        eLL_table = 0x100,
        eLL_debug = 0x200,
        eLL_utf8 = 0x400,
        eLL_all = 0xFFFFFFFF
    };

    class State{
    private:
        lua_State * l;

        explicit State(LuaLib type);

        template <typename T,typename ... Params>
        void push(T& t,Params ... p);

        template <typename T>
        void push(T& t);

        //参数为0的情况
        void push(){};

        template <typename T>
        T pop();

        template <typename Tuple,size_t N>
        friend class TupleTraversal;

    public:
        static State * create(LuaLib type = eLL_all);

        ~State();

        int dostring(std::string& lua);
        int dostring(std::string lua);

        int dofile(std::string& path);
        int dofile(std::string path);

        template <typename ... Args,typename ... Params>
        std::tuple<int,Args...> call(std::string name,Params ... p);

    };

};

#include "wLuaBinder.inl"

