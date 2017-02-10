CC     = gcc
TARGET = thread_pool_sample
CCFLAGS := -std=gnu99 -Wall -g

ifeq ($(DEBUG), 1)
CCFLAGS += -O0
else
CCFLAGS += -O2
endif

SRC_PATH := .
            
C_SRCS := $(SRC_PATH)/main.c \
            $(SRC_PATH)/list.c      \
            $(SRC_PATH)/thread_pool.c  \
            $(SRC_PATH)/ordered_thread_pool.c  

OBJECTS  := $(patsubst %.c,%.o,$(C_SRCS))

$(TARGET): $(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) -o $(TARGET) -lpthread 

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
