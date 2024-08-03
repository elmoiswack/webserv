NAME	:=	webserv

SRC	:=	main.cpp \
		Parser.cpp \
		Server.cpp \
		Validate.cpp \
		Location.cpp \
		Cgi.cpp \
		Request.cpp \
		Response.cpp \
		Client.cpp \
		Method.cpp \
		Html.cpp \

SRCDIR 		:= 	./srcs
OBJDIR 		:= 	./objs

OBJS		:= 	$(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))
SRCS		:= 	$(addprefix $(SRCDIR)/,$(SRC))

CC	:=	c++
CFLAGS	:= -Wall -Werror -Wextra -std=c++17

HEADERS := -I ./includes

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $^ -o $(NAME) $(CFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir $@

clean:
	rm -f $(OBJS)
	rm -rf $(OBJDIR)
	
fclean:
	rm -f $(NAME) $(OBJS)
	rm -rf $(OBJDIR)

re: fclean all

.PHONY: all clean fclean re