#include "ngx_hash.h"
#include <stdio.h>


volatile ngx_cycle_t *ngx_cycle; 
void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log,
            ngx_err_t err, const char *fmt, ...)
{
}

int main() { 
    ngx_uint_t k;
    ngx_pool_t* pool; 
    ngx_hash_t* hash; 
    ngx_array_t* elements; 
    ngx_hash_key_t* arr_node; 
    ngx_hash_init_t hash_init; 
    char* find; 
    int i;

    ngx_str_t names[] = {
        ngx_string("a"), 
        ngx_string("b"), 
        ngx_string("c")
    }; 
    char* descs[] = {
        "1.1.1.1",
        "2.2.2.2",
        "3.3.3.3"
    };

    ngx_cacheline_size = 32;
    // hash key cal start
    ngx_str_t str       = ngx_string("hello, world");
    k                   = ngx_hash_key_lc( str.data, str.len);
    pool                = ngx_create_pool(1024*10, NULL);
    printf("caculated key is %u \n", k);
    // hask key cal end
    //
    hash = (ngx_hash_t*) ngx_pcalloc(pool, sizeof(hash));
    hash_init.hash      = hash;                      // hash结构
    hash_init.key       = &ngx_hash_key_lc;          // hash算法函数
    hash_init.max_size  = 1024*10;                   // max_size
    hash_init.bucket_size = 64; // ngx_align(64, ngx_cacheline_size);
    hash_init.name      = "hash_test";          // 在log里会用到
    hash_init.pool           = pool;                 // 内存池
    hash_init.temp_pool      = NULL;

    // 创建数组
    elements = ngx_array_create(pool, 32, sizeof(ngx_hash_key_t));
    for(i = 0; i < 3; i++) {
        arr_node            = (ngx_hash_key_t*) ngx_array_push(elements);
        arr_node->key       = (names[i]);
        arr_node->key_hash  = ngx_hash_key_lc(arr_node->key.data, arr_node->key.len);
        arr_node->value     = (void*) descs[i];
        printf("key: %s , key_hash: %u\n", arr_node->key.data, arr_node->key_hash);
    }

    if (ngx_hash_init(&hash_init, (ngx_hash_key_t*) elements->elts, elements->nelts)!=NGX_OK){
        return 1;
    }

    // 查找
    k    = ngx_hash_key_lc(names[1].data, names[1].len);
    printf("%s key is %d\n", names[1].data, k);
    find = (char*)
    ngx_hash_find(hash, k, (u_char*) names[1].data, names[1].len);

    if (find) {
        printf("value is : %s\n", (char*) find);
    }

    ngx_array_destroy(elements);
    ngx_destroy_pool(pool);

    return 0;
}
