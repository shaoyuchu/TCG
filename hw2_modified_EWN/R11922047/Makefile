CC = g++
CFLAGS = -O2 -std=c++11
SRCDIR = src
OBJDIR = obj
TARGET = main.out

$(TARGET): $(OBJDIR)/board.o $(OBJDIR)/main.o $(OBJDIR)/mcts.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

profile: CFLAGS += -pg
profile: $(TARGET)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(TARGET)