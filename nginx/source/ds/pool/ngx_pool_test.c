//ngx_pool_test.c
#include <stdio.h>
#include <string.h>
#include "ngx_config.h"//包含相关 nginx 头文件
#include "nginx.h"
#include "ngx_conf_file.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"

volatile ngx_cycle_t *ngx_cycle; // 测试需要
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
}
// 自定义结构体类型
typedef struct demo_s
{
    int key;
    char *name;
}demo_t;

void pool_blocks(ngx_pool_t *pool)
{
    int n = 0;
    ngx_pool_t *pool_head = pool;
    while(pool)
    {
        printf("Block %d:\n", n+1);
        printf("block addr = 0x%0x\n", pool);
        printf("  .current = 0x%0x\n", pool_head->current);
        printf("unused memory size is %d\n", (ngx_uint_t)(pool->d.end -
                        pool->d.last));
        printf("Block %d failed %d\n", n+1, pool->d.failed);
        pool = pool->d.next;
        ++n;
    }
    printf("-------------------------------\n");
}


int main()
{
    ngx_pool_t *pool;
    demo_t *demo;
    char name[] = "hello NGX!";
    char *buf;
    ngx_log_t log;
    pool = ngx_create_pool(1024, &log);
    printf("pool max is %d\n\n", pool->max);
    pool_blocks(pool);
    demo = ngx_palloc(pool, sizeof(demo_t));
    buf = ngx_palloc(pool, strlen(name)+1);
    demo->key = 1;
    demo->name = buf;
    strcpy(buf, name);
    printf("Data\n");
    printf("demo->key=%d, demo->name=%s\n", demo->key, demo->name);
    pool_blocks(pool);

    ngx_palloc(pool, 970);
    pool_blocks(pool);

    ngx_palloc(pool, 970);
    pool_blocks(pool);

    ngx_destroy_pool(pool);
    return 0;
}
