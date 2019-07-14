#include "test/Test.h"
#include  <stdio.h>
#include <iostream>
#include "binder/wLuaBinder.h"
#include "binder/TemplateHelper.h"

using namespace wLua;

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


class TestParam{
public:
    TestParam(const char * hello, int pos){
        std::cout<<hello<<":" << pos <<std::endl;
    };
    TestParam(double key){
        std::cout<<"create testParam : " << key <<std::endl;
    };
    ~TestParam(){
        std::cout<<"TestParam Gc"<<std::endl;
    }
    void testParam(int i,int b,char * c){};
};

float test_func(int v1, int v2, float v3)
{
    std::cout << "(v1, v2, v3) == "
              << "("  << v1
              << ", " << v2
              << ", " << v3
              << ")"  << std::endl;
    return v1+v2+v3;
}

void testParam(int i,int b,char * c,int g){}

void testCallFuncWithTupleParam(){
//     如何支持类的成员函数获取参数
    std::cout <<"func param size:" << wLua::getParamSize(&TestParam::testParam)<<std::endl;
    std::cout <<"func param size:" <<wLua::getParamSize(testParam) <<std::endl;
    std::tuple< int, int, float> tp{ 100, 200, 3.141593F };
    std::cout << "ret : "<<wLua::callFuncWithTupleParam(tp, test_func)<<std::endl;
    std::tuple<const char *, int> t{"Hello World!", 12};
    auto a = wLua::createClazzWithTuple<TestParam>(t);
    delete a;
}


int main() {
//    testCCallLua();
//    testLuaCallC();
//    testLuaCallCpp();
//    wLua::State * state = wLua::State::create();
//    state->dofile("../res/test3.lua");
//    auto ret = state->call<char *,int,char*,bool>("sayHello","LiMing", 21, true, 10);
//    LogTuple<decltype(ret)>::traversal(ret);
//
//    auto ret2 = state->call<char *>("sayHelloToWorld");
//    LogTuple<decltype(ret2)>::traversal(ret2);
//
//    auto ret3 = state->call("sayHelloInLua");
//    LogTuple<decltype(ret3)>::traversal(ret3);
//
//    delete state;

    // 如何支持类的成员函数获取参数
//    testCallFuncWithTupleParam();

    wLua::State * state = wLua::State::create();

//    state->dofile("../res/test3.lua");
//    auto ret = state->call<char *,int,char*,bool>("sayHello","LiMing", 21, true, 10);
//    LogTuple<decltype(ret)>::traversal(ret);
    state->register_class<TestParam,const char *, int>("TestParam");
    state->dofile("../res/test4.lua");
    delete state;

    return 0;
}