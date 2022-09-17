#include <lua.h>
#include <lauxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 30

int **mkmatrix (int rows, int cols) {
    int i, j, count = 1;
    int **m = (int **) malloc (rows * sizeof (int *));
    for (i = 0; i < rows; i++) {
    m[i] = (int *) malloc (cols * sizeof (int));
    for (j = 0; j < cols; j++) {
        m[i][j] = count++;
    }
    }
    return (m);
}

void zeromatrix (int rows, int cols, int **m) {
    int i, j;
    for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++) m[i][j] = 0;
}

void freematrix (int rows, int **m) {
    while (--rows > -1) {
    free (m[rows]);
    }
    free (m);
}

int **mmult (int rows, int cols, int **m1, int **m2, int **m3) {
    int i, j, k, val;
    for (i = 0; i < rows; i++) {
    for (j = 0; j < cols; j++) {
        val = 0;
        for (k = 0; k < cols; k++) {
        val += m1[i][k] * m2[k][j];
        }
        m3[i][j] = val;
    }
    }
    return (m3);
}

int tmain (int n) {
    int i;
    int **m1 = mkmatrix (SIZE, SIZE);
    int **m2 = mkmatrix (SIZE, SIZE);
    int **mm = mkmatrix (SIZE, SIZE);

    for (i = 0; i < n; i++) {
    mm = mmult (SIZE, SIZE, m1, m2, mm);
    }

    freematrix (SIZE, m1);
    freematrix (SIZE, m2);
    freematrix (SIZE, mm);
    return mm[0][0];
}



int test(lua_State *L) {
int N = lua_tointeger(L, 1);
lua_pushinteger(L, tmain(N));
return 1;
}

int luaopen_luacmatrix(lua_State *L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        {"test", test},
        {NULL, NULL},
    };
    luaL_newlib(L, l);
    return 1;
}