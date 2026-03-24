CC = gcc
# Update -I to look inside src/include
CFLAGS = -Wall -Wextra -std=c11 -g -Isrc/include

TARGET = ceylonicus

# Define the source directory
SRCDIR = src

# Prepend the directory to your source files
SRCS = $(SRCDIR)/main.c $(SRCDIR)/lexer.c $(SRCDIR)/utf8.c $(SRCDIR)/keywords.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Update the pattern rule to handle files in the src directory
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET) $(TARGET).exe