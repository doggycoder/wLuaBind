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

        template <typename T, typename Tuple>
        static T pop(Tuple &tp, size_t pos, void * l);


        //Tuple遍历赋值处理
        template <typename Tuple,size_t N = std::tuple_size<Tuple>::value>
        class TupleTraversal{
        public:
            static void traversal(Tuple& tuple,int ret,void * data){
                using type = typename std::tuple_element<N - 1, Tuple>::type;
                std::get<N-1>(tuple) = State::pop<type>(tuple, N-1, data);
                TupleTraversal<Tuple, N - 1>::traversal(tuple, ret, data);
            }

            static void traversal(Tuple& tuple,void * data){
                using type = typename std::tuple_element<N - 1, Tuple>::type;
                std::get<N-1>(tuple) =  State::pop<type>(tuple, N-1, data);
                TupleTraversal<Tuple , N - 1>::traversal(tuple, data);
            }
        };

        template <typename Tuple>
        class TupleTraversal<Tuple, 1>{
        public:
            static void traversal(Tuple& tuple,int ret,void * data){
                using type = typename std::tuple_element<0, Tuple>::type;
                std::get<0>(tuple) = ret;
            }

            static void traversal(Tuple& tuple,void * data){
                using type = typename std::tuple_element<0, Tuple>::type;
                std::get<0>(tuple) = State::pop<type>(tuple, 0, data);
            }
        };


    public:
        static State * create(LuaLib type = eLL_all);

        ~State();

        int dostring(std::string& lua);
        int dostring(std::string lua);

        int dofile(std::string& path);
        int dofile(std::string path);

        template <typename ... Args,typename ... Params>
        std::tuple<int,Args...> call(std::string name,Params ... p);

        template <typename Clazz,typename ... Params>
        void register_class(const char * name = nullptr);

    };

};

#include "wLuaBinder.inl"

