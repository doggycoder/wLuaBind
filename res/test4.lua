--
-- Created by IntelliJ IDEA.
-- User: wuwang
-- Date: 19-7-6
-- Time: 上午9:08
-- To change this template use File | Settings | File Templates.
--
tp = TestParam("Hello World! I am Lua, registed by wLua.", 13)
print("test 1 add :", tp:add(134.0,7.0))
tp:changeNameInt(186)
tp:changeName("Tp1")
tp2 = TestParam("Hello World! I am Lua2 registed by wLua.", 18)
print("test 4 add :", tp2:add(13.0,7.0))
tp2:changeName("Tp2__")
print("test 4 sayHello :", tp:sayHello())
print("test 4 .lua exec finished")
print("test_func : " + test_func(12,25, 123))

