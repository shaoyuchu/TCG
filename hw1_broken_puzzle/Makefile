CC = g++
CFLAGS = -O2 -std=c++11
SRCDIR = src
OBJDIR = obj
TARGET = main.out

$(TARGET): $(OBJDIR)/board.o $(OBJDIR)/main.o
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)