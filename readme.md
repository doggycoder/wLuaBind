
wLuaBind目标是做一个使用更简单的Lua封装。

更多是为了更好的学习C++而创建的实践项目。

## C++调用Lua

C++调用Lua，步骤如下。返回值是一个tuple结构，通过std::get<pos>(tuple)可以获得指定位置的值。
其中0的位置存储的是个int，表示lua函数调用结果。后续依次位模板参数传入的类型。

call函数的模板参数为被调用函数的返回值类型，支持多返回、单返回和无返回。
函数参数第一个为被调用的Lua函数名，后续为Lua函数的参数。

```c++
wLua::State * state = wLua::State::create();

state->dofile("../res/test3.lua");

auto ret = state->call<char *,int,char*,bool>("sayHello","LiMing", 21, true, 10);

auto ret2 = state->call<char *>("sayHelloToWorld");

auto ret3 = state->call("sayHelloInLua");

delete state;
```