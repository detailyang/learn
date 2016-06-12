#include <stdio.h>
#include <string.h>
#include "ngx_config.h"
#include "nginx.h"
#include "ngx_conf_file.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"
#include "ngx_array.h"

#define N 5

volatile ngx_cycle_t *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
    (void)level;
    (void *)log;
    (void)err;
    (void *)fmt;
}

int main()
{
    ngx_str_t test = ngx_string("abcd");
    ngx_str_t a;

    ngx_str_set(&a, "fff");
    printf("data:%s, len:%d", test.data, test.len);
    printf("data:%s, len:%d", a.data, a.len);

    return 0;
}

