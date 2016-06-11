#include "ngx_queue.h"
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


// 自定义 结构体类型
typedef struct
{
    int key;
    char name[32];
    ngx_queue_t link;
}ngx_qTest_t;

// 从大到小的顺序
ngx_int_t cmp(const ngx_queue_t *a, const ngx_queue_t *b)
{
    ngx_qTest_t *aTest = ngx_queue_data(a, ngx_qTest_t, link);
    ngx_qTest_t *bTest = ngx_queue_data(b, ngx_qTest_t, link);
    return aTest->key < bTest->key;
}

void print_queue(ngx_queue_t *h)
{
    ngx_queue_t *t;
    for( t=ngx_queue_head(h); t != ngx_queue_sentinel(h);
    t = ngx_queue_next(t))
    {
        ngx_qTest_t *tmp = ngx_queue_data(t, ngx_qTest_t, link);
        printf("Key:%d, name:%s\n", tmp->key, tmp->name);
    }
}

int main()
{
    ngx_qTest_t arr[5];
    ngx_queue_t head;
    ngx_queue_init(&head);
    int i=0;
    for( ; i<5; ++i)//初始化 队列
    {
        arr[i].key = i;
        sprintf(arr[i].name, "\"My key is:%d.\"", arr[i].key);
        if(i%2)
        {
            ngx_queue_insert_head(&head, &arr[i].link);
        }
        else
        {
            ngx_queue_insert_tail(&head, &arr[i].link);
        }
    }
    printf("*******************************\n");
    printf("Before sort:\n");
    print_queue(&head);
    ngx_queue_sort(&head, cmp);
    printf("*******************************\n");
    printf("After sort:\n");
    print_queue(&head);

    ngx_queue_t * midq = ngx_queue_middle(&head);
    ngx_qTest_t *mid_data = ngx_queue_data(midq, ngx_qTest_t, link);
    printf("*******************************\n");
    // 求取中间节点
    printf("%The middle key is %d.\n", mid_data->key);
    return 0;
}
