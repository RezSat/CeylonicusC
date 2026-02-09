CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude

TARGET = ceylonicus
SRCS = main.c lexer.c utf8.c keywords.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET) $(TARGET).exe