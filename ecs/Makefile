##
## EPITECH PROJECT, 2018
## ElSandbox: ECS
## File description:
## Makefile
##

NAME		=	ecs

CXX		=	g++ -std=c++1z

RM		=	rm -f

SRCSDIR		=	./src

SRCS		=	$(SRCSDIR)/Entity.cpp		\
			$(SRCSDIR)/EntityHandle.cpp	\
			$(SRCSDIR)/main.cpp		\
			$(SRCSDIR)/Manager.cpp

OBJS		=	$(SRCS:.cpp=.o)

TESTNAME	=	unit-tester

TESTSRCSDIR	=	./src/tests
TESTSRCS	=	$(TESTSRCSDIR)/Entity_test.cpp			\
				$(TESTSRCSDIR)/Machinery_test.cpp		\
				$(TESTSRCSDIR)/System_test.cpp 			\
				$(TESTSRCSDIR)/EntityHandle_test.cpp	\
				$(TESTSRCSDIR)/Manager_test.cpp			\
				$(TESTSRCSDIR)/main_test.cpp

EXAMPLENAME = example

EXAMPLESRCSDIR = ./src/example
EXAMPLESRCS = $(EXAMPLESRCSDIR)/main.cpp

EXAMPLEOBJS		=	$(EXAMPLESRCS:.cpp=.o)

# TESTSRCS	=	$(patsubst $(SRCSDIR)/%.cpp, $(TESTSRCSDIR)/%_test.cpp, $(SRCS))

TESTOBJS	=	$(TESTSRCS:.cpp=.o)

CPPFLAGS	=	-I ./include/ -I../ -DNDEBUG
CPPFLAGS	+=	-W -Wall -Wextra -Werror

LDFLAGS		=
TESTLDFLAGS	=	-L /usr/local/lib -lgtest -lgtest_main -lpthread 

all: debug test

exec:	CPPFLAGS += -s -Ofast
exec:	$(NAME)

debug:	CPPFLAGS += -s -Ofast
# debug:	CPPFLAGS += -g3 -O0
debug:	$(NAME)

test: CPPFLAGS += -s -Ofast
 # test: CPPFLAGS += -g3 -O0
test: $(TESTOBJS)
	 $(CXX) $(TESTOBJS) -o $(TESTNAME) $(TESTLDFLAGS)

$(NAME): $(OBJS)
	 $(CXX) $(OBJS) -o $(NAME) $(LDFLAGS)

$(EXAMPLENAME): $(EXAMPLEOBJS)
	 $(CXX) $(EXAMPLEOBJS) -o $(EXAMPLENAME) $(LDFLAGS)

clean:
	$(RM) $(OBJS) $(TESTOBJS) $(EXAMPLEOBJS)

fclean: clean
	$(RM) $(NAME) $(TESTNAME) $(EXAMPLENAME)

re: fclean all
retest: fclean test

.PHONY: all clean fclean re
