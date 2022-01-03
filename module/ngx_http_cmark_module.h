#pragma once

#include <ngx_http.h>

static void *ngx_http_cmark_create_loc_conf(ngx_conf_t *);
static char *ngx_http_cmark_merge_loc_conf(ngx_conf_t *, void *, void *);
static ngx_int_t ngx_http_cmark_handler(ngx_http_request_t *);
static ngx_int_t ngx_http_cmark_init(ngx_conf_t *);
static ngx_int_t ngx_http_cmark_open_file(const ngx_str_t, ngx_file_t *, ngx_log_t *);
static ngx_int_t ngx_http_cmark_read_file(ngx_file_t, u_char *, ngx_log_t *);

typedef struct {

    ngx_flag_t enabled;
    ngx_str_t header;
    ngx_str_t footer;

} ngx_http_cmark_loc_conf_t;
