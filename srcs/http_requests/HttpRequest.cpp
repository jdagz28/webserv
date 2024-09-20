/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:18:22 by jdagoy            #+#    #+#             */
/*   Updated: 2024/09/20 15:39:58 by jdagoy           ###   ########.fr       */
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

HttpRequest::HttpRequest(int client_socket)
    : _request(),  _headersN(0), _status(OK), _errorMsg(""), _client_socket(client_socket), _parseStep(REQUEST_INIT)
{
    requestToBuffer();
    // printBuffer();
    if (_errorMsg.empty())
        parseHttpRequest();
}

HttpRequest::~HttpRequest()
{
}

void HttpRequest::parseHttpRequest()
{
    if (_parseStep == REQUEST_INIT)
        _parseStep = REQUEST_LINE;
    while (true)
    {
        std::string line = getLineAndPopFromBuffer();

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
                    return;
                }
                else
                {
                    parseRequestHeaders(line);
                    _headersN++;
                    if (_status != OK)
                        return ;
                }
                break;
            default:
                break;
        }
    }
}

std::string    HttpRequest::extract_token(const std::string &line, size_t &pos, char del)
{
    size_t  end = line.find(del, pos);
    if (end == std::string::npos)
        end = line.length();
    std::string token = line.substr(pos, end - pos);
    pos = end + 1;
    return (token);
}



void    HttpRequest::parseRequestLine(const std::string &line)
{
    size_t  pos = 0;
    
    if (_request.getMethod().empty())
    {
        std::string method = extract_token(line, pos, ' '); 
        if (method.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        trimWhitespaces(method);
        _request.setMethod(method);
    }

    if (_request.getUri().empty())
    {
        std::string uri = extract_token(line, pos, ' ');
        if (uri.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        if (uri.length() > MAX_URI_LENGTH)
        {
            setStatusCode(URI_TOO_LONG);
            return ;
        }
        trimWhitespaces(uri);
        _request.setUri(uri);
    }

    if (_request.getVersion().empty())
    {
        std::string version = extract_token(line, pos, ' ');
        if (version.empty())
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        trimWhitespaces(version);
        _request.setVersion(version);
    }
}

bool    HttpRequest::isValidFieldName(const std::string &line)
{
    if (line.empty())
        return (false);
    for (size_t i = 0; i < line.size(); i++)
        if (!std::isalnum(line[i]) && line[i] != '-')
            return (false);
    return (true);
}

bool    HttpRequest::isValidFieldValue(const std::string &line)
{
    if (line.empty())
        return (false);
    for (size_t i = 0; i < line.size(); i++)
    {
        if (!std::isprint(line[i]) && !std::isspace(line[i]) )
            return (false);
    }
    return (true);
}

void HttpRequest::parseRequestHeaders(const std::string &line)
{
    size_t  colonPos = line.find (':');
    if (colonPos == std::string::npos)
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }

    std::string fieldName = line.substr(0, colonPos);
    std::string fieldValue = line.substr(colonPos + 1);
    
    fieldName = toLower(fieldName);
    // for (size_t i = 0; i < fieldName.size(); i++)
    //     fieldName[i] = std::tolower(fieldName[i]);
    if (!isValidFieldName(fieldName) || !isValidFieldValue(fieldValue))
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }    
    trimWhitespaces(fieldValue);
    if (fieldValue.length() > MAX_HEADER_LENGTH)
    {
        setStatusCode(REQUEST_HEADER_TOO_LARGE);
        return ;
    }
    
    std::stringstream ss(fieldValue);
    std::string value;
    std::vector<std::string> values;
    while (std::getline(ss, value, ','))
    {
        trimWhitespaces(value);
        values.push_back(value);
    }
    _headers[fieldName] = values;
}

size_t  getContentLengthBuffer(const std::string &header)
{
    size_t  pos = header.find("content-length:");
    if (pos != std::string::npos) {
        pos += 16;
        std::string::size_type endPos = header.find("\r\n", pos);
        return (strToInt(header.substr(pos, endPos - pos)));
    }
    return (0);
}


void    HttpRequest::requestToBuffer()
{
    char    buffer[1024];
    int     bytes_read;
    size_t  totalBytesRead = 0;
    size_t  contentLen = 0;

    _buffer.clear();
    while (true) 
    {
        bytes_read = recv(_client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0)
        {
            _errorMsg = "Error: receiving request to buffer";
            return ;
        }
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++)
            _buffer.push_back(buffer[i]);
        
        std::string currentData(_buffer.begin(), _buffer.end());
        size_t headerEnd = currentData.find("\r\n\r\n");
        if (headerEnd != std::string::npos)
        {
            contentLen = getContentLengthBuffer(currentData.substr(0, headerEnd));
            totalBytesRead = _buffer.size();
            break ;
        }

        if (contentLen > 0)
        {
            size_t remainingBytes = contentLen - totalBytesRead;
            while(remainingBytes > 0)
            {
                bytes_read = recv(_client_socket, buffer, std::min(sizeof(buffer) - 1, remainingBytes), 0);
                if (bytes_read <= 0)
                {
                    _errorMsg = "Error: receiving request to buffer";
                    return ;
                }
                buffer[bytes_read] = '\0';
                for (int i = 0; i < bytes_read; i++)
                    _buffer.push_back(buffer[i]);
                remainingBytes -= bytes_read;
            }
        }
    }
}

std::vector<unsigned char>::iterator HttpRequest::findBufferCRLF()
{
    std::vector<unsigned char>::iterator it;
    for (it = _buffer.begin(); it != _buffer.end(); it++)
    {
        if (*it == '\r' && *(it + 1) == '\n')
            return (it);
    }
    return (_buffer.end());
}

std::string    HttpRequest::getLineAndPopFromBuffer()
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

void    HttpRequest::printBuffer() const
{
    std::vector<unsigned char>::const_iterator it;
    for (it = _buffer.begin(); it != _buffer.end(); it++)
        std::cout << *it;
    std::cout << std::endl;
}

void    HttpRequest::setClientSocket(int client_socket)
{
    _client_socket = client_socket;
}

const HttpRequestLine& HttpRequest::getRequestLine() const
{
    return (_request);
}

const std::map<std::string, std::vector<std::string> >& HttpRequest::getHeaders() const
{
    
    return (_headers);
}

bool HttpRequest::isConnectionClosed() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header = _headers.find("connection");
    
    if (header == _headers.end())
        return (false);
    std::vector<std::string>::const_iterator value;
    for (value = header->second.begin(); value != header->second.end(); value++)
    {
        if (*value == "close")
            return (true);
    }
    return (false);
}

std::string HttpRequest::getHost() const
{
    std::map<std::string, std::vector<std::string> >::const_iterator header;
    header =_headers.find("host");
    if (header != _headers.end())
        return (header->second[0]);
    return (std::string());
}

void HttpRequest::setStatusCode(StatusCode status)
{
    _status = status;
}

StatusCode HttpRequest::getStatusCode() const
{
    return (_status);
}

const std::string &HttpRequest::getErrorMsg() const
{
    return (_errorMsg);
}

const std::string HttpRequest::getHeader(const std::string &field) const
{
    std::map<std::string, std::vector<std::string> > ::const_iterator header;
    
    header = _headers.find(field);
    if (header != _headers.end())
    {
        if (header->second[0].substr(0, 18) == "multipart/form-data")
            return ("multipart/form-data");
        return (header->second[0]);
    }
    return (std::string());
}

const std::map<std::string, std::string> &HttpRequest::getFormData() const
{
    return (_formData);
}

std::string HttpRequest::parseFieldname(const std::string &line, size_t *pos)
{
    if (!pos)
        return (std::string());
    size_t head_pos = *pos;
    size_t end_pos = line.find(':', head_pos); 
    if (end_pos == std::string::npos || end_pos <= head_pos) {
        return (std::string()); 
    }
    std::string fieldname = line.substr(head_pos, end_pos - head_pos);
    *pos = end_pos + 1;
    return (fieldname); 
}

std::string HttpRequest::parseFieldValue(const std::string &line, size_t *pos)
{
    if (!pos)
        return std::string();
    while (isSpace(line[*pos]))
        (*pos)++;
    size_t start = *pos; 
    size_t len = 0;
    while (line[*pos + len] && !isSpace(line[*pos + len]) && line[*pos + len] != ';')
        len++;
    std::string fieldvalue = line.substr(start, len);
    *pos += len;
    while (isSpace(line[*pos]))
        (*pos)++;
    return (fieldvalue);
}

void HttpRequest::splitFormLine(const std::string &line, MultiFormData *form)
{
    //extract field name
    size_t pos = 0;
    std::string fieldname = parseFieldname(line, &pos);
    if (fieldname.empty())
    {
        setStatusCode(BAD_REQUEST);
        return ;   
    }
    //check if ':' is not skipped
    if (line[pos] == ':')
        pos++;
    //check for OWS optional whitespace
    while (isSpace(line[pos]))
        pos++;
    //extract field value
    std::string fieldvalue = parseFieldValue(line, &pos);
    if (fieldvalue.empty())
    {
        setStatusCode(BAD_REQUEST);
        return ;   
    }
    //check extracted fieldvalue if there are more info
    std::stringstream ss(line);
    std::string token;
    fieldname = toLower(fieldname);
    while (std::getline(ss, token, ';'))
    {
        size_t equalPos = token.find('=');
        if (equalPos != std::string::npos)
        {
            std::string key = token.substr(0, equalPos);
            std::string value = token.substr(equalPos + 1);
            
            if (value[0] == '"' && value[value.size() - 1] == '"')
                value = value.substr(1, value.size() - 2);
            trimWhitespaces(key);
            key = toLower(key);
            if (!key.empty() && !value.empty())
                form->fields[key] = value;
        }
    }
    if (!fieldname.empty() && !fieldvalue.empty())
        form->fields[fieldname] = fieldvalue;
}


void HttpRequest::parseUntilBinary(const std::string &boundary, MultiFormData *form)
{
    const std::string separator = "--" + boundary;

    while (true)
    {
        std::string line = getLineAndPopFromBuffer();
        
        if (line == separator)
            continue ;
        if (line.empty())
            break ;
        splitFormLine(line, form);
        if (_status != OK)
            return ;
    }
    std::string type = form->fields["content-type"];
    if (type != "image/jpeg" && type != "image/jpg" && type != "image/gif" && type != "image/png" && type != "image/bmp")
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
}

void HttpRequest::parseBinary(const std::string &boundary, MultiFormData *form)
{
    if (_buffer.empty())
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }

    while (true)
    {
        if (_buffer.empty())
            return ;
        std::string line = getLineAndPopFromBuffer();
        if (line.empty())
            continue ;
        if ((line == "--" + boundary + "--") || line == "--" + boundary)
            break ;
        form->binary.insert(form->binary.end(), line.begin(), line.end());
    }
}


void HttpRequest::parseMultipartForm(const std::string &boundary)
{
    if (_buffer.empty() || boundary.empty())
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }
    MultiFormData form;
    
    parseUntilBinary(boundary, &form);
    if (_status != OK)
        return ;
    parseBinary(boundary, &form);
    if (_status != OK)
        return ;
    _multiFormData[form.fields["name"]] = form;
    
    std::cout << "================================" << std::endl;
    std::cout << "Multipart Form Data" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "_multiFormData size: " << _multiFormData.size() << std::endl;
    std::map<std::string, MultiFormData>::iterator it;
    for (it = _multiFormData.begin(); it != _multiFormData.end(); it++)
    {
        std::cout << "MultiFormData key: " << it->first << std::endl;
        std::cout << "fields size: " << it->second.fields.size() << std::endl;
        std::map<std::string, std::string>::iterator field;
        for (field = it->second.fields.begin(); field != it->second.fields.end(); field++)
        {
            std::cout << field->first << ": " << field->second << std::endl;
        }
        std::cout << "binary size: " << it->second.binary.size() << std::endl;
        std::vector<unsigned char>::iterator binary;
        for (binary = it->second.binary.begin(); binary != it->second.binary.end(); binary++)
        {
            std::cout << *binary;
        }
    }
}

void HttpRequest::parseRequestBody()
{
    std::cout << "================================" << std::endl;
    std::cout << "parsing request body" << std::endl;
    printBuffer();
    std::cout << "================================" << std::endl;
    std::string contentLen = getHeader("content-length");
    if (contentLen.empty())
    {
        setStatusCode(BAD_REQUEST);
        return ;
    }
    std::string type = getHeader("content-type");
    if (type.empty())
    {
        setStatusCode(UNSUPPORTED_MEDIA_TYPE);
        return ;
    }
    while (true)
    {
        std::string line = getLineAndPopFromBuffer();
        if (_buffer.empty())
            break;
        if (line.empty())
            continue ;
        switch(_parseStep)
        {
            case REQUEST_BODY:
            {
                if (getHeader("content-type") == "application/x-www-form-urlencoded")
                {
                    parseFormData(line);
                    if (_status != OK)
                        return ;
                }
                std::string boundary;
                if (isMultiPartFormData(&boundary))
                    parseMultipartForm(boundary);
                else
                {
                    setStatusCode(BAD_REQUEST);
                    break ;
                }
            }
            default:
                break ;
        }
    }
}

bool HttpRequest::isSupportedMediaPOST()
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

void HttpRequest::parseFormData(const std::string &line)
{
    std::stringstream ss(line);
    std::string pair;

    while (std::getline(ss, pair, '&'))
    {
        size_t equalPos = pair.find('=');
        if (equalPos == std::string::npos)
        {
            setStatusCode(BAD_REQUEST);
            return ;
        }
        std::string key = pair.substr(0, equalPos);
        std::string value = pair.substr(equalPos + 1);
        _formData[key] = value;
    }
}


bool    HttpRequest::isMultiPartFormData(std::string *boundary)
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