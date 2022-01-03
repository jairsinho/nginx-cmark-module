cmake_minimum_required(VERSION 3.10)
project(nginx-cmark-module)

set(NGINX_VERSION 1.21.5)
set(CMARK_VERSION 0.29.0.gfm.2)

include(ExternalProject)

ExternalProject_Add(
    cmark-gfm
    GIT_REPOSITORY https://github.com/github/cmark-gfm.git
    GIT_TAG ${CMARK_VERSION}
    GIT_SHALLOW ON
    USES_TERMINAL_DOWNLOAD ON
    PREFIX ${PROJECT_SOURCE_DIR}/external
    CONFIGURE_COMMAND cmake ../cmark-gfm
    BUILD_COMMAND cmake --build . -t libcmark-gfm_static -t libcmark-gfm-extensions_static
    INSTALL_COMMAND ""
    LOG 0
)

ExternalProject_Add(
    nginx
    GIT_REPOSITORY https://github.com/nginx/nginx.git
    GIT_TAG release-${NGINX_VERSION}
    GIT_SHALLOW ON
    USES_TERMINAL_DOWNLOAD ON
    PREFIX ${PROJECT_SOURCE_DIR}/external
    CONFIGURE_COMMAND cd ../nginx && auto/configure --with-compat --add-dynamic-module=${PROJECT_SOURCE_DIR}/module
    BUILD_COMMAND cd ../nginx && make modules
    BUILD_IN_SOURCE ON
    INSTALL_COMMAND ""
    LOG 0
)