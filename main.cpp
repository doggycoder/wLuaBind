#include "test/Test.h"
#include  <stdio.h>
#include <iostream>
#include "binder/wLuaBinder.h"
#include "binder/TemplateHelper.h"
#include <memory>

using namespace wLua;

class TestParam{
public:
    std::string name;
    int nameInt{0};
    int valueInt{123};
public:
    TestParam(const char * hello, int pos){
        std::cout<<hello<<":" << pos <<std::endl;
    };
//    TestParam(double key){
//        std::cout<<"create testParam : " << key <<std::endl;
//    };

    int add(int i1, int i2){
        return i1+ i2;
    }

    std::string sayHello(){
        return "Hello, "+ name +" , I am C++ . IntName = " + std::to_string(nameInt);
    }

    void changeName(const char * name){
        this->name = name;
    }

    void changeName(int name){
        this->nameInt = name;
    }

    void changeNameByStr(std::string name){
        this->name = std::move(name);
    }

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
    state->register_func(&TestParam::add,"add");
    state->register_func<void(TestParam::*)(int)>(&TestParam::changeName,"changeNameInt");
    state->register_func(&TestParam::changeNameByStr,"changeNameByStr");
    state->register_func(&TestParam::sayHello,"sayHello");
    state->register_func<void(TestParam::*)(const char * c)>(&TestParam::changeName,"changeName");
    state->register_field(&TestParam::nameInt,"nameInt");
    state->register_field(&TestParam::valueInt,"valueInt");
    // key offset is : &((TestParam*)0)->key
    // name offset is : &((TestParam*)0)->name
    //非成员函数注册
    state->register_func(test_func,"test_func");
    state->dofile("../res/test4.lua");

    //下一步去实现重载函数的同名注册。还有成员数据的注册
    //通过lua_gettop判断有多少个参数及参数的基本类型，去调用不同的构造函数

    delete state;


    return 0;
}