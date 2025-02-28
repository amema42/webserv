NAME = webserv

CXX = g++

CXXFLAGS = -Wall -Wextra -Werror -std=c++11

INCLUDES = -I../include

SRCS = $(wildcard *.cpp)

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

hide:
	if [ -f net/main.cpp ]; then mv net/main.cpp net/main.txt; fi
	if [ -f parser/main.cpp ]; then mv parser/main.cpp parser/main.txt; fi

show:
	if [ -f net/main.txt ]; then mv net/main.txt net/main.cpp; fi
	if [ -f parser/main.txt ]; then mv parser/main.txt parser/main.cpp; fi

.PHONY: all clean fclean re hide
