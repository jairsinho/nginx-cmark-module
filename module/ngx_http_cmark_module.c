#include "ngx_http_cmark_module.h"

#include "cmark-gfm.h"
#include "cmark-gfm-core-extensions.h"
#include "cmark-gfm-extension_api.h"
#include "parser.h"
#include "registry.h"

static ngx_command_t  ngx_http_cmark_commands[] = {

    {
        ngx_string("cmark"),
        NGX_HTTP_LOC_CONF | NGX_HTTP_LIF_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_cmark_loc_conf_t, enabled),
        NULL
    },
    {
        ngx_string("cmark_header"),
        NGX_HTTP_LOC_CONF | NGX_HTTP_LIF_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_cmark_loc_conf_t, header),
        NULL
    },
    {
        ngx_string("cmark_footer"),
        NGX_HTTP_LOC_CONF | NGX_HTTP_LIF_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_cmark_loc_conf_t, footer),
        NULL
    },
    ngx_null_command
};

static ngx_http_module_t ngx_http_cmark_module_ctx = {

    NULL,                                  /* preconfiguration */
    ngx_http_cmark_init,                   /* postconfiguration */
    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */
    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */
    ngx_http_cmark_create_loc_conf,        /* create location configuration */
    ngx_http_cmark_merge_loc_conf          /* merge location configuration */
};

ngx_module_t ngx_http_cmark_module = {
    NGX_MODULE_V1,
    &ngx_http_cmark_module_ctx,            /* module context */
    ngx_http_cmark_commands,               /* module directives */
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

static void *ngx_http_cmark_create_loc_conf(ngx_conf_t *cf) {

    ngx_http_cmark_loc_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_cmark_loc_conf_t));

    if (conf == NULL) {

        return NULL;
    }

    conf->enabled = NGX_CONF_UNSET;

    return conf;
}

static char *ngx_http_cmark_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {

    ngx_http_cmark_loc_conf_t *prev = parent;
    ngx_http_cmark_loc_conf_t *conf = child;

    ngx_conf_merge_value(conf->enabled, prev->enabled, 0);
    ngx_conf_merge_str_value(conf->header, prev->header, "");
    ngx_conf_merge_str_value(conf->footer, prev->footer, "");

    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_cmark_handler(ngx_http_request_t *r) {

    ngx_http_cmark_loc_conf_t *cf;

    cf = ngx_http_get_module_loc_conf(r, ngx_http_cmark_module);

    if (cf == NULL) {

        ngx_log_error(NGX_ERROR_ALERT, r->connection->log, 0, "ngx_http_get_module_loc_conf failed!");
        return NGX_DECLINED;
    }

    if (cf->enabled == NGX_CONF_UNSET || cf->enabled == 0) {

        ngx_log_debug(NGX_ERROR_ALERT, r->connection->log, 0, "cmark is not enabled.");
        return NGX_DECLINED;
    }

    ngx_int_t   rc;
    ngx_file_t  f;
    u_char *markdown;
    u_char *header;
    u_char *footer;
    size_t header_size = 0;
    size_t footer_size = 0;

    u_char *p;
    size_t root;

    ngx_str_t fn = ngx_null_string;
    p = ngx_http_map_uri_to_path(r, &fn, &root, 0);
    fn.len = ngx_strlen(fn.data);

    if (cf->header.len != 0) {

        rc = ngx_http_cmark_open_file(cf->header, &f, r->connection->log);

        if (rc != NGX_OK) {

            return rc;
        }

        ssize_t file_size;
        file_size = f.info.st_size;

        header_size = file_size;
        header = ngx_palloc(r->pool, file_size);

        if (header == NULL) {

            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        rc = ngx_http_cmark_read_file(f, header, r->connection->log);
    }

    if (cf->footer.len != 0) {

        rc = ngx_http_cmark_open_file(cf->footer, &f, r->connection->log);

        if (rc != NGX_OK) {

            return rc;
        }

        ssize_t file_size;
        file_size = f.info.st_size;

        footer_size = file_size;
        footer = ngx_palloc(r->pool, file_size);

        if (footer == NULL) {

            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        rc = ngx_http_cmark_read_file(f, footer, r->connection->log);
    }

    rc = ngx_http_cmark_open_file(fn, &f, r->connection->log);

    if (rc != NGX_OK) {

        return rc;
    }

    markdown = ngx_palloc(r->pool, f.info.st_size + 1);

    if (markdown == NULL) {

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    rc = ngx_http_cmark_read_file(f, markdown, r->connection->log);

    if ( rc < 0 || rc != f.info.st_size) {

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    cmark_gfm_core_extensions_ensure_registered();

    int options = CMARK_OPT_DEFAULT|CMARK_OPT_FOOTNOTES;
    cmark_parser *parser = NULL;
    cmark_node *document = NULL;

    char *extensions[] = {
        "autolink",
        "strikethrough",
        "table",
        "tagfilter",
        "tasklist"
    };

    parser = cmark_parser_new_with_mem(options, cmark_get_arena_mem_allocator());

    for (int i = 0; i < 5; i++) {

        cmark_syntax_extension *syntax_extension = cmark_find_syntax_extension(extensions[i]);

        if (syntax_extension) {

            cmark_parser_attach_syntax_extension(parser, syntax_extension);
        }
    }

    cmark_parser_feed(parser, (char *) markdown, ngx_strlen(markdown));
    document = cmark_parser_finish(parser);

    cmark_mem *mem = cmark_get_default_mem_allocator();

    u_char *html = (u_char *) cmark_render_html_with_mem(document, options, parser->syntax_extensions, mem);

    size_t  bufpos = 0;
    size_t  html_size  = ngx_strlen(html);
    size_t  total_size = header_size + html_size + footer_size;

    u_char *hbuf  = ngx_palloc(r->pool, total_size);

    if (header_size) {

        ngx_memcpy(hbuf + bufpos, header, header_size);
        bufpos += header_size;
    }

    if (html_size) {

        ngx_memcpy(hbuf + bufpos, html, html_size);
        bufpos += html_size;
    }

    if (footer_size) {

        ngx_memcpy(hbuf + bufpos, footer, footer_size);
        bufpos += footer_size;
    }

    ngx_free(html);
    cmark_arena_reset();

    ngx_str_t content_type = ngx_string("text/html");

    r->headers_out.content_type = content_type;
    r->headers_out.content_length_n = total_size;
    r->headers_out.status = NGX_HTTP_OK;

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {

        return rc;
    }

    ngx_buf_t *b = ngx_create_temp_buf(r->pool, total_size);

    if (b == NULL) {

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b->pos = hbuf;
    b->last = b->pos + total_size;
    b->last_buf = 1;

    ngx_chain_t out;
    out.buf  = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}

static ngx_int_t ngx_http_cmark_init(ngx_conf_t *cf) {

    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);

    if (h == NULL) {

        return NGX_ERROR;
    }

    *h = ngx_http_cmark_handler;

    return NGX_OK;
}

static ngx_int_t ngx_http_cmark_open_file(const ngx_str_t fn, ngx_file_t *f, ngx_log_t *log) {

    ngx_err_t err;
    ngx_fd_t fd;
    ngx_int_t rc;
    ngx_file_info_t fi;
    ngx_uint_t level;

    ngx_memzero(f, sizeof(ngx_file_t));
    fd = ngx_open_file(fn.data, NGX_FILE_RDONLY, NGX_FILE_OPEN, 0);

    if (fd == NGX_INVALID_FILE) {

        err = ngx_errno;

        if (err == NGX_ENOENT) {

            level = NGX_LOG_ERR;
            rc = NGX_HTTP_NOT_FOUND;

        } else {

            level = NGX_LOG_CRIT;
            rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        ngx_log_error(level, log, err, ngx_open_file_n " '%s' failed", fn.data);
        return rc;
    }

    if (ngx_fd_info(fd, &fi) == NGX_FILE_ERROR) {

        ngx_log_error(NGX_LOG_CRIT, log, ngx_errno, ngx_fd_info_n " \"%s\" failed",fn.data);
        return NGX_ERROR;
    }

    f->fd = fd;
    f->info = fi;
    f->log = log;
    f->name = fn;

    return NGX_OK;
}

static ngx_int_t ngx_http_cmark_read_file(ngx_file_t f, u_char *content, ngx_log_t *log) {

    ssize_t n;

    n = ngx_read_file(&f, content, f.info.st_size, 0);
    content[f.info.st_size] = '\0';

    if (ngx_close_file(f.fd) == NGX_FILE_ERROR) {

        ngx_log_error(NGX_LOG_CRIT, log, ngx_errno, ngx_read_file_n " \"%s\" failed", f.name.data);
    }

    return n;
}