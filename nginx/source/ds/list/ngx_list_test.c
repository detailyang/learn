//ngx_list_test.c
#include <stdio.h>
#include <string.h>
#include "ngx_config.h"
#include "nginx.h"
#include "ngx_conf_file.h"
#include "ngx_core.h"
#include "ngx_string.h"
#include "ngx_palloc.h"
#include "ngx_list.h"

#define N 5
volatile ngx_cycle_t *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
}

void print_list(ngx_list_t *l)
{

    ngx_list_part_t *p = &(l->part);
    size_t i, n_part=0;
    while(p)
    {
        i=0;
        printf("------------part %d---------------\n", ++n_part);
        for(; i<p->nelts; ++i)
        {
            printf("%s\n", (char*)(((ngx_str_t*)p->elts + i)->data) );
        }
        p = p->next;
    }
    printf("-------------------------------\n");
}


int main()
{
    ngx_pool_t *pool;
    int i;
    char str[] = "hello NGX!";
    ngx_log_t log;
    ngx_list_t *l;

    pool = ngx_create_pool(1024, &log);
    printf("Create pool. pool max is %d\n", pool->max);
    l = ngx_list_create(pool, N, sizeof(ngx_str_t));
    printf("Create list. size=%d nalloc=%d\n", l->size, l->nalloc);
    printf("unused memory size is %d\n", (ngx_uint_t)(pool->d.end -
                    pool->d.last) );
    for(i=0; i<10; ++i)
    {
        ngx_str_t *pstr = ngx_list_push(l);
        char *buf = ngx_palloc(pool, 6*N);
        sprintf(buf, "My Id is %d,%s", i+1, str);

        pstr->len = strlen(buf);
        pstr->data = buf;
    }
    print_list(l);
    printf("unused memory size is %d\n", (ngx_uint_t)(pool->d.end -
                    pool->d.last) );
    ngx_destroy_pool(pool);
    return 0;
}

