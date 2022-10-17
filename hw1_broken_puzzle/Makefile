CC = g++
CFLAGS = -O2 -std=c++11
SRCDIR = src
OBJDIR = obj
TARGET = main.out
DB_TARGET = pattern_database_generator.out

$(TARGET): $(OBJDIR)/board/board.o $(OBJDIR)/board/a_star_board.o $(OBJDIR)/main.o $(OBJDIR)/brute_force_solver.o $(OBJDIR)/a_star_solver.o
	$(CC) $(CFLAGS) $^ -o $@

$(DB_TARGET): $(OBJDIR)/board/board.o $(OBJDIR)/board/a_star_board.o $(OBJDIR)/board/pattern_db_board.o $(OBJDIR)/a_star_solver.o $(OBJDIR)/pattern_database_generator.o
	$(CC) $(CFLAGS) $^ -o $@

profile: CFLAGS += -pg
profile: $(TARGET)

database: $(DB_TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET) $(DB_TARGET)