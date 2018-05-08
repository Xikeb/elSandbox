##
## EPITECH PROJECT, 2018
## ElSandbox: ECS
## File description:
## Makefile
##

NAME		=	ecs

CXX		=	g++

RM		=	rm -f

SRCSDIR		=	./src

SRCS		=	./src/Entity.cpp	\
			./src/EntityHandle.cpp	\
			./src/Manager.cpp

OBJS		=	$(SRCS:.cpp=.o)

TESTNAME	=	test
TESTSRCSDIR	=	./src/tests
TESTSRCS	=	$(SRCS:.cpp=_test.cpp)
TESTSRCS	=	$(patsubst $(SRCSDIR)/%.cpp, $(TESTSRCSDIR)/%_test.cpp, $(SRCS))
TESTOBJS	=	$(TESTSRCS:.cpp=.o)

CPPFLAGS	=	-I ./include/ -I../
CPPFLAGS	+=	-W -Wall -Wextra -Werror

all: debug

exec:	$(NAME)

debug:	CPPFLAGS += -ggdb
debug:	exec

$(NAME): $(OBJS)
	 $(CXX) $(OBJS) -o $(NAME) $(LDFLAGS)

test: $(TESTOBJS)
	$(CXX) $(TESTOBJS) -o $(TESTNAME) $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TESTOBJS)

fclean: clean
	$(RM) $(NAME) $(TESTNAME)

re: fclean all

.PHONY: all clean fclean re
