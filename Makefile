# Directories
CUR_DIR := .
INC_DIR := $(CUR_DIR)/inc
LIB_DIR := $(CUR_DIR)/lib
OBJ_DIR := $(CUR_DIR)/obj
SRC_DIR := $(CUR_DIR)/src
BIN_DIR := $(CUR_DIR)/bin
STA_DIR := $(LIB_DIR)/static
SHARE_DIR := $(LIB_DIR)/shared

# Compiler and flags
#CC := /home/s2nmt/work/beagle_bone_black/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-gcc
CC := /home/s2nmt/work/beagle_bone_black//gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
CFLAGS := -Wall
INC_FLAG := -I $(INC_DIR)

# Library name
LIB_NAME := snake_game

# Object files
OBJS := $(OBJ_DIR)/oled_i2c_ssd1306.o $(OBJ_DIR)/button.o $(OBJ_DIR)/snake.o $(OBJ_DIR)/main.o

# Targets
all: sta_all share_all

# Compile object files for static linking
mk_objs_sta:
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(SRC_DIR)/oled_i2c_ssd1306.c -o $(OBJ_DIR)/oled_i2c_ssd1306.o $(INC_FLAG)
	$(CC) -c $(SRC_DIR)/button.c -o $(OBJ_DIR)/button.o $(INC_FLAG)
	$(CC) -c $(SRC_DIR)/snake.c -o $(OBJ_DIR)/snake.o $(INC_FLAG)
	$(CC) -c $(CUR_DIR)/main.c -o $(OBJ_DIR)/main.o $(INC_FLAG)

# Compile object files for shared linking with -fPIC flag
mk_objs_share:
	@mkdir -p $(OBJ_DIR)
	$(CC) -c -fPIC $(SRC_DIR)/oled_i2c_ssd1306.c -o $(OBJ_DIR)/oled_i2c_ssd1306.o $(INC_FLAG)
	$(CC) -c -fPIC $(SRC_DIR)/button.c -o $(OBJ_DIR)/button.o $(INC_FLAG)
	$(CC) -c -fPIC $(SRC_DIR)/snake.c -o $(OBJ_DIR)/snake.o $(INC_FLAG)
	$(CC) -c -fPIC $(CUR_DIR)/main.c -o $(OBJ_DIR)/main.o $(INC_FLAG)

# Create static library
mk_static:
	@mkdir -p $(STA_DIR)
	ar rcs $(STA_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/button.o $(OBJ_DIR)/oled_i2c_ssd1306.o $(OBJ_DIR)/snake.o

# Create shared library
mk_share:
	@mkdir -p $(SHARE_DIR)
	$(CC) -shared $(OBJ_DIR)/button.o $(OBJ_DIR)/snake.o $(OBJ_DIR)/oled_i2c_ssd1306.o -o $(SHARE_DIR)/lib$(LIB_NAME).so

# Install shared library to system
install:
	cp -f $(SHARE_DIR)/lib$(LIB_NAME).so /usr/lib

# Static linking executable
sta_all: mk_objs_sta mk_static
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o -L$(STA_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/main_static

# Shared linking executable
share_all: mk_objs_share mk_share install
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_DIR)/main.o -L$(SHARE_DIR) -l$(LIB_NAME) -o $(BIN_DIR)/main_shared

# Clean generated files
clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJ_DIR)/*
	rm -rf $(STA_DIR)/*
	rm -rf $(SHARE_DIR)/*
