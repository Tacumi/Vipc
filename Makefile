CC = clang
TARGET = vipc
OBJS = vipc.o
$(TARGET):	$(OBJS)
	$(CC) $^ -o $@
