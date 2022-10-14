CC = g++
CFLAGS = -O2 -std=c++11
SRCDIR = src
OBJDIR = obj
TARGET = main.out

$(TARGET): $(OBJDIR)/board.o $(OBJDIR)/main.o $(OBJDIR)/brute_force_solver.o
	$(CC) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)