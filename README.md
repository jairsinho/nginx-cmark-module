# NGINX Markdown module

This module implements [cmark-gfm](https://github.com/github/cmark-gfm) to render Markdown files as html files.

# Building module

The build will generate the extension file under `external/src/nginx/objs/ngx_http_cmark_module.so`

```bash
mkdir build
cd build
cmake ..
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
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/plugins/toolbar/prism-toolbar.min.css">
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/github-markdown-css/5.1.0/github-markdown-light.min.css" />
    <link rel="stylesheet" href="//cdnjs.cloudflare.com/ajax/libs/bootstrap/5.1.3/css/bootstrap.min.css" />
    <style>
      body{
        margin-top: 32px;
        margin-bottom: 32px;
      }

      .card {
        min-width: 200px;
        max-width: 1000px;
        margin: 0 auto;
      }

      .markdown-body {
        padding: 32px;
      }

      @media (max-width: 800px) {
        .markdown-body {
          padding: 15px;
        }
      }
    </style>
  </head>

  <body>
    <div class="card">
      <div class="card-header d-flex gap-2" id="xfilename" style="font-weight: bold;">
        <span class="align-middle">
          <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" fill="currentColor" class="bi bi-list-ul" viewBox="0 0 16 16"><path fill-rule="evenodd" d="M5 11.5a.5.5 0 0 1 .5-.5h9a.5.5 0 0 1 0 1h-9a.5.5 0 0 1-.5-.5zm0-4a.5.5 0 0 1 .5-.5h9a.5.5 0 0 1 0 1h-9a.5.5 0 0 1-.5-.5zm0-4a.5.5 0 0 1 .5-.5h9a.5.5 0 0 1 0 1h-9a.5.5 0 0 1-.5-.5zm-3 1a1 1 0 1 0 0-2 1 1 0 0 0 0 2zm0 4a1 1 0 1 0 0-2 1 1 0 0 0 0 2zm0 4a1 1 0 1 0 0-2 1 1 0 0 0 0 2z"/></svg></span> <span class="align-middle" id="filename">
        </span>
      </div>
      <div class="markdown-body">
```

## footer.html

```html
        </div>
      </div>
    </div>
    <script src="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/components/prism-core.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/plugins/autoloader/prism-autoloader.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/plugins/toolbar/prism-toolbar.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/prism/1.25.0/plugins/copy-to-clipboard/prism-copy-to-clipboard.min.js"></script>
    <script src="//cdnjs.cloudflare.com/ajax/libs/bootstrap/5.1.3/js/bootstrap.bundle.min.js"></script>
    <script>
      var url = window.location.pathname;
      var filename = unescape(url.substring(url.lastIndexOf('/') + 1));
      document.getElementById("filename").innerHTML = document.title = filename;
    </script>
  </body>

</html>
```
