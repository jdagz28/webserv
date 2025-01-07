# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdagz28 <jdagz28@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/05 02:06:08 by jdagoy            #+#    #+#              #
#    Updated: 2025/01/07 15:56:03 by jdagz28          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME				:= webserv

CXX					:= c++
CXXFLAGS			:= -Wall -Wextra -Werror -g -std=c++98
RM					:= rm -rf

OBJ_DIR				:= ./objects/
INC_DIR				:= ./includes/
SRC_DIR				:= ./srcs/
INCLUDES			:= -I $(INC_DIR)

HEADER_LIST			:= webserv.hpp \
						debug.hpp \
						Config.hpp \
						LocationConfig.hpp \
						ServerConfig.hpp \
						HttpRequest.hpp \
						HttpRequestLine.hpp \
						HttpResponse.hpp \
						Logger.hpp \
						Server.hpp\
						Socket.hpp
HEADER_FILES		:= $(addprefix $(INC_DIR), $(HEADER_LIST))


SRCS_LIST			:= main.cpp \
						utilities.cpp
OBJS_LIST 			:= $(patsubst %.cpp, %.o, $(SRCS_LIST))
OBJS				:= $(addprefix $(OBJ_DIR), $(OBJS_LIST))


CONFIG_DIR			:= $(SRC_DIR)config/
CONFIG_SRCS			:= Config.cpp \
						LocationConfig.cpp \
						ServerConfig.cpp \
						parseHttpBlock.cpp \
						parseServerBlock.cpp \
						parseLocationBlock.cpp
CONFIG_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(CONFIG_SRCS))
CONFIG_OBJS			:= $(addprefix $(OBJ_DIR), $(CONFIG_OBJS_LIST))

SERVER_DIR			:= $(SRC_DIR)server/
SERVER_SRCS			:= Server.cpp \
						Socket.cpp
SERVER_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(SERVER_SRCS))
SERVER_OBJS			:= $(addprefix $(OBJ_DIR), $(SERVER_OBJS_LIST))

DEBUG_DIR			:= $(SRC_DIR)debug/
DEBUG_SRCS			:= configPrint.cpp \
						httpRequestPrint.cpp \
						httpResponsePrint.cpp
DEBUG_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(DEBUG_SRCS))
DEBUG_OBJS			:= $(addprefix $(OBJ_DIR), $(DEBUG_OBJS_LIST))

HTTPREQUEST_DIR		:= $(SRC_DIR)http_requests/
HTTPREQUEST_SRCS	:= HttpRequest.cpp \
						HttpRequestLine.cpp\
						parseBody.cpp\
						parseHeader.cpp
HTTPREQUEST_OBJS_LIST := $(patsubst %.cpp, %.o, $(HTTPREQUEST_SRCS))
HTTPREQUEST_OBJS	:= $(addprefix $(OBJ_DIR), $(HTTPREQUEST_OBJS_LIST))

HTTPRESPONSE_DIR	:= $(SRC_DIR)http_response/
HTTPRESPONSE_SRCS	:= HttpResponse.cpp \
						MimeTypes.cpp \
						StatusCodes.cpp \
						Redirect.cpp \
						Get.cpp \
						GetDirectory.cpp\
						generateResponse.cpp \
						Error.cpp \
						Post.cpp \
						Delete.cpp
HTTPRESPONSE_OBJS_LIST := $(patsubst %.cpp, %.o, $(HTTPRESPONSE_SRCS))
HTTPRESPONSE_OBJS	:= $(addprefix $(OBJ_DIR), $(HTTPRESPONSE_OBJS_LIST))

LOGGER_DIR			:= $(SRC_DIR)logger/
LOGGER_SRCS			:= Logger.cpp
LOGGER_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(LOGGER_SRCS))
LOGGER_OBJS			:= $(addprefix $(OBJ_DIR), $(LOGGER_OBJS_LIST))


all: $(NAME) 

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(CONFIG_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(SERVER_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(DEBUG_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(HTTPREQUEST_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(HTTPRESPONSE_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(LOGGER_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(NAME): $(OBJ_DIR) $(OBJS) $(CONFIG_OBJS) $(SERVER_OBJS) $(DEBUG_OBJS) $(HTTPREQUEST_OBJS) $(HTTPRESPONSE_OBJS) $(LOGGER_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(CONFIG_OBJS) $(SERVER_OBJS) $(DEBUG_OBJS) $(HTTPREQUEST_OBJS)  $(HTTPRESPONSE_OBJS) $(LOGGER_OBJS) -o $(NAME)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

# DOCKER
build:
	docker build -t test-nginx .

start:
	docker run -d -p 4242:4242 --name test-nginx test-nginx

stop: 
	docker stop test-nginx

exec:
	docker run --entrypoint /bin/zsh -p 4242:4242 -it test-nginx
	
stop-containers:
	@if [ -n "$$(docker container ls -aq)" ]; then \
		docker container stop $$(docker container ls -aq); \
	fi

remove-containers:
	@if [ -n "$$(docker container ls -aq)" ]; then \
		docker container rm $$(docker container ls -aq); \
	fi

remove-images:
	@if [ -n "$$(docker images -aq)" ]; then \
		docker rmi -f $$(docker images -aq); \
	fi

remove: stop-containers remove-containers remove-images
