C=gcc
C_FLAGS = -g -Wall -Wextra  
DIR=/data/nginx-1.9.15/
TARGETS=ngx_pool_test
TARGETS_FILE=$(TARGETS).c

all:$(TARGETS)

clean:
	rm -f $(TARGETS) *.o

CORE_INCS=-I $(DIR)/src/core/ \
		  -I $(DIR)/objs/ \
		  -I $(DIR)/src/event \
		  -I $(DIR)/src/event/modules \
		  -I $(DIR)/src/os/unix \
		  -I $(DIR)/Nginx_Pre/pcre-8.32/

NGX_OBJ = $(DIR)/objs/src/core/ngx_palloc.o \
		  $(DIR)/objs/src/core/ngx_string.o \
		  $(DIR)/objs/src/os/unix/ngx_alloc.o \
		  $(DIR)/objs/src/core/ngx_array.o

$(TARGETS):$(TARGETS_FILE)
	$(CC) $(C_FLAGS) $(TARGETS_FILE) $(CORE_INCS) $(NGX_OBJ) -o $@

