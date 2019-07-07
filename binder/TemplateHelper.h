//
// Created by wuwang on 19-7-7.
//

#pragma once

#include <cstdio>
#include <tuple>
#include <iostream>
#include <typeinfo>
#include "wLuaBinder.h"

namespace wLua{

    class State;

    template <typename Tuple,size_t N = std::tuple_size<Tuple>::value>
    class TupleTraversal{
    public:
        static void traversal(Tuple& tuple,State * state,int ret){
            using type = typename std::tuple_element<N - 1, Tuple>::type;
            std::get<N-1>(tuple) = state->pop<type>();
            TupleTraversal<Tuple, N - 1>::traversal(tuple, state, ret);
        }
    };

    template <typename Tuple>
    class TupleTraversal<Tuple,1>{
    public:
        static void traversal(Tuple& tuple, State * state,int ret){
            using type = typename std::tuple_element<0, Tuple>::type;
            std::get<0>(tuple) = ret;
        }
    };

}
