CC = g++
CFLAGS = -O2 -std=c++11
SRCDIR = src
OBJDIR = obj
TARGET = main.out

$(TARGET): $(OBJDIR)/main.o $(OBJDIR)/agent.o $(OBJDIR)/board.o $(OBJDIR)/solver.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

profile: CFLAGS += -pg
profile: $(TARGET)

profile: CFLAGS += -g
profile: $(TARGET)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(TARGET)