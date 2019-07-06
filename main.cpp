#include "test/Test.h"
#include  <stdio.h>
#include <iostream>

int main() {
    testCCallLua();
    testLuaCallC();
    testLuaCallCpp();
    return 0;
}