
CC=gcc
CFLAGS = -g -Wall -lm
LIBS += -lncurses -lpthread

SRC_FILES += \
  ../comm-prot/sm/uart-sm.c \
  ../comm-prot/pc/pc-key-rx.c \
  ../comm-prot/pc/pc-js-rx.c \
  ../comm-prot/pc/pc-uart-rx.c\
  ../comm-prot/pc/pc-uart-tx.c \
  modules/interface.c \
  modules/drone-state.c \
  modules/rs232_service.c \
  pc_terminal.c \
  modules/pc_logging.c \
  modules/hiredis/async.c \
  modules/hiredis/hiredis.c \
  modules/hiredis/net.c \
  modules/hiredis/read.c \
  modules/hiredis/sds.c \
  modules/redis.c \
  
INC_DIRS += \
  ./ \
  ../comm-prot/sm \
  ../comm-prot/pc \
  ../comm-prot/es \
  modules \
  modules/hiredis \

INC_PARAMS=$(foreach d, $(INC_DIRS), -I$d)

CUSTOMFLAGS = -DBUILD_PC
#CUSTOMFLAGS += -DDEBUG_MSG_NO_CHECKSUM
CUSTOMFLAGS += -DDEBUG_MSG_NO_ACK
#CUSTOMFLAGS += -DDEBUG_MSG_ACK
#CUSTOMFLAGS += -DDEBUG_MSG_ASCII
#CUSTOMFLAGS += -DDEBUG_RAW_MSG
#CUSTOMFLAGS += -DJS_DETECTION_OVERRIDE

EXEC = ./pc-terminal


all:
	$(CC) $(CFLAGS) $(INC_PARAMS) $(SRC_FILES) $(CUSTOMFLAGS) $(LIBS) -o $(EXEC)

run: all
	$(EXEC)
