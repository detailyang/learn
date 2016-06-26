#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int luaadd (lua_State *L, int x, int y )
{
    int sum;
    lua_getglobal(L, "add");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_call(L, 2, 1);
    sum = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    return sum;
}

int main(void)
{
    lua_State *L = lua_open();
    luaL_openlibs(L);
    luaL_dofile(L, "./test.lua");
    int a = luaadd(L, 1, 10);
    printf("sum is %d\r\n", a);
    lua_close(L);
    return 0;
}
