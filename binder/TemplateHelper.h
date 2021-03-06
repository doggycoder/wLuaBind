//
// Created by wuwang on 19-7-7.
//

#pragma once

#include <cstdio>
#include <tuple>
#include <iostream>
#include <typeinfo>
#include <functional>

namespace wLua{

    //获取函数、成员函数的参数个数及列表
    template<typename Sig>
    struct get_{
    };

    template<typename R,typename... Args>
    struct get_<R(*)(Args...)> {
        static size_t const value = sizeof...(Args);
        using Func = R(*)(Args...);
        typedef R retType;
        using T = void;
        using ParamTp = std::tuple<Args...>;
    };

    template<typename Clazz,typename R,typename... Args>
    struct get_<R(Clazz::*)(Args...)> {
        static size_t const value = sizeof...(Args);
        using Func = R(Clazz::*)(Args...);
        typedef R retType;
        using T = Clazz;
        using ParamTp = std::tuple<Args...>;
    };

    template<typename Sig>
    inline size_t getParamSize(Sig) {
        return get_<Sig>::value;
    }

    //Num 为tuple参数个数时，Tuple的Index为0，Num推导到0的时候，Tuple的Index为0,1,...Num-1
    //最后有个Num = 0的偏特化，Index就是0,1,...Num-1。后面要使用的就是这个Index
    template< size_t... _Indexes >
    struct IndexTuple{};

    template< std::size_t _Num, typename _Tuple = IndexTuple<> >
    struct Indexes;

    template< std::size_t _Num, size_t... _Indexes >
    struct Indexes<_Num, IndexTuple< _Indexes... > >
            : Indexes< _Num - 1, IndexTuple< _Indexes..., sizeof...(_Indexes) > >
    {
    };

    template<size_t... _Indexes >
    struct Indexes< 0, IndexTuple< _Indexes... > >
    {
        typedef IndexTuple< _Indexes... > __type;
    };

    //函数传入Tuple作为参数的调用
    template<typename Tuple,typename Func, size_t... _Ind>
    typename get_<Func>::retType __callFuncWithTupleParam(Tuple tp,IndexTuple< _Ind... >,Func func)
    {
        return func(std::get< _Ind >(tp)...);
    }

    template<typename Tuple,typename Func>
    typename get_<Func>::retType callFuncWithTupleParam(Tuple tp, Func func){
        return __callFuncWithTupleParam(tp,typename Indexes<std::tuple_size<Tuple>::value>::__type(),func);
    }

    template<typename Tuple,typename Func, typename Clazz ,size_t... _Ind>
    typename get_<Func>::retType __callFuncWithTupleParam(Tuple tp,IndexTuple< _Ind... >,Func func, Clazz *clazz)
    {
        return (clazz->*func)(std::get< _Ind >(tp)...);
    }

    template<typename Tuple,typename Func, size_t... _Ind>
    typename get_<Func>::retType __callFuncWithTupleParam(Tuple tp,IndexTuple< _Ind... >,Func func, void *clazz)
    {
        return func(std::get< _Ind >(tp)...);
    }

    template<typename Tuple,typename Func,typename Clazz>
    typename get_<Func>::retType callFuncWithTupleParam(Tuple tp, Clazz *clazz, Func func){
        return __callFuncWithTupleParam(tp,typename Indexes<std::tuple_size<Tuple>::value>::__type(), func, clazz);
    }

    template<typename Clazz, typename Tuple, size_t... _Ind>
    Clazz * __createClazzWithTuple(Tuple& tp,IndexTuple< _Ind... >)
    {
        return new Clazz(std::get<_Ind>(tp)...);
    }

    template<typename Clazz,typename Tuple>
    Clazz * createClazzWithTuple(Tuple& tp){
        return __createClazzWithTuple<Clazz>(tp,typename Indexes<std::tuple_size<Tuple>::value>::__type());
    }

    template <typename Tuple,size_t N = std::tuple_size<Tuple>::value>
    class LogTuple{
    public:
        static void traversal(Tuple& tuple){
            using type = typename std::tuple_element<N - 1, Tuple>::type;
            std::cout<<"Tuple("<<N-1<<") :" << std::get<N-1>(tuple) << std::endl;
            LogTuple<Tuple, N - 1>::traversal(tuple);
        }
    };

    template <typename Tuple>
    class LogTuple<Tuple,1>{
    public:
        static void traversal(Tuple& tuple){
            using type = typename std::tuple_element<0, Tuple>::type;
            std::cout<<"Tuple("<<0<<") :" << std::get<0>(tuple) << std::endl;
        }
    };

}
