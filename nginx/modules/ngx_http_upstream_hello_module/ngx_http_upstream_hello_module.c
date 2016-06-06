/*
* @Author: detailyang
* @Date:   2016-05-29 22:22:23
* @Last Modified by:   detailyang
* @Last Modified time: 2016-06-06 18:07:22
*/

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char * ngx_http_upstream_hello(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_upstream_hello_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_upstream_hello_create_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_upstream_hello_process_header(ngx_http_request_t *r);
static ngx_int_t ngx_http_upstream_hello_reinit_request(ngx_http_request_t *r);
static void ngx_http_upstream_hello_abort_request(ngx_http_request_t *r);
static void ngx_http_upstream_hello_finalize_request(ngx_http_request_t *r, ngx_int_t rc);
static void *ngx_http_proxy_create_loc_conf(ngx_conf_t *cf);
static ngx_int_t ngx_http_upstream_hello_filter_init(void *data);

typedef struct {
    ngx_http_upstream_conf_t       upstream;
} ngx_http_proxy_loc_conf_t;
static ngx_command_t ngx_http_upstream_hello_commands[] = {
    {
        ngx_string("hello_proxy"),
        NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
        ngx_http_upstream_hello,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },
    ngx_null_command
};

typedef struct {
    size_t                     rest;
    ngx_http_request_t        *request;
    ngx_str_t                  key;
} ngx_http_hello_ctx_t;

static ngx_http_module_t ngx_http_upstream_hello_module_ctx = {
        NULL,                          /* preconfiguration */
        NULL,                          /* postconfiguration */

        NULL,                          /* create main configuration */
        NULL,                          /* init main configuration */

        NULL,                          /* create server configuration */
        NULL,                          /* merge server configuration */

        ngx_http_proxy_create_loc_conf,/* create location configuration */
        NULL                            /* merge location configuration */
};

ngx_module_t ngx_http_upstream_hello_module = {
        NGX_MODULE_V1,
        &ngx_http_upstream_hello_module_ctx,    /* module context */
        ngx_http_upstream_hello_commands,       /* module directives */
        NGX_HTTP_MODULE,               /* module type */
        NULL,                          /* init master */
        NULL,                          /* init module */
        NULL,                          /* init process */
        NULL,                          /* init thread */
        NULL,                          /* exit thread */
        NULL,                          /* exit process */
        NULL,                          /* exit master */
        NGX_MODULE_V1_PADDING
};

static void *
ngx_http_proxy_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_proxy_loc_conf_t  *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_proxy_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    /*
     * set by ngx_pcalloc():
     *
     *     conf->upstream.bufs.num = 0;
     *     conf->upstream.ignore_headers = 0;
     *     conf->upstream.next_upstream = 0;
     *     conf->upstream.cache_zone = NULL;
     *     conf->upstream.cache_use_stale = 0;
     *     conf->upstream.cache_methods = 0;
     *     conf->upstream.temp_path = NULL;
     *     conf->upstream.hide_headers_hash = { NULL, 0 };
     *     conf->upstream.uri = { 0, NULL };
     *     conf->upstream.location = NULL;
     *     conf->upstream.store_lengths = NULL;
     *     conf->upstream.store_values = NULL;
     *     conf->upstream.ssl_name = NULL;
     *
     *     conf->method = { 0, NULL };
     *     conf->headers_source = NULL;
     *     conf->headers.lengths = NULL;
     *     conf->headers.values = NULL;
     *     conf->headers.hash = { NULL, 0 };
     *     conf->headers_cache.lengths = NULL;
     *     conf->headers_cache.values = NULL;
     *     conf->headers_cache.hash = { NULL, 0 };
     *     conf->body_lengths = NULL;
     *     conf->body_values = NULL;
     *     conf->body_source = { 0, NULL };
     *     conf->redirects = NULL;
     *     conf->ssl = 0;
     *     conf->ssl_protocols = 0;
     *     conf->ssl_ciphers = { 0, NULL };
     *     conf->ssl_trusted_certificate = { 0, NULL };
     *     conf->ssl_crl = { 0, NULL };
     *     conf->ssl_certificate = { 0, NULL };
     *     conf->ssl_certificate_key = { 0, NULL };
     */

    // conf->upstream.store = NGX_CONF_UNSET;
    // conf->upstream.store_access = NGX_CONF_UNSET_UINT;
    // conf->upstream.next_upstream_tries = NGX_CONF_UNSET_UINT;
    // conf->upstream.buffering = NGX_CONF_UNSET;
    // conf->upstream.request_buffering = NGX_CONF_UNSET;
    // conf->upstream.ignore_client_abort = NGX_CONF_UNSET;
    // conf->upstream.force_ranges = NGX_CONF_UNSET;

    // conf->upstream.local = NGX_CONF_UNSET_PTR;

    conf->upstream.connect_timeout = 60000;
    conf->upstream.send_timeout = 60000;
    conf->upstream.read_timeout = 60000;
    conf->upstream.buffer_size = ngx_pagesize;
    // conf->upstream.next_upstream_timeout = 60000;

    // conf->upstream.send_lowat = NGX_CONF_UNSET_SIZE;
    // conf->upstream.buffer_size = NGX_CONF_UNSET_SIZE;
    // conf->upstream.limit_rate = NGX_CONF_UNSET_SIZE;

    // conf->upstream.busy_buffers_size_conf = NGX_CONF_UNSET_SIZE;
    // conf->upstream.max_temp_file_size_conf = NGX_CONF_UNSET_SIZE;
    // conf->upstream.temp_file_write_size_conf = NGX_CONF_UNSET_SIZE;

    // conf->upstream.pass_request_headers = NGX_CONF_UNSET;
    // conf->upstream.pass_request_body = NGX_CONF_UNSET;

#if (NGX_HTTP_CACHE)
    // conf->upstream.cache = NGX_CONF_UNSET;
    // conf->upstream.cache_min_uses = NGX_CONF_UNSET_UINT;
    // conf->upstream.cache_bypass = NGX_CONF_UNSET_PTR;
    // conf->upstream.no_cache = NGX_CONF_UNSET_PTR;
    // conf->upstream.cache_valid = NGX_CONF_UNSET_PTR;
    // conf->upstream.cache_lock = NGX_CONF_UNSET;
    // conf->upstream.cache_lock_timeout = NGX_CONF_UNSET_MSEC;
    // conf->upstream.cache_lock_age = NGX_CONF_UNSET_MSEC;
    // conf->upstream.cache_revalidate = NGX_CONF_UNSET;
    // conf->upstream.cache_convert_head = NGX_CONF_UNSET;
#endif

    // conf->upstream.hide_headers = NGX_CONF_UNSET_PTR;
    // conf->upstream.pass_headers = NGX_CONF_UNSET_PTR;

    // conf->upstream.intercept_errors = NGX_CONF_UNSET;

// #if (NGX_HTTP_SSL)
//     conf->upstream.ssl_session_reuse = NGX_CONF_UNSET;
//     conf->upstream.ssl_server_name = NGX_CONF_UNSET;
//     // conf->upstream.ssl_verify = NGX_CONF_UNSET;
// #endif
//     conf->upstream.cyclic_temp_file = 0;
//     conf->upstream.change_buffering = 1;
    conf->upstream.cyclic_temp_file = 0;
    conf->upstream.buffering = 0;
    conf->upstream.ignore_client_abort = 0;
    conf->upstream.send_lowat = 0;
    conf->upstream.bufs.num = 0;
    conf->upstream.busy_buffers_size = 0;
    conf->upstream.max_temp_file_size = 0;
    conf->upstream.temp_file_write_size = 0;
    conf->upstream.intercept_errors = 1;
    conf->upstream.intercept_404 = 1;
    conf->upstream.pass_request_headers = 0;
    conf->upstream.pass_request_body = 0;

    // ngx_str_set(&conf->upstream.module, "upstream_hello");

    return conf;
}

static char *
ngx_http_upstream_hello(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;
    ngx_http_proxy_loc_conf_t *plcf = conf;
    ngx_url_t                  u;
    ngx_str_t                  *value;

    if (plcf->upstream.upstream) {
        return "is duplicate";
    }

    value = cf->args->elts;

    ngx_memzero(&u, sizeof(ngx_url_t));

    u.url = value[1];
    u.no_resolve = 1;

    plcf->upstream.upstream = ngx_http_upstream_add(cf, &u, 0);
    if (plcf->upstream.upstream == NULL) {
        ngx_conf_log_error(NGX_LOG_EMERG, cf, 0,
                           "add upstream error");
        return NGX_CONF_ERROR;
    }

    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_upstream_hello_handler;

    if (clcf->name.data[clcf->name.len - 1] == '/') {
        clcf->auto_redirect = 1;
    }

    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_upstream_hello_handler(ngx_http_request_t *r)
{
    ngx_int_t                   rc;
    ngx_http_upstream_t        *u;
    ngx_http_proxy_loc_conf_t  *plcf;
    ngx_http_hello_ctx_t *ctx;

    rc = ngx_http_discard_request_body(r);

    if (rc != NGX_OK) {
        return rc;
    }

    if (ngx_http_set_content_type(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (ngx_http_upstream_create(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    u = r->upstream;

    ngx_str_set(&u->schema, "http://");
    u->output.tag = (ngx_buf_tag_t) &ngx_http_upstream_hello_module;
    plcf = ngx_http_get_module_loc_conf(r, ngx_http_upstream_hello_module);

    u->conf = &plcf->upstream;

    /* attach the callback functions */
    u->create_request = ngx_http_upstream_hello_create_request;
    u->reinit_request = ngx_http_upstream_hello_reinit_request;
    u->process_header = ngx_http_upstream_hello_process_header;
    u->abort_request = ngx_http_upstream_hello_abort_request;
    u->finalize_request = ngx_http_upstream_hello_finalize_request;

    ctx = ngx_palloc(r->pool, sizeof(ngx_http_hello_ctx_t));
    if (ctx == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ctx->request = r;

    ngx_http_set_ctx(r, ctx, ngx_http_upstream_hello_module);

    u->input_filter_init = ngx_http_upstream_hello_filter_init;
    u->input_filter_ctx = ctx;

    r->main->count++;

    ngx_http_upstream_init(r);

    return NGX_DONE;
}

static void
ngx_http_upstream_hello_finalize_request(ngx_http_request_t *r, ngx_int_t rc)
{
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
               "finalize http proxy request");
    return;
}

static void
ngx_http_upstream_hello_abort_request(ngx_http_request_t *r)
{
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
               "abort http hello proxy request");
    return;
}

static ngx_int_t
ngx_http_upstream_hello_reinit_request(ngx_http_request_t *r)
{
    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
               "reinit http hello proxy request");
    return NGX_OK;
}

static ngx_int_t
ngx_http_upstream_hello_create_request(ngx_http_request_t *r)
{
    /* make a buffer and chain */
    ngx_buf_t *b;
    ngx_chain_t *cl;

    char *request = "GET / HTTP/1.1\r\n\r\n";

    b = ngx_create_temp_buf(r->pool, 20);
    if (b == NULL)
        return NGX_ERROR;

    cl = ngx_alloc_chain_link(r->pool);
    if (cl == NULL)
        return NGX_ERROR;

    /* hook the buffer to the chain */
    cl->buf = b;
    cl->next = NULL;
    /* chain to the upstream */
    r->upstream->request_bufs = cl;

    /* now write to the buffer */
    b->pos = (u_char *)request;
    b->last = b->pos + 20;

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "fuck\r\n");

    return NGX_OK;
}

static ngx_int_t
ngx_http_upstream_hello_process_header(ngx_http_request_t *r)
{
    ngx_http_upstream_t       *u;

    u = r->upstream;

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "uuuuuuuuuuuuuuuuuu");
    // /* read the first character */
    // switch(u->buffer.pos[0]) {
    //     case '?':
    //         r->header_only = 1;  suppress this buffer from the client
    //         u->headers_in.status_n = 404;
    //         break;
    //     case ' ':
    //         break;
    // }

    u->headers_in.content_length_n = 0;
    u->headers_in.status_n = 200;
    u->state->status = 200;
    u->buffer.pos++; /* move the buffer to point to the next character */

    return NGX_OK;
}

static ngx_int_t
ngx_http_upstream_hello_filter_init(void *data) {
    ngx_http_hello_ctx_t  *ctx = data;

    ngx_http_upstream_t  *u;

    u = ctx->request->upstream;

    ngx_log_error(NGX_LOG_ERR, ctx->request->connection->log, 0, "fffffffff");

    u->length = 0;

    return NGX_OK;
}

