//
// Created by wuwang on 19-7-6.
//

#pragma once

#include "lua.hpp"
#include <string>
#include <tuple>
#include <functional>
#include <map>
#include "TemplateHelper.h"

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

    struct RegClazz{
        std::map<std::string,lua_CFunction> funcs;
    };


    template <typename T, typename Tuple>
    class LuaPop{
    public:
        static T pop(Tuple &tp, size_t pos, void * l);
    };

    template <typename Tuple>
    class LuaPop<std::string,Tuple> {
    public:
        static std::string pop(Tuple &tp, size_t pos, void *l);
    };

    class State{
    private:
        lua_State * l;
        std::map<const char *,RegClazz> clazzes;
        std::map<const char *,void *> funcAddrs;
        static const char * STATE_KEY;

        explicit State(LuaLib type);

        template <typename T,typename ... Params>
        void push(T& t,Params ... p);

        void check(const char * name);

        template <typename T>
        void push(T& t);

        //参数为0的情况
        void push(){};

        //Tuple遍历赋值处理
        template <typename Tuple,size_t N = std::tuple_size<Tuple>::value>
        class TupleTraversal{
        public:
            static void traversal(Tuple& tuple,int ret,void * data){
                using type = typename std::tuple_element<N - 1, Tuple>::type;
                std::get<N-1>(tuple) = LuaPop<type,Tuple>::pop(tuple, N-1, data);
                TupleTraversal<Tuple, N - 1>::traversal(tuple, ret, data);
            }

            static void traversal(Tuple& tuple,void * data){
                using type = typename std::tuple_element<N - 1, Tuple>::type;
                std::get<N-1>(tuple) =  LuaPop<type,Tuple>::pop(tuple, N-1, data);
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
                std::get<0>(tuple) = LuaPop<type,Tuple>::pop(tuple, 0, data);
            }
        };

        template <typename Tuple>
        class TupleTraversal<Tuple, 0>{
        public:
            static void traversal(Tuple& tuple,int ret,void * data){
            }

            static void traversal(Tuple& tuple,void * data){
            }
        };


        template<typename Tuple,typename Func,typename Clazz,typename RetType>
        class PushAndReturn{
        public:
            static int pushAndRet(Tuple tp, Clazz *clazz, Func func,State * state){
                RetType ret = callFuncWithTupleParam(tp,clazz,func);
                state->push(ret);
                return 1;
            }
        };

        template<typename Tuple,typename Func,typename Clazz>
        class PushAndReturn<Tuple,Func,Clazz,void>{
        public:
            static int pushAndRet(Tuple tp, Clazz *clazz, Func func,State * state){
                callFuncWithTupleParam(tp,clazz,func);
                return 0;
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

        template <typename Sig>
        void register_func(Sig,const char * name = nullptr);

        template <typename Clazz>
        void register_field(const char *name);

    };

};

#include "wLuaBinder.inl"

