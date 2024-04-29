NAME	:=	webserv

SRC	:=	main.cpp \

SRCDIR 		:= 	./srcs
OBJDIR 		:= 	./objs

OBJS		:= 	$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))
SRCS		:= 	$(addprefix $(SRCDIR)/,$(SRC))

CC	:=	c++
CFLAGS	:= -Wall -Werror -Wextra -std=c++11

HEADERS := -I ./includes

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $^ -o $(NAME)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean:
	rm -f $(NAME) $(OBJS)

re: fclean all

.PHONY: all clean fclean re