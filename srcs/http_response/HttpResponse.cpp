/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/04 11:39:22 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include "webserv.hpp"
#include "ServerConfig.hpp"
#include "HttpRequestLine.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <ctime>
#include <sstream>

HttpResponse::HttpResponse(HttpRequest &request,
                            Config &config,
                            int client_socket)
    : _request(request), _config(config), _status(INIT), _client_socket(client_socket), _allowedMethods(), _headers(), _body("")
{
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode HttpResponse::getStatusCode() const
{
    return (_status);
}


int HttpResponse::checkMethod(const std::string &method)
{
    if (method == "GET")
        return (GET);
    else if (method == "POST")
        return (POST);
    else if (method == "DELETE")
        return (DELETE);
    return (ErrorMethod);
}

void HttpResponse::execMethod()
{
    std::string method = _request.getRequestLine().getMethod();
    

    switch (checkMethod(method))
    {
        case GET:
            processRequestGET();
            break ;
        default:
            setStatusCode(METHOD_NOT_ALLOWED);
    }
}

static bool isMatchingPrefix(const std::string &pattern, const std::string &target)
{
    if (pattern.empty() || target.empty())
        return (false);
    std::string target_prefix = target.substr(0, pattern.length());
    return (pattern == target_prefix);
}

std::string HttpResponse::comparePath(const ServerConfig &server, const HttpRequestLine &request)
{
    std::string target_path = request.getUri();
    std::string path;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return(std::string());
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();

        if (config_location == target_path)
            return (config_location);
        if (target_path == "/")
        {
            if (path.empty() || config_location == "/")
                path = config_location;
            continue;
        }
        if (isMatchingPrefix(config_location, target_path))
            if (path.empty() || path.length() < config_location.length())
                path = config_location;
    }
    return (path);
}

bool HttpResponse::checkLocConfigAndRequest()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (false);
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            return (false);
        if (!isMethodAllowed(*server, path, _request.getRequestLine()))
            return (false);
    }
    return (true);
}
bool HttpResponse::isMethodAllowed(const ServerConfig &server, const std::string &path, const HttpRequestLine &request)
{
    std::string requestMethod = request.getMethod();
    
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return (false);
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        if (config_location == path)
        {
            if (location->isLimited())
            {
                if (!location->isLimitExcept(requestMethod))
                {
                    setStatusCode(METHOD_NOT_ALLOWED);
                    return (false);
                }
            }
            else
            {
                if (!location->isMethodAllowed(requestMethod))
                {
                    setStatusCode(METHOD_NOT_ALLOWED);
                    return (false);
                }
            }
            _allowedMethods = location->getAllowedMethods();
            return (true);
        }
    }
    setStatusCode(METHOD_NOT_ALLOWED);
    return (false);
    
}

std::string HttpResponse::checkRoot(const ServerConfig &server, const std::string &path)
{
    std::string rootpath;
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return (std::string());
    std::vector<LocationConfig>::const_iterator location;

    for (location = locationConfigs.begin(); location != server.getLocationConfig().end(); location++)
    {
        std::string config_location = location->getPath();
        if (config_location == path)
        {
            rootpath = location->getRoot();
            return (rootpath);
        }
    }
    return (std::string());
}

std::string HttpResponse::resolvePath()
{
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (std::string());
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
            return (std::string());
        std::string root = checkRoot(*server, path);
        if (!root.empty())
            return (root + path);
        else
            return (path);
    }
    return (std::string());
}

std::string HttpResponse::getDefaultNameLoc(const ServerConfig &server)
{
    const std::vector<LocationConfig> &LocationConfigs = server.getLocationConfig();
    if (LocationConfigs.empty())
        return (std::string());
    std::vector<LocationConfig>::const_iterator location;
    for (location = LocationConfigs.begin(); location != LocationConfigs.end(); location++)
    {
        std::string path = comparePath(server, _request.getRequestLine());
        if (path.empty())
            return(std::string());
        if (location->getPath() == path)
            return (location->getDefaultName());
    }
    return (std::string());
}


std::string HttpResponse::getDefaultName()
{
    std::string defaultName;
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (std::string());
    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
        defaultName = getDefaultNameLoc(*server);
    return (defaultName);
}

static bool checkSlash(const std::string &defaultLoc, const std::string &page)
{
    bool defaultSlash = !defaultLoc.empty() && defaultLoc[defaultLoc.length() - 1] == '/';
    bool pageSlash = !page.empty() && page[0] == '/';
    
    if (!defaultSlash && !pageSlash)
        return (false);
    return (true);
}

static std::string extractResourceName(const std::string &uri)
{
    size_t  lastSlashPos = uri.find_last_of('/');
    if (lastSlashPos != std::string::npos && lastSlashPos + 1 < uri.length())
        return (uri.substr(lastSlashPos + 1));
    return (std::string());
}

void HttpResponse::addContentTypeHeader(const std::string &type)
{
    if (type.empty())
        _headers["Content-Type"] = "text/html";
    else
        _headers["Content-Type"] = getMimeType(type);
}

void  HttpResponse::getResourceContent(const std::string &file_path)
{
    std::ifstream   infile;
    std::string     buffer;
    
    infile.open(file_path.c_str(), std::ios::binary);
    if (!infile.is_open())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    std::string contentType = getExtension(file_path);
    if (!isSupportedMedia(contentType))
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
    
    // Read the binary file content manually
    infile.seekg(0, std::ios::end);  // Move to the end of the file to get the size
    std::streampos fileSize = infile.tellg();  // Get the size of the file
    infile.seekg(0, std::ios::beg);  // Move back to the beginning
    _body.resize(fileSize);
    infile.read(&_body[0], fileSize); 
    infile.close();
    addContentTypeHeader(contentType);
    setStatusCode(OK);
}

bool HttpResponse::isSupportedMedia(const std::string &uri)
{
    std::string extension = getExtension(uri);
    std::string type = getMimeType(extension);
    return (!type.empty());    
}

void HttpResponse::getResource(const std::string &target_path)
{
    std::string indexPath;
        

    std::string uri = _request.getRequestLine().getUri();
    if (isSupportedMedia(uri))
    {
        //!Check for other supported media types
        std::string resourceName = extractResourceName(uri);
        if (!checkSlash(target_path, resourceName))
            indexPath = target_path + '/' + resourceName;
        else
            indexPath = target_path + resourceName;
        if (fileExists(indexPath))
            getResourceContent(indexPath);
        else
            setStatusCode(NOT_FOUND);
    }
    else
    {
        std::string defaultPage = getDefaultName();
        if (defaultPage.empty())
            setStatusCode(NOT_FOUND);
        if (!checkSlash(target_path, defaultPage))
            indexPath = target_path + '/' + defaultPage;
        else
            indexPath = target_path + defaultPage;
        getResourceContent(indexPath);
    }
}


void HttpResponse::processRequestGET()
{
    if (!checkLocConfigAndRequest())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }

    /** 
     * TODO: check if redirect
     * * check if URI is absolute or relative; a redirection can be either
     */
    
    /** 
     * TODO: check if is directory
     * * check location config if auto index
     * * list
     */
    
    std::string path = resolvePath();

    if (path.empty())
        setStatusCode(NOT_FOUND);
    
    if (isDirectory(path))
    {
        getResource(path);
        
    }
}


void HttpResponse::generateHttpResponse()
{
    _headers["Server"] = "webserv";
    _headers["Date"] = getHttpDateCET();
    _headers["Content-Length"] = toString(_body.size());
    addKeepAliveHeader();
    addAllowHeader();

    std::string statusLine = generateStatusLine();
    std::string headerLines = generateHeaderLines();
    std::string empty = CRLF;

    if (!_responseMsg.empty())
        _responseMsg.clear();
    _responseMsg.insert(_responseMsg.end(), statusLine.begin(), statusLine.end());
    _responseMsg.insert(_responseMsg.end(), headerLines.begin(), headerLines.end());
    _responseMsg.insert(_responseMsg.end(), empty.begin(), empty.end());
    _responseMsg.insert(_responseMsg.end(), _body.begin(), _body.end());
}


std::string HttpResponse::getHttpDateCET()
{
    std::time_t now = std::time(NULL);
    std::tm *gmt_tm = std::gmtime(&now);
    
    const int kDateBufSize = 1024;
    char date[kDateBufSize];
    std::strftime(date, sizeof(date), "%a, %d %b %Y %H:%M:%S GMT", gmt_tm);
    
    return (std::string(date));
}

bool HttpResponse::isKeepAlive() const
{
    if (_request.isConnectionClosed())
        return (false);
    if (_config.getKeepAliveTimeout() == 0)
        return (false);
    return (true);
}


void HttpResponse::addKeepAliveHeader()
{
    if (isKeepAlive())
    {
        _headers["Connection"] = "keep-alive";
        std::ostringstream keepalive;
        keepalive << "time=" << _config.getKeepAliveTimeout();
        keepalive << ", max=100"; //! MAX connections
        _headers["Keep-Alive"] = keepalive.str();
    }
    else
        _headers["Connection"] = "close";
}

void HttpResponse::addAllowHeader()
{
    if (getStatusCode() != METHOD_NOT_ALLOWED) 
        return ;
    
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }
    std::vector<ServerConfig>::const_iterator server;
    
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        std::string path = comparePath(*server, _request.getRequestLine());
        if (path.empty())
        {
            setStatusCode(NOT_FOUND);
            return ;
        }
        std::string target_path = _request.getRequestLine().getUri();
        
        const std::vector<LocationConfig> &locationConfigs = server->getLocationConfig();
        if (locationConfigs.empty())
        {
            setStatusCode(NOT_FOUND);
            return ;
        }
        std::vector<LocationConfig>::const_iterator location;
        for (location = locationConfigs.begin(); location != locationConfigs.end(); location++)
        {
            std::string config_location = location->getPath();
            if (config_location == target_path)
            {
                std::string allowed_method;
                if (location->isLimited())
                {
                    std::vector<std::string> limitExceptMethods = location->getLimitExcept();
                    for (size_t i = 0; i < limitExceptMethods.size(); i++)
                    {
                        if (i != 0)
                            allowed_method += ", ";
                        allowed_method += limitExceptMethods[i];
                    }
                    _headers["Allow"] = allowed_method;
                }
                else
                {
                    std::vector<std::string> allowedMethods = location->getAllowedMethods();
                    for (size_t i = 0; i < allowedMethods.size(); i++)
                    {
                        if (i != 0)
                            allowed_method += ", ";
                        allowed_method += allowed_method[i];
                    }
                    _headers["Allow"] = allowed_method;
                }
            }
        }
    }
}

std::string HttpResponse::generateStatusLine()
{
    std::ostringstream status_line;
    
    status_line << "HTTP/1.1" << " ";
    status_line << static_cast<int>(getStatusCode()) << " ";
    status_line << getStatusReason(getStatusCode());
    status_line << CRLF;
    return (status_line.str());   
}

std::string HttpResponse::generateHeaderLines()
{
    std::map<std::string, std::string>::const_iterator header;
    std::ostringstream response_headers;

    for (header = _headers.begin(); header != _headers.end(); header++)
    {
        std::string field_name = header->first;
        std::string field_value = header->second;

        response_headers << field_name << ": " << field_value << CRLF;
    }
    return (response_headers.str());
}

void HttpResponse::sendResponse()
{
    if (_responseMsg.empty())
    {
        std::cerr << "ERROR: response empty" << std::endl;
        return;
    }

    ssize_t bytesSent = send(_client_socket, _responseMsg.data(), _responseMsg.size(), 0);
    if (bytesSent < 0)
    {
        std::cerr << "ERROR: sending bytes" << std::endl;
        return;
    }
    // std::cout << bytesSent << " bytes sent" << std::endl;
    _responseMsg.erase(_responseMsg.begin(), _responseMsg.begin() + bytesSent);
    if (_headers["Connection"] != "keep-alive")
    {
        if (close(_client_socket) < 0)
            std::cerr << "ERROR: closing socket" << std::endl;
    }
}

std::string HttpResponse::getHttpResponse()
{
    std::string response(_responseMsg.begin(), _responseMsg.end());
    return (response);
}