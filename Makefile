NAME = webserv

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

INCLUDES = -I./include

SRCS = $(filter-out src/net/Parser.cpp, $(wildcard *.cpp  src/*.cpp src/parser/*.cpp src/net/*.cpp src/cgi/*.cpp))

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
	if [ -f cgi/main.cpp ]; then mv cgi/main.cpp cgi/main.txt; fi

show:
	if [ -f net/main.txt ]; then mv net/main.txt net/main.cpp; fi
	if [ -f parser/main.txt ]; then mv parser/main.txt parser/main.cpp; fi
	if [ -f cgi/main.txt ]; then mv cgi/main.txt cgi/main.cpp; fi

cgi:
	cd cgi; make

.PHONY: all clean fclean re hide show cgi
