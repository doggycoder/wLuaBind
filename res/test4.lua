--
-- Created by IntelliJ IDEA.
-- User: wuwang
-- Date: 19-7-6
-- Time: 上午9:08
-- To change this template use File | Settings | File Templates.
--
tp = TestParam("Hello World! I am Lua, registed by wLua.", 13)
tp2 = TestParam("Hello World! I am Lua2 registed by wLua.", 18)
print("test 4 add :", tp2:add(13.0,7.0))
tp:changeNameByStr("Tp1")
tp2:changeName("Tp2__")
print("test 4 sayHello :", tp:sayHello())
print("test 4 .lua exec finished")

