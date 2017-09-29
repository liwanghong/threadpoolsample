CC     = gcc
CCFLAGS := -std=gnu99 -Wall -g

ifeq ($(DEBUG), 1)
CCFLAGS += -O0
else
CCFLAGS += -O2
endif

SRC_PATH := .
            
C_SRCS := $(SRC_PATH)/list.c      \
            $(SRC_PATH)/thread_pool.c  \
            $(SRC_PATH)/ordered_thread_pool.c  

TEST_SRCS := $(SRC_PATH)/main.c

all: libthreadpool.so libthreadpool.a threadpooltest

OBJECTS  := $(patsubst %.c,%.o,$(C_SRCS))
TEST_OBJECTS := $(patsubst %.c,%.o,$(TEST_SRCS))

libthreadpool.so: $(OBJECTS)
	$(CC) $(CCFLAGS) -shared -o $@ $^ 

libthreadpool.a: $(OBJECTS)
	ar -rcs $@ $^ 
    

threadpooltest: $(TEST_OBJECTS) libthreadpool.a
	$(CC) $(CCFLAGS) -o $@ $^ -lpthread

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.so threadpooltest
