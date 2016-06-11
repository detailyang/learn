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
// 自定义结构体类型
typedef struct node
{
    int id;
    char buf[32];
}Node;

void print_array(ngx_array_t *a)// 遍历输出array
{
    printf("-------------------------------\n");
    Node *p = a->elts;
    size_t i;
    for(i=0; i < a->nelts; ++i)
    {
        printf("%s.\n", (p+i)->buf);
    }
    printf("-------------------------------\n");
}


int main()
{
    ngx_pool_t *pool;
    int i;
    Node *ptmp;
    char str[] = "hello NGX!";
    ngx_array_t *a;
    ngx_log_t log;

    pool = ngx_create_pool(1024, &log);
    printf("Create pool. pool max is %zd\n", pool->max);
    a = ngx_array_create(pool, N, sizeof(Node));// 创建动态数组
    printf("Create array. size=%zd nalloc=%zd\n", a->size, a->nalloc);
    printf("unused memory size is %zd\n", (ngx_uint_t)(pool->d.end -
                    pool->d.last) );
    for(i=0; i<8; ++i)
    {
        ptmp = ngx_array_push(a);// 添加一个元素
        ptmp->id = i+1;
        sprintf(ptmp->buf, "My Id is %d,%s", ptmp->id, str);
    }
    ptmp = ngx_array_push_n(a, 2);// 添加两个元素
    ptmp->id = i+1;
    sprintf(ptmp->buf, "My Id is %d,%s", ptmp->id, str);
    ++ptmp;
    ptmp->id = i+2;
    sprintf(ptmp->buf, "My Id is %d,%s", ptmp->id, str);
    print_array(a);
    printf("unused memory size is %d\n", (ngx_uint_t)(pool->d.end -
                    pool->d.last) );
    ngx_array_destroy(a);
    printf("After destroy array unused memory size is %d\n",
                (ngx_uint_t)(pool->d.end - pool->d.last) );
    ngx_destroy_pool(pool);

    return 0;
}

