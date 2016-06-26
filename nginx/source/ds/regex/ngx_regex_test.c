#include "ngx_regex.h"
#include <stdio.h>

/*
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */

volatile ngx_cycle_t *ngx_cycle; // 测试需要
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
}
void
ngx_conf_log_error(ngx_uint_t level, ngx_conf_t *cf, ngx_err_t err,
    const char *fmt, ...)
{
}
char *
ngx_conf_set_flag_slot(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{

}


int main()
{
    ngx_log_t log;
    ngx_regex_compile_t rc;
    u_char                 errstr[NGX_MAX_CONF_ERRSTR];
    ngx_int_t n;
    ngx_str_t x;
    ngx_str_t y;

    ngx_str_set(&x, "qq.com");
    ngx_str_set(&y, "abcd.qq.com");

    ngx_memzero(&rc, sizeof(ngx_regex_compile_t));
    rc.pattern = x;
    rc.pool = ngx_create_pool(1024, &log);
    // rc.err.len = NGX_MAX_CONF_ERRSTR;
    // rc.err.data = errstr;
    ngx_regex_compile(&rc);
    n = ngx_regex_exec(rc.regex, &y, NULL, 0);
    n = ngx_regex_exec(rc.regex, &y, NULL, 0);
    n = ngx_regex_exec(rc.regex, &y, NULL, 0);

    if (n == NGX_REGEX_NO_MATCHED) {
        printf("not matched");

        return 0;
    }

    printf("matched");
}
