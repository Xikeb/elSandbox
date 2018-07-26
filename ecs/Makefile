##
## EPITECH PROJECT, 2018
## ElSandbox: ECS
## File description:
## Makefile
##

NAME		=	ecs

CXX		=	g++ -std=c++1y

RM		=	rm -f

SRCSDIR		=	./src

SRCS		=	./src/Entity.cpp	\
			./src/EntityHandle.cpp	\
			./src/main.cpp		\
			./src/Manager.cpp

OBJS		=	$(SRCS:.cpp=.o)

TESTNAME	=	test
TESTSRCSDIR	=	./src/tests
TESTSRCS	=	$(SRCS:.cpp=_test.cpp)
TESTSRCS	=	$(patsubst $(SRCSDIR)/%.cpp, $(TESTSRCSDIR)/%_test.cpp, $(SRCS))
TESTOBJS	=	$(TESTSRCS:.cpp=.o)

CPPFLAGS	=	-I ./include/ -I../ #-DNDEBUG
CPPFLAGS	+=	-W -Wall -Wextra -Werror

all: debug

exec:	$(NAME)

debug:	CPPFLAGS += -g3 -O0
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
