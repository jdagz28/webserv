# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/05 02:06:08 by jdagoy            #+#    #+#              #
#    Updated: 2024/07/07 02:50:59 by jdagoy           ###   ########.fr        #
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
						ServerConfig.hpp 
HEADER_FILES		:= $(addprefix $(INC_DIR), $(HEADER_LIST))


SRCS_LIST			:= main.cpp \
						utilities.cpp
OBJS_LIST 			:= $(patsubst %.cpp, %.o, $(SRCS_LIST))
OBJS				:= $(addprefix $(OBJ_DIR), $(OBJS_LIST))


CONFIG_DIR			:= $(SRC_DIR)config/
CONFIG_SRCS			:= Config.cpp \
						LocationConfig.cpp \
						ServerConfig.cpp
CONFIG_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(CONFIG_SRCS))
CONFIG_OBJS			:= $(addprefix $(OBJ_DIR), $(CONFIG_OBJS_LIST))

DEBUG_DIR			:= $(SRC_DIR)debug/
DEBUG_SRCS			:= configDebug.cpp
DEBUG_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(DEBUG_SRCS))
DEBUG_OBJS			:= $(addprefix $(OBJ_DIR), $(DEBUG_OBJS_LIST))


all: $(NAME) 

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(CONFIG_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(OBJ_DIR)%.o: $(DEBUG_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(NAME): $(OBJ_DIR) $(OBJS) $(CONFIG_OBJS) $(DEBUG_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(CONFIG_OBJS) $(DEBUG_OBJS) -o $(NAME)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re