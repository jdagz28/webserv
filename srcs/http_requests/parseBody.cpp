/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parseBody.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/25 12:44:49 by jdagoy            #+#    #+#             */
/*   Updated: 2025/02/05 12:09:42 by jdagoy           ###   ########.fr       */
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

std::string HttpRequest::parseFieldname(const std::string &line, size_t *pos)
{
    if (!pos)
        return (std::string());
    size_t head_pos = *pos;
    size_t end_pos = line.find(':', head_pos); 
    if (end_pos == std::string::npos || end_pos <= head_pos)
        return (std::string());
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
    size_t pos = 0;
    std::string fieldname = parseFieldname(line, &pos);
    if (fieldname.empty())
    {
        setStatusCode(BAD_REQUEST);
        return ;   
    }
    if (line[pos] == ':')
        pos++;
    while (isSpace(line[pos]))
        pos++;
    std::string fieldvalue = parseFieldValue(line, &pos);
    if (fieldvalue.empty())
    {
        setStatusCode(BAD_REQUEST);
        return ;   
    }
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
        return ;
    std::string boundaryStr = "--" + boundary;
    std::vector<unsigned char>::iterator boundaryIt;

    while (true)
    {
        boundaryIt = std::search(_buffer.begin(), _buffer.end(), boundaryStr.begin(), boundaryStr.end());
        if (boundaryIt == _buffer.end())
        {
            form->binary.insert(form->binary.end(), _buffer.begin(), _buffer.end());
            if (form->binary.size() > _maxBodySize)
            {
                setStatusCode(PAYLOAD_TOO_LARGE);
                return ;
            }
            _buffer.clear();
            break;
        }
        form->binary.insert(form->binary.end(), _buffer.begin(), boundaryIt);
		if (form->binary.size() > _maxBodySize)
		{
			setStatusCode(PAYLOAD_TOO_LARGE);
			return ;
		}
        _buffer.erase(_buffer.begin(), boundaryIt + boundaryStr.size());
        if (std::string(_buffer.begin(), _buffer.begin() + 2) == "--")
        {
			_buffer.erase(_buffer.begin(), _buffer.begin() + 2);
            break;
        }
    }
}
std::string byteToHex(unsigned char byte) 
{
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return ss.str();
}

void HttpRequest::parseMultipartForm(const std::string &boundary)
{
    if (_buffer.empty() || boundary.empty())
    {
        setStatusCode(INTERNAL_SERVER_ERROR);
        return ;
    }
    MultiFormData form;
    
    while (!_buffer.empty())
    {
        parseUntilBinary(boundary, &form);
        if (_status != OK)
            return ;
		std::cout << "Buffer size before parsing binary: " << _buffer.size() << std::endl;
        parseBinary(boundary, &form);
		std::cout << "Buffer size after parsing binary: " << _buffer.size() << std::endl;
        if (_status != OK)
            return ;
        _multiFormData[form.fields["name"]] = form;
    }
	
	// std::cout << "================================" << std::endl;
    // std::cout << "Multipart Form Data" << std::endl;
    // std::cout << "================================" << std::endl;
    // std::cout << "_multiFormData size: " << _multiFormData.size() << std::endl;
    // std::map<std::string, MultiFormData>::iterator it;
    // for (it = _multiFormData.begin(); it != _multiFormData.end(); it++)
    // {
    //     std::cout << "MultiFormData key: " << it->first << std::endl;
    //     std::cout << "fields size: " << it->second.fields.size() << std::endl;
    //     std::map<std::string, std::string>::iterator field;
    //     for (field = it->second.fields.begin(); field != it->second.fields.end(); field++)
    //     {
    //         std::cout << field->first << ": " << field->second << std::endl;
    //     }
    //     std::cout << "binary size: " << it->second.binary.size() << std::endl;
    //     std::vector<unsigned char>::iterator binary;
    //     for (binary = it->second.binary.begin(); binary != it->second.binary.end(); binary++)
    //     {
    //         std::cout << byteToHex(*binary) << " ";
    //     }
    //     std::cout << std::endl;
    // }
}

void HttpRequest::parseRequestBody()
{
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
    // printBuffer();
    if (_parseStep != REQUEST_BODY)
        _parseStep = REQUEST_BODY;
    while (true)
    {
        if (_buffer.empty())
			break;
		std::string line = getLineAndPopFromBuffer();
        if (line.empty())
            continue ;
        switch(_parseStep)
        {
            case REQUEST_BODY:
            {
                if (type == "application/x-www-form-urlencoded")
                {
                    parseFormData(line);
                    if (_status != OK)
                        return ;
                }
				else if (type.find("multipart/form-data") != std::string::npos)
				{
					std::string boundary;
					if (isMultiPartFormData(&boundary)) 
					{
						parseMultipartForm(boundary);
					}
					if (_status != OK)
						return ;
				}
                _parseStep = REQUEST_DONE;
            }
            default:
                break ;
        }
    }
}

void HttpRequest::parseFormData(const std::string &line)
{
    std::string key;
    std::string token;
    std::istringstream iss(line);
    int i = 0; 

    while (std::getline(iss, token, '&'))
    {
        size_t equalPos = token.find('=');
        if (equalPos != std::string::npos)
        {    
            key = token.substr(0, equalPos);
            if (key != "q" && key != "_method" && key != "files")
            {
                setStatusCode(BAD_REQUEST);
                return ;
            }
            token = token.substr(key.size() + 1);
        }
        if (key == "files")
            key = key + '_' + toString(i);
        _formData[key] = token;
        i++;
    }
}