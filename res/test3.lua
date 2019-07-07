-- helloLua.lua文件
myName = "beauty girl"

helloTable = {name = "mutou", IQ = 125}

sex = 'Mr'

function sayHello(name, age, ismale, after)
    if ismale then
        sex = 'Mr'
    else
        sex = 'Ms'
    end
    print("call func sayHello")
    return string.format("Hello, %s %s, after %d years , your are %d years old!", sex, name, after, after+age), after+age, name, ismale
end

function sayHelloToWorld()

    return "Hello Wolrd!"
end
