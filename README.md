# webserv (an HTTP server in C++98) - Work in Progress

## Configuration File

Webserv reads from a `.conf` configuration file, similar to NGINX. The syntax and structure closely follow NGINX, allowing use of the same configuration file.

### Format
The configuration consists of main blocks and directives. It supports three block types:

- **`http`** 
- **`server`** (nested inside `http`)
- **`location`** (nested inside `server`)

An events block is allowed for NGINX compatibility but is ignored by webserv. 

**Blocks should be enclosed in braces, with each opening or closing brace placed on its own line.**

### Example

```conf
http 
{
    error_page              400 website/error/400.html;
    error_page              404 website/error/404.html;
    error_page              500 501 502 503  website/50x.html;
    
    server 
    {
        listen              4242;
        server_name         test test.com localhost test;
        error_page          400 website/error/400.html;
        error_page          404 website/error/404.html;

        location / 
        {
            root            website;
            index           index.html;
            limit_except    GET
            {
                deny all;
            }
        }

        location /directory/uploads 
        {
            root                    website;
            autoindex               on;
            client_max_body_size    5M;
            limit_except            POST GET
            {
                deny all;
            }
        }
    }

    server 
    {
        listen              1919;
        server_name         localhost;
        error_page          404 website/error/1919.html;

        location / 
        {
            root            website;
            index           index.html;
            limit_except    GET
            {
                deny all;
            }
        }

        location /resources/css
        {
            root            website;
            types
            {
                text/css    css;
            }
            limit_except    GET
            {
                deny all;
            }
            
        }
    }
}

```

## Block Directives

### 1. `http` Block

This is the top-level block for defining server-wide settings.

- `error_page`: Specifies the URI to redirect to in case of an error, such as 404 or 500.

### 2. `server` Block

The `server` block defines configuration related to a specific virtual host or domain. It must be nested inside the `http` block.

- `listen`: Defines the port the server listens on.
- `server_name`: Specifies the host/domain name.
    - Only exact name is handled.
    - Only alphanumeric characters handled with ‘.’
    - No wildcards
- `error_page`: Specific error pages for the server.

### 3. `location` Block

The `location` block is used to define how to respond to requests for a specific location or path on the server. It must be nested within the `server` block.

- `root`: Defines the root directory from which files will be served.
- `index`: Specifies the default `.html` file to serve.
- `autoindex`: When enabled (`on`), allows directory listing.
- `client_max_body_size`:  Sets the maximum size of the body of an HTTP request. 
- `limit_except`: Restricts access to certain HTTP methods
    - If not specified, allows all methods possible. Webserv is restricted to `GET`, `POST`, and `DELETE` methods.
    - Specify ro restrict allowed methods.
    - Example: 
        - Deny all except `GET`.
            ```conf
            limit_except GET 
            {
                deny all;
            }
            ```
        - Deny all
            ```conf
            limit_except 
            {
                deny all;
            }
            ```
- `types`: Required in NGINX, to ensure CSS files are served correctly.  In Webserv, while this directive is parsed for compatibility, it is not used. Instead, Content-Type headers are set based on the file extension of the requested resource.
    
    ```conf
    types 
    {
        text/css css;
    }
    ```
- `cgi_mode`: Determines whether CGI processing is enabled for this location block. Set it `on` to enable CGI support or `off` to disable it.
- `cgi_extension`: Specifies the file extensions that should be treated as CGI scripts. List the supported extensions (for example, `.py`, `.pl`, `.sh`) that the server will process via the CGI mechanism.

---
