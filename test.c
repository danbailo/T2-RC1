#include <stdio.h>
#include <time.h>

struct tm now(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    // printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return tm;
}

int main(){

    struct tm tm = now();

    printf("now: %d-%d-%d %d:%d:%d\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);



    char month[] = "Atualmente estamos no mês %d do ano.\n";
    sprintf(month, "%d\n",month_client);

    return 0;
}