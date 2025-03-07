/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/04 01:19:13 by jdagoy            #+#    #+#             */
/*   Updated: 2025/01/14 22:28:04 by jdagoy          ###   ########.fr       */
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

HttpResponse::HttpResponse(HttpRequest &request, const Config &config, int client_socket)
    : _request(request), _config(config), _status(OK), _client_socket(client_socket), _allowedMethods(), _headers(), _body("")
{
    if (_request.getStatusCode() !=  OK && _request.getStatusCode() != INIT)
    {
        _status = _request.getStatusCode();
        generateHttpResponse();
        return ;
    }
    execMethod();
    generateHttpResponse();
}

HttpResponse::~HttpResponse()
{
    _body.clear();
	_responseMsg.clear();
    _headers.clear();
    _allowedMethods.clear();
}

void	HttpResponse::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode HttpResponse::getStatusCode() const
{
    return (_status);
}


int	HttpResponse::checkMethod(const std::string &method)
{
    if (method == "GET")
        return (GET);
    else if (method == "POST")
        return (POST);
    else if (method == "DELETE")
        return (DELETE);
    return (ErrorMethod);
}

void	HttpResponse::execMethod()
{
    std::string method = _request.getRequestLine().getMethod();
    _serverConfig = checkLocConfigAndRequest();
    if (!_serverConfig.isValid())
    {
        if (_status == INIT)
            setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    _locationConfig = getLocationConfig();
    if (_locationConfig.getPath().empty())
    {
        setStatusCode(NOT_FOUND);
        return ;
    }

    if (!isMethodAllowed(_locationConfig, method))
        return ;
    if (isCGIRequest(_request.getRequestLine().getUri()))
    {
        std::cout << "CGI REQUEST" << std::endl;
        std::string cgiPath = resolveCGIPath();
        // Cgi cgi(_request.getRequestLine(), _request, cgiPath, UPLOAD_DIR);
        try
        {
            if (_request.getRequestLine().getMethod() == "POST")
            {
                // std::cout << _request.getHeader("content-type") << std::endl;
                // std::cout << _request.getHeader("content-length") << std::endl;
                // _request.printBuffer();
                Cgi cgi(_request.getRequestLine(), _request, cgiPath, UPLOAD_DIR);
                _request.parseRequestBody();
                cgi.printMultiFormData();

            } 
            
            // _body = cgi.runCgi();
            // _status = cgi.getStatusCode();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return ;
    }
    
    switch (checkMethod(method))
    {
        case GET:
            processRequestGET();
            break ;
        case POST:
            _request.setMaxBodySize(_locationConfig.getClientMaxBodySize(), _locationConfig.getMaxBodyMode());
            _request.parseRequestBody();
			if (_request.getStatusCode() != OK)
			{
				setStatusCode(_request.getStatusCode());
				break ;
			}
			if (!checkPostLocation())
				break ; 
			if (_request.getFormData("_method") == "DELETE")
			{
				processRequestDELETE();
				break ; 
			}
			processRequestPOST();
				break ;
        case DELETE:
            processRequestDELETE();
            break ;
        default:
            setStatusCode(METHOD_NOT_ALLOWED);
    }
}

static bool	isMatchingPrefix(const std::string &pattern, const std::string &target)
{
    if (pattern.empty() || target.empty())
        return (false);
    std::string target_prefix = target.substr(0, pattern.length());
	return (pattern == target_prefix);
}

static int countSlashes(const std::string &path)
{
	int count = 0;
	for (size_t i = 0; i < path.length(); i++)
	{
		if (path[i] == '/')
			count++;
	}
	return (count);
}

std::string	HttpResponse::comparePath(const ServerConfig &server, const HttpRequestLine &request)
{
    std::string target_path = request.getUri();
    std::string path;
    
    const std::vector<LocationConfig> &locationConfigs = server.getLocationConfig();
    if (locationConfigs.empty())
        return(std::string());
    std::vector<LocationConfig>::const_iterator location;

    std::string uriExtension = getExtension(target_path);
	int uriSlashCount = countSlashes(target_path);
    bool slashAutoIndex;
    bool slash= false;

    for (location = locationConfigs.begin(); location != locationConfigs.end(); location++)
    {
        std::string config_location = location->getPath();
		if (config_location == "/")
		{
			if (config_location == target_path || target_path == "/index.html")
				return (config_location);
			slash = true;
			slashAutoIndex = location->getAutoIndex() == "on";
			continue ;
		}
        if (!config_location.empty() && config_location[0] == '.')
        {
            std::string cleanLocPath = config_location.substr(1);
            if (uriExtension == cleanLocPath && uriSlashCount == 1)
                return (config_location);
        }
        if (config_location == target_path || (target_path + "/") == config_location)
            return (config_location);
        
        if (isMatchingPrefix(config_location, target_path))
		{
            if (path.empty() || path.length() < config_location.length())
				path = config_location;
		}
    }
	if (slash && slashAutoIndex)
        return ("/");
    if (path.empty())
        setStatusCode(NOT_FOUND);
    return (path);
}


LocationConfig	HttpResponse::getLocationConfig()
{
    LocationConfig location;
    
    std::string path = comparePath(_serverConfig, _request.getRequestLine());
    if (path.empty())
        return (location);
    
    const std::vector<LocationConfig> &locationConfigs = _serverConfig.getLocationConfig();
    if (locationConfigs.empty())
        return (location);
    std::vector<LocationConfig>::const_iterator loc;
    for (loc = locationConfigs.begin(); loc != locationConfigs.end(); loc++)
    {
        if (loc->getPath() == path)
        {
            location = *loc;
            return (location);
        }
    }
    return (location);    
}

ServerConfig	HttpResponse::checkLocConfigAndRequest()
{
    ServerConfig config;
    
    const std::vector<ServerConfig> &serverConfigs = _config.getServerConfig();
    if (serverConfigs.empty())
        return (config);
    std::string host = _request.getHost();
        if (host.empty())
            return (config);
    std::string requestHost;
    int port;
    size_t check = host.find(":");
    if (check != std::string::npos)
    {
        requestHost = host.substr(0, check);
        port = strToInt(host.substr(check + 1));
    }

    std::vector<ServerConfig>::const_iterator server;
    for (server = serverConfigs.begin(); server != serverConfigs.end(); server++)
    {
        _serverName = server->checkServerName(requestHost);
        if (port == server->getPort() && _serverName == requestHost)
        {
            std::string path = comparePath(*server, _request.getRequestLine());
            config = *server;
            if (!path.empty())
                config.setValid();
            return (config);;
        }    
    }

    return (config);
}

bool	HttpResponse::isMethodAllowed(const LocationConfig &location, const std::string &requestMethod)
{
	if (checkMethod(requestMethod) == ErrorMethod)
	{
		setStatusCode(NOT_IMPLEMENTED);
		return (false);
	}
	if (location.isDenyMethod(requestMethod) && !location.isMethodAllowed(requestMethod))
	{
		setStatusCode(METHOD_NOT_ALLOWED);
		return (false);
	}
    
    _allowedMethods = location.getAllowedMethods();
    return (true);
}

std::string	HttpResponse::checkRoot(const std::string &path)
{
    std::string rootpath;

    std::string config_location = _locationConfig.getPath();
    if (path.find(config_location) != std::string::npos)
    {
        rootpath = _locationConfig.getRoot();
        return (rootpath);
    }
    return (std::string());
}

std::string	HttpResponse::resolvePath(const ServerConfig &server)
{
    std::string uri = _request.getRequestLine().getUri();
    
    std::string path = comparePath(server, _request.getRequestLine());
    if (uri.find(path) != std::string::npos && uri.length() > path.length())
        path = lastSlash(uri);
    if (path.empty())
        return (std::string());
    std::string root = checkRoot(path);
    if (!root.empty())
        return (root + path);
    else if (root.empty() && _locationConfig.getPath() == "/")
        return ("." + path);
	else
		return (path);
}

std::string	HttpResponse::getDirectiveLoc(const std::string &directive)
{

    if (directive == "index")
        return (_locationConfig.getIndex());
    else if (directive == "autoindex")
        return (_locationConfig.getAutoIndex());
    return (std::string());
}


bool	HttpResponse::isSupportedMedia(const std::string &uri)
{
    std::string extension = getExtension(uri);
    std::string type = getMimeType(extension);
    return (!type.empty());    
}

void	HttpResponse::sendResponse()
{
	size_t totalSent = 0;
	size_t responseSize = _responseMsg.size();

    if (_responseMsg.empty())
	{
        throw(std::runtime_error("Error: empty response"));
	}
	while (totalSent < responseSize)
	{
		ssize_t bytesSent = send(_client_socket, _responseMsg.data(), _responseMsg.size(), 0);
		if (bytesSent < 0)
			throw(std::runtime_error("Error: sending response"));
		if (bytesSent == 0)
			throw(std::runtime_error("Error: connection closed while sending"));
		totalSent += bytesSent;
	}
    _responseMsg.clear();
    if (_headers["Connection"] != "keep-alive")
    {
        if (close(_client_socket) < 0)
			throw(std::runtime_error("Error: closing socket"));
    }
}

std::string	HttpResponse::getHttpResponse() const
{
    std::string response(_responseMsg.begin(), _responseMsg.end());
    return (response);
}

std::string	HttpResponse::cleanURI(std::string uri)
{
    if (uri == "/")
        return (uri);
    while (uri.find("//") != std::string::npos)
        uri.erase(uri.find("//"), 1);
    if (!uri.empty() && uri[uri.size() - 1] == '/')
        uri = uri.substr(0, uri.size() - 1);
    if (uri[0] == '/')
        uri = uri.substr(1);
    return (uri);
}

bool	HttpResponse::isCGIRequest(const std::string &uri)
{
    std::string extension;
    
    if (uri.find("?") != std::string::npos)
        extension = getExtension(uri.substr(0, uri.find("?")));
    else
    {
        extension = getExtension(uri);
    }

    if (_locationConfig.isCGIExtensionAllowed(extension))
        return (true);
    return (false);
}

std::string HttpResponse::resolveCGIPath()
{
    std::string uri = _request.getRequestLine().getUri();
    std::string scriptName = uri.substr(uri.find_last_of("/") + 1);
    if (scriptName.find("?") != std::string::npos)
        scriptName = scriptName.substr(0, scriptName.find("?"));
    if (_locationConfig.getPath()[0] == '.' && _locationConfig.getRoot().empty())
        return ("./website/directory/cgi-bin/" + scriptName);
    else
    {
        if (_locationConfig.getRoot().empty())
            return ("./website/directory/cgi-bin/" + scriptName);
        else
        {
            std::string path = "./" + _locationConfig.getRoot() + _locationConfig.getPath();
            if (path[path.size() - 1] == '/')
                return (path + scriptName);
            else
                return (path + "/" + scriptName);
        }
    }
}
