#include <stdio.h>
#include "../_COMMON/Log.h"
int main( int argc, char const *argv[])
{
    LOG_D("hello thanhoccho\n");
    LOG_I("hello thanhoccho\n");
    LOG_W("hello thanhoccho\n");
    LOG_E("hello thanhoccho\n");

    LOG_DT(" hello thanhoccho\n");
    LOG_IT(" hello thanhoccho\n");
    LOG_WT(" hello thanhoccho\n");
    LOG_ET(" hello thanhoccho");

    LOG_D("");
    return 0;

}