/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 02:11:42 by jdagoy            #+#    #+#             */
/*   Updated: 2025/03/07 16:03:35 by jdagoy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <vector>
#include <map>
#include <string>
#include "HttpRequestLine.hpp"
#include "webserv.hpp"

struct MultiFormData
{
    std::map<std::string, std::string> fields;
    std::vector<unsigned char> binary;
};

class HttpRequest
{
    private:
        HttpRequestLine                                                     _request;
        std::map<std::string, std::vector<std::string> >                    _headers;
        int                                                                 _headersN;
        std::vector<unsigned char>                                          _buffer;
        StatusCode                                                          _status;
        std::string                                                         _errorMsg;
        int                                                                 _client_socket;
        std::map<std::string, std::string>                                  _formData;
        HtmlRequestParseStep                                                _parseStep;
        std::map<std::string, MultiFormData>                                _multiFormData;
        size_t                                                              _maxBodySize;
        
        
        HttpRequest();
		
        void    parseRequestLine(const std::string &line);
        void    parseRequestHeaders(const std::string &line);
        void    parseFormData(const std::string &line);
		std::string	parseFieldname(const std::string &line, size_t *pos);
        std::string	parseFieldValue(const std::string &line, size_t *pos);
        void	splitFormLine(const std::string &line, MultiFormData *form);
        void	parseUntilBinary(const std::string &boundary, MultiFormData *form);
        void	parseBinary(const std::string &boundary, MultiFormData *form);
        void	parseMultipartForm(const std::string &boundary);

        std::string     getLineAndPopFromBuffer();
        std::string     extract_token(const std::string &line, size_t &pos, char del);
        bool    isValidFieldName(const std::string &line);
        bool    isValidFieldValue(const std::string &line);
        std::vector<unsigned char>::iterator findBufferCRLF();
		size_t  getContentLengthBuffer(const std::string &header);

    public:
        HttpRequest(int client_socket);
        HttpRequest(const HttpRequest &copy);
        HttpRequest &operator=(const HttpRequest &copy);
        ~HttpRequest();

        void    setClientSocket(int client_socket);
        void    setStatusCode(StatusCode status);
        void    setMaxBodySize(size_t bodySize, const std::string &mode);

        const HttpRequestLine	&getRequestLine() const;
        const std::map<std::string, std::vector<std::string> >	&getHeaders() const;    
        std::string	getHost() const;
        StatusCode	getStatusCode() const;
        const std::string	&getErrorMsg() const;
        const std::string	getHeader(const std::string &field) const;
        const std::map<std::string, MultiFormData>	&getMultiFormData() const;
        const std::map<std::string, std::string>	&getFormData() const;
        std::string	getFormData(const std::string &method);
		size_t 	expectedTotalBytes();
		size_t	getBufferSize() const;

        bool    isConnectionClosed() const;
        bool    isMultiPartFormData(std::string *boundary);
        bool    isMultiPartFormData();
        bool    isForUpload();
        bool    isSupportedMediaPOST();
        bool    isSupportedMediaPOST(const std::string &type);
		bool	isHeadersComplete();
        
		void    parseHttpRequest();
        void    requestToBuffer(); 
        void    parseRequestBody();
        void    printBuffer() const;
		void	reset();
};


#endif
