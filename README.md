# NGINX Markdown module

This module implements [cmark-gfm](https://github.com/github/cmark-gfm) to render Markdown files as html files.

# Building module

The build will generate the extension file under `external/src/nginx/objs/ngx_http_cmark_module.so`

```bash
mkdir build
cd build
make
```

You can change the nginx or cmark-gfm version in the `CMakeLists.txt` file

```cmake
set(NGINX_VERSION 1.21.5)
set(CMARK_VERSION 0.29.0.gfm.2)
```

# Configuring NGINX

## nginx.conf

```nginx
load_module modules/ngx_http_cmark_module.so;

http {

    ...

    server {

        ...

        location ~ \.md$ {
             root /var/www/htdocs/mweb;
             cmark on;
             cmark_header /var/www/htdocs/header.html;
             cmark_footer /var/www/htdocs/footer.html;
        }
    }
}
```

## header.html

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/themes/prism.min.css">
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/github-markdown-css/5.1.0/github-markdown-light.min.css" />
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/highlight.js/11.3.1/styles/github.min.css" />
    <script src="//cdnjs.cloudflare.com/ajax/libs/highlight.js/11.3.1/highlight.min.js"></script>
    <script>
      hljs.highlightAll()
    </script>
    <style>
      .markdown-body {
        border: 1px solid;
        border-color: lightgray;
        border-radius: 8px;
        box-sizing: border-box;
        min-width: 200px;
        max-width: 1000px;
        margin: 0 auto;
        padding: 45px;
      }

      @media (max-width: 800px) {
        .markdown-body {
          padding: 15px;
        }
      }
    </style>
  </head>

  <body>
    <div class="box-body">
      <article class="markdown-body">
```

## footer.html

```html
      </article>
    </div>
    <script src="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/prism.min.js"></script>
  </body>

</html>
```
