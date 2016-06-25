#include "ngx_queue.h"
#include <stdio.h>

/*
 * find the middle queue element if the queue has odd number of elements
 * or the first element of the queue's second part otherwise
 */

volatile ngx_cycle_t *ngx_cycle;
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
}

int main()
{
    ngx_rbtree_t tree;
    ngx_rbtree_node_t sentinel;
    
    ngx_rbtree_init(&tree, &sentinel, ngx_rbtree_insert_value);
    
    ngx_rbtree_node_t rbnode[100];
    int i = 0;
    for (i = 0; i < 100; i ++) {
        rbnode[i].key = i;
        rbnode[i].parent = NULL;
        rbnode[i].left = NULL;
        rbnode[i].right = NULL;
        ngx_rbtree_insert(&tree, &rbnode[i]);
    }
    
    for (i = 0; i < 100; i++) {
        ngx_rbtree_node_t *p = ngx_rbtree_min(tree.root, &sentinel);
        printf("min key%d\r\n", (int)p->key);
        ngx_rbtree_delete(&tree, p);
    }
    
    return 0;
}
