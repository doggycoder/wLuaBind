#include "test/Test.h"
#include  <stdio.h>
#include <iostream>
#include "binder/wLuaBinder.h"

template <typename Tuple,size_t N = std::tuple_size<Tuple>::value>
class TupleTraversal{
public:
    static void traversal(Tuple& tuple){
        using type = typename std::tuple_element<N - 1, Tuple>::type;
        std::cout<<"Tuple("<<N-1<<") :" << std::get<N-1>(tuple) << std::endl;
        TupleTraversal<Tuple, N - 1>::traversal(tuple);
    }
};

template <typename Tuple>
class TupleTraversal<Tuple,1>{
public:
    static void traversal(Tuple& tuple){
        using type = typename std::tuple_element<0, Tuple>::type;
        std::cout<<"Tuple("<<0<<") :" << std::get<0>(tuple) << std::endl;
    }
};

void testParam(int i,int b,char * c,int g){}

class TestParam{
public:
    void testParam(int i,int b,char * c){};
};

int main() {
//    testCCallLua();
//    testLuaCallC();
    testLuaCallCpp();
//    wLua::State * state = wLua::State::create();
//    state->dofile("../res/test3.lua");
//    auto ret = state->call<char *,int,char*,bool>("sayHello","LiMing", 21, true, 10);
//    TupleTraversal<decltype(ret)>::traversal(ret);
//
//    auto ret2 = state->call<char *>("sayHelloToWorld");
//    TupleTraversal<decltype(ret2)>::traversal(ret2);
//
//    auto ret3 = state->call("sayHelloInLua");
//    TupleTraversal<decltype(ret3)>::traversal(ret3);
//
//    delete state;

    // 如何支持类的成员函数获取参数
    std::cout <<"func param size:" <<wLua::get(&TestParam::testParam) <<std::endl;
    std::cout <<"func param size:" <<wLua::get(testParam) <<std::endl;
    return 0;
}