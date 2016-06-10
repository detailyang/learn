#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static ngx_int_t
ngx_http_header_size_filter(ngx_http_request_t *r);
static ngx_int_t
ngx_http_header_size_filter_init(ngx_conf_t* cf);

ngx_http_module_t  ngx_http_header_size_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_header_size_filter_init,       /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL,                                  /* merge location configuration */
};


ngx_module_t  ngx_http_header_size_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_header_size_filter_module_ctx, /* module context */
    NULL,                                  /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

//定义一个static的next指针
static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

//初始化函数，在配置解析完后
//此函数主要完成header函数过滤链的插入和衔接
static ngx_int_t ngx_http_header_size_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_header_size_filter;

    return NGX_OK;
}

/*
 * 如果说返回给用户的content_length超过1024那么就设置状态码为404
 */
static ngx_int_t
ngx_http_header_size_filter(ngx_http_request_t *r)
{
    if (r->headers_out.content_length_n > 1024)
    {
            r->headers_out.status = NGX_HTTP_NOT_FOUND;
    }
    return ngx_http_next_header_filter(r);
}