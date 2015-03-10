.PHONY:clean all

EXE = test
LIB = librawsocket.a

LIBS = rawsocket
LIBS_SEARCH_PATH = ./
INCLUDES = 

CC = g++ -ggdb3 -O0 -Wall -Wextra -Werror -Wconversion -Wshadow
AR = ar crs 

RM = rm -rf
MKDIR = mkdir -p

DIR_BUILD = build
DIR_OBJS = objs
DIR_DEPS = deps

DIR_OBJS := $(addprefix $(DIR_BUILD)/,$(DIR_OBJS))
DIR_DEPS := $(addprefix $(DIR_BUILD)/,$(DIR_DEPS))
DIRS = $(DIR_BUILD) $(DIR_OBJS) $(DIR_DEPS)

SRCS = $(wildcard *.cpp)
OBJS = $(addprefix $(DIR_OBJS)/,$(SRCS:.cpp=.o))
DEPS = $(addprefix $(DIR_DEPS)/,$(SRCS:.cpp=.dep))

ifneq ($(LIBS),"")
LIBS := $(addprefix -l,$(LIBS))
endif

ifneq ($(LIBS_SEARCH_PATH),"")
LIBS_SEARCH_PATH := $(addprefix -L,$(LIBS_SEARCH_PATH))
endif

ifneq ($(INCLUDES),"")
INCLUDES := $(addprefix -I,$(INCLUDES))
endif

$(shell mkdir -p $(DIRS))

all:$(EXE)

ifneq ($(LIB),"")
all : $(LIB)
endif

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

$(EXE) : $(LIB) $(DIR_OBJS)/$(EXE).o
	$(CC) $(LIBS_SEARCH_PATH) -o $@ $^ $(LIBS) 

$(LIB) : $(filter-out $(DIR_OBJS)/$(EXE).o, $(OBJS))
	$(AR) -o $@ $^
	
$(DIR_DEPS)/%.dep : %.cpp Makefile
	@echo "Making $@ ..."
	@set -e;\
		$(CC) -E -MM $(filter %.cpp, $^) | sed 's,\(.*\)\.o[ :]*,$(DIR_OBJS)/\1.o $@:,g' > $@

$(DIR_OBJS)/%.o : %.cpp Makefile
	$(CC) $(INCLUDES) -o $@ -c $(filter %.cpp, $^)  

clean:
	$(RM) $(DIR_OBJS) $(DIR_DEPS) $(EXE) $(LIB)

