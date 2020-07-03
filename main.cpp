#include "scope_exit.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <algorithm>


void ScopeExitReturnTest()
{
    char* str = strdup("Hello!");
    SCOPE_EXIT
    {
        free(str);
        puts("ScopeExitReturnTest() free called!");
    };

    if(2 + 2 == 4)
        return;
}

void ScopeExitDeallocatingMatrix()
{
    char* s1 = strdup("KOPOBA");
    char* s2 = strdup("MOLOKO");
    SCOPE_EXIT
    {
        free(s1);
        free(s2);
        puts("free(s1) and free(s2) called!");
    };

    // allocating matrix of size (strlen(s1) + 1) x (strlen(s2) + 1)
    const size_t countRows = strlen(s1) + 1;
    const size_t countColumns = strlen(s2) + 1;

    int** mtx = (int**)calloc(countRows, sizeof(int*));
    int* const* lastRow = mtx + countRows;
    for(int** row = mtx; row != lastRow; ++row)
        *row = (int*)calloc(countColumns, sizeof(int));

    SCOPE_EXIT
    {
        // deallocating matrix
        for(int** row = mtx; row != lastRow; ++row)
            free(*row);

        free(mtx);
        puts("ScopeExitDeallocatingMatrix() matrix deallocated!");
    };

    // Wagner-Fischer algorithm
    mtx[0][0] = 0;

    for(int row = 1; row < countRows; ++row)
        mtx[row][0] = row;

    for(int col = 1; col < countColumns; ++col)
        mtx[0][col] = col;

    for(int i = 1; i < countRows; ++i)
    {
        for(int j = 1; j < countColumns; ++j)
        {
            if(s1[i - 1] == s2[j - 1])
                mtx[i][j] = mtx[i - 1][j - 1];
            else
                mtx[i][j] = std::min(mtx[i - 1][j], std::min(mtx[i][j - 1], mtx[i - 1][j - 1])) + 1;
        }
    }

    // printing matrix
    printf("[ ");
    for(const int* const* row = mtx; row != lastRow; ++row)
    {
        const int* const lastColumn = *row + countColumns;
        printf(row == mtx ? "[ " : "  [ ");
        for(const int* col = *row; col != lastColumn; ++col)
            printf(col == lastColumn - 1 ? "%d ]" : "%d, ", *col);

        if(row != lastRow - 1) putchar((int)'\n');
    }
    puts(" ]");

    printf("Levenshtein distance between '%s' and '%s' is %d\n",
            s1, s2, mtx[countRows - 1][countColumns - 1]);
}

void ScopeExitTrowingExceptionTest()
{
    char* str = strdup("Hello!");
    SCOPE_EXIT
    {
        free(str);
        puts("ScopeExitTrowingExceptionTest() free called!");
    };

    throw std::runtime_error("Unexpected exception!");
}


int main()
{
    ScopeExitReturnTest();
    try { ScopeExitTrowingExceptionTest(); } catch(...){}
    ScopeExitDeallocatingMatrix();
    return 0;
}
