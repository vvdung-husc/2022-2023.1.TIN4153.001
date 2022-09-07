#include <stdio.h>
#include "../_COMMON/Log.h"

int main(int argc, char const *argv[])
{
    /* code */
    LOG_D("hello kiiti\n");
    LOG_I("hello kiiti\n");
    LOG_W("hello kiiti\n");
    LOG_E("hello kiiti\n");

    LOG_DT("hello kiiti\n");
    LOG_IT("hello kiiti\n");
    LOG_WT("hello kiiti\n");
    LOG_ET("hello kiiti");

    LOG_D("");
    return 0;
}
