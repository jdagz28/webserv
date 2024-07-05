# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jdagoy <jdagoy@student.s19.be>             +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/05 02:06:08 by jdagoy            #+#    #+#              #
#    Updated: 2024/07/05 03:39:00 by jdagoy           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME				:= webserv

CXX					:= c++
CXXFLAGS			:= -Wall -Wextra -Werror -std=c++98
RM					:= rm -rf

OBJ_DIR				:= ./objects/
INC_DIR				:= ./includes/
SRC_DIR				:= ./srcs/
INCLUDES			:= -I $(INC_DIR)

HEADER_LIST			:= 
HEADER_FILES		:= $(addprefix $(INC_DIR), $(HEADER_LIST))


SRCS_LIST			:= main.cpp
OBJS_LIST 			:= $(patsubst %.cpp, %.o, $(SRCS_LIST))
OBJS				:= $(addprefix $(OBJ_DIR), $(OBJS_LIST))


CONFIG_NAME			:= config
CONFIG_DIR			:= $(SRC_DIR)config/
CONFIG_HEADER_LIST	:= Config.hpp
CONFIG_HEADER_FILES	:= $(addprefix $(INC_DIR), $(CONFIG_HEADER_LIST))
CONFIG_SRCS			:= Config.cpp
CONFIG_OBJS_LIST 	:= $(patsubst %.cpp, %.o, $(CONFIG_SRCS))
CONFIG_OBJS			:= $(addprefix $(OBJ_DIR), $(CONFIG_OBJS_LIST))


all: $(CONFIG_NAME) #$(NAME) 

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp $(HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)%.o: $(CONFIG_DIR)%.cpp $(CONFIG_HEADER_FILES)
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

$(CONFIG_NAME): $(OBJ_DIR) $(OBJS) $(CONFIG_OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(CONFIG_OBJS) -o $(CONFIG_NAME)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)
	$(RM) $(CONFIG_NAME)

re: fclean all

.PHONY: all clean fclean re