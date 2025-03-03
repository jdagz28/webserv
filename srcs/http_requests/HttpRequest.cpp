/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: romvan-d <romvan-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/25 18:44:14 by romvan-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "webserv.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <ctime>
#include <sys/stat.h>
#include <fstream>
#include <iomanip> 
#include <cstring>

HttpRequest::HttpRequest(int client_socket)
    : _request(),  _headersN(0), _status(OK), _errorMsg(""), _client_socket(client_socket), _parseStep(REQUEST_INIT), _maxBodySize(0)
{}

HttpRequest::HttpRequest(const HttpRequest &copy)
    : _request(copy._request),
        _headers(copy._headers),
        _headersN(copy._headersN),
        _buffer(copy._buffer),
        _status(copy._status),
        _errorMsg(copy._errorMsg),
        _client_socket(copy._client_socket),
        _formData(copy._formData),
        _parseStep(copy._parseStep),
        _multiFormData(copy._multiFormData),
        _maxBodySize(copy._maxBodySize)
{}

HttpRequest &HttpRequest::operator=(const HttpRequest &copy)
{
    if (this != &copy)
    {
        _request = copy._request;
        _headers = copy._headers;
        _headersN = copy._headersN;
        _buffer = copy._buffer;
        _status = copy._status;
        _errorMsg = copy._errorMsg;
        _client_socket = copy._client_socket;
        _formData = copy._formData;
        _parseStep = copy._parseStep;
        _multiFormData = copy._multiFormData;
        _maxBodySize = copy._maxBodySize;
    }
    return (*this);
}

HttpRequest::~HttpRequest()
{
	if (_client_socket >= 0)
	{
        close(_client_socket);
        _client_socket = -1; 
    }
}

void	HttpRequest::parseHttpRequest()
{
    if (_parseStep == REQUEST_INIT)
        _parseStep = REQUEST_LINE;
    while (true)
    {
        std::string line = getLineAndPopFromBuffer();
        if (line.empty())
            return ;
        switch (_parseStep)
        {
            case REQUEST_LINE:
                parseRequestLine(line);
                if (_status != OK)
                    return ;
                _parseStep = REQUEST_HEADER;
                break;
            case REQUEST_HEADER:
                if (line.empty())
                {
                    _parseStep = REQUEST_BODY;
                    return ;
                }
                else
                {
                    parseRequestHeaders(line);
                    _headersN++;
                    if (_status != OK)
                        return ;
                }
                break ;
            default:
                break ;
        }
    }
}

size_t	HttpRequest::getContentLengthBuffer(const std::string &header)
{
    std::string lowerHeader = toLower(header);
	std::string label = "content-length:";
    size_t  pos = lowerHeader.find(label);
    if (pos == std::string::npos)
	{
		return (0);
	}
	if (pos + label.size() >= lowerHeader.size())
		return (0);
    pos += label.size();
	while (pos < lowerHeader.size() && isspace(lowerHeader[pos]))
		pos++;
	if (pos >= lowerHeader.size())
		return (0);
    std::string::size_type endPos = lowerHeader.find(CRLF, pos);
	if (endPos == std::string::npos)
		return (0);
    return (strToInt(lowerHeader.substr(pos, endPos - pos)));
}

void	HttpRequest::requestToBuffer()
{
    const size_t tempBufferSize = 1024;
    std::vector<unsigned char> tempBuffer(tempBufferSize);
    ssize_t bytesRead = 0;
	bool dataRead = false;
    
	while (true)
	{
		bytesRead = recv(_client_socket, &tempBuffer[0], tempBufferSize, 0);
		if (bytesRead > 0)
		{
			_buffer.insert(_buffer.end(), tempBuffer.begin(), tempBuffer.begin() + bytesRead);
			dataRead = true;
		}
		else if (bytesRead == 0)
			break;
		else
		{
			if (!dataRead)
				throw::std::runtime_error("Error: reading from socket");
			break ; 
		}
	}
}

std::vector<unsigned char>::iterator	HttpRequest::findBufferCRLF()
{
    std::vector<unsigned char>::iterator it;
    for (it = _buffer.begin(); it != _buffer.end() - 1; ++it)
    {
        if (*it == '\r' && *(it + 1) == '\n')
            return (it);
    }
    return (_buffer.end());
}

std::string	HttpRequest::getLineAndPopFromBuffer()
{
    std::string line;
    
    if (_buffer.empty())
        return (std::string());
    std::vector<unsigned char>::iterator crlf_pos = findBufferCRLF();
    if (crlf_pos != _buffer.end())
    {
        line = std::string(_buffer.begin(), crlf_pos);
        _buffer.erase(_buffer.begin(), crlf_pos + 2);
    }
    else
    {
        line = std::string(_buffer.begin(), _buffer.end());
        _buffer.clear();
    }
    if (!line.empty() && line[line.size() - 1] == '\r')
        line = line.substr(0, line.size() - 1);
    return (line);
}

void	HttpRequest::printBuffer() const
{
    std::vector<unsigned char>::const_iterator it;
    for (it = _buffer.begin(); it != _buffer.end(); it++)
        std::cout << *it;
    std::cout << std::endl;
}

void	HttpRequest::setClientSocket(int client_socket)
{
    _client_socket = client_socket;
}

void	HttpRequest::setMaxBodySize(size_t bodySize)
{
    _maxBodySize = bodySize * 1024 * 1024;
}

const	HttpRequestLine& HttpRequest::getRequestLine() const
{
    return (_request);
}

const	std::map<std::string, std::vector<std::string> >& HttpRequest::getHeaders() const
{
    
    return (_headers);
}

bool	HttpRequest::isConnectionClosed() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header = _headers.find("connection");
    
    if (header == _headers.end())
        return (false);
    std::vector<std::string>::const_iterator value;
    for (value = header->second.begin(); value != header->second.end(); value++)
    {
        if (toLower(*value) == "close")
            return (true);
    }
    return (false);
}

std::string	HttpRequest::getHost() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header =_headers.find("host");
    if (header != _headers.end())
        return (header->second[0]);
    return (std::string());
}

void	HttpRequest::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode	HttpRequest::getStatusCode() const
{
    return (_status);
}

const std::string	&HttpRequest::getErrorMsg() const
{
    return (_errorMsg);
}

const std::string	HttpRequest::getHeader(const std::string &field) const
{
    std::map<std::string, std::vector<std::string> > ::const_iterator header;
    
    header = _headers.find(field);
    if (header != _headers.end())
        return (header->second[0]);
    return (std::string());
}

const std::map<std::string, std::string>	&HttpRequest::getFormData() const
{
    return (_formData);
}

bool	HttpRequest::isSupportedMediaPOST()
{
    std::string type = getHeader("content-type");
    if (type.empty())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return (false);
    }
    if (type == "application/x-www-form-urlencoded" || type.substr(0, 19) == "multipart/form-data")
        return (true);
    if (type == "image/jpeg" || type == "image/jpg" || type == "image/gif" || type == "image/png" || type == "image/bmp")
        return (true);
    setStatusCode(UNSUPPORTED_MEDIA_TYPE);
    return (false);
}

bool	HttpRequest::isSupportedMediaPOST(const std::string &type)
{
    if (type.empty())
        return (false);
    if (type == "image/jpeg" || type == "image/jpg" || type == "image/gif" || type == "image/png" || type == "image/bmp")
        return (true);
    return (false);
}

bool HttpRequest::isMultiPartFormData(std::string *boundary)
{
    std::string type = getHeader("content-type");
    size_t pos = type.find("multipart/form-data");
    if (pos == std::string::npos)
        return (false);
    pos = type.find("boundary=");
    if (pos == std::string::npos)
        return (false);
    *boundary = type.substr(pos + 9);
    return (true);
}

bool HttpRequest::isMultiPartFormData()
{
    std::string type = getHeader("content-type");
    size_t pos = type.find("multipart/form-data");
    if (pos == std::string::npos)
        return (false);
    return (true);
}

bool HttpRequest::isForUpload()
{
	if (_multiFormData.empty())
		return (false);
    std::map<std::string, MultiFormData>::iterator form;
    for (form = _multiFormData.begin(); form != _multiFormData.end(); form++)
    {
        std::cout << form->second.fields["content-type"];
		if (form->second.fields["content-type"] == "image/jpeg" || form->second.fields["content-type"] == "image/jpg" || form->second.fields["content-type"] == "image/png" || form->second.fields["content-type"] == "image/gif" || form->second.fields["content-type"] == "image/bmp")
            return (true);
    }
    return (false);
}

const std::map<std::string, MultiFormData> &HttpRequest::getMultiFormData() const
{
    return (_multiFormData);
}

std::string HttpRequest::getFormData(const std::string &method)
{
    std::map<std::string, std::string>::const_iterator it;
    it = _formData.find(method);
    if (it != _formData.end())
    {
        _formData.erase(method);
        return (std::string("DELETE"));
    }
    return (std::string());
}

bool	HttpRequest::isHeadersComplete()
{
	std::string buffStr(_buffer.begin(), _buffer.end());
	return (buffStr.find(CRLF CRLF) != std::string::npos);
}

size_t	HttpRequest::expectedTotalBytes()
{
	std::string buffStr(_buffer.begin(), _buffer.end());
	size_t headerEndPos = buffStr.find(CRLF CRLF);
	if (headerEndPos == std::string::npos)
		return (0);
	headerEndPos += 4;
	size_t contentLength = getContentLengthBuffer(std::string(_buffer.begin(), _buffer.begin() + headerEndPos));
	return (headerEndPos + contentLength);
}

size_t HttpRequest::getBufferSize() const
{
	return (_buffer.size());
}

void	HttpRequest::reset()
{
	_request = HttpRequestLine();
	_headers.clear();
	_headersN = 0;
	_buffer.clear();
	_status = OK;
	_errorMsg = "";
	_formData.clear();
	_parseStep = REQUEST_INIT;
	_multiFormData.clear();
	_maxBodySize = 0;
}
