# Copyright 2016, Gustavo Muro
# All rights reserved.
#
# This file is part of Workspace.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# application name
PROJECT_NAME := blinking_con_makefile

# Modules needed by the application

# source files folder
PROJECT_SRC_FOLDERS := src

# header files folder
PROJECT_INC_FOLDERS := inc

# source files
PROJECT_C_FILES := $(wildcard src/*.c)

OUT_PATH = out
OBJ_PATH = $(OUT_PATH)/obj

PROJECT_OBJ_FILES := $(addprefix $(OBJ_PATH)/,$(notdir $(PROJECT_C_FILES:.c=.o)))

PROJECT_OBJS := $(notdir $(PROJECT_OBJ_FILES))

# Defined symbols
SYMBOLS += -DDEBUG -DCORE_M4 -D__USE_LPCOPEN -D__LPC43XX__ -D__CODE_RED \
           -DLPC43_MULTICORE_M0APP -D__MULTICORE_MASTER \
					 -D__MULTICORE_MASTER_SLAVE_M0APP

# Compilation flags
CFLAGS  := -Wall -ggdb3 -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 \
           -mfloat-abi=softfp -fdata-sections -ffunction-sections

# Linking flags
LFLAGS  := -nostdlib -fno-builtin -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 \
           -mfloat-abi=softfp -Xlinker -Map=$(OUT_PATH)/$(PROJECT_NAME).map \
			  -Wl,--gc-sections

# Linker scripts
LD_FILE := -Tetc/ld/lpc4337_m4_lib.ld -Tetc/ld/lpc4337_m4_mem.ld \
           -Tetc/ld/lpc4337_m4.ld

INCLUDES := $(addprefix -I,$(PROJECT_INC_FOLDERS))

vpath %.c $(PROJECT_SRC_FOLDERS)

%.o: %.c
	@echo "*** compiling C file $< ***"
	@arm-none-eabi-gcc $(SYMBOLS) $(CFLAGS) $(INCLUDES) -c $< -o $(OBJ_PATH)/$@
	@arm-none-eabi-gcc $(SYMBOLS) $(CFLAGS) $(INCLUDES) -c $< -MM > $(OBJ_PATH)/$(@:.o=.d)

all : $(PROJECT_OBJS)
	@echo "*** linkeando... ***"
	@arm-none-eabi-gcc $(LFLAGS) $(LD_FILE) -o $(OUT_PATH)/$(PROJECT_NAME).axf $(PROJECT_OBJ_FILES) -L$(OUT_PATH)
	
clean:
	rm -f $(OBJ_PATH)/*.*
	rm -f $(OUT_PATH)/*.*
	rm -f *.launch

info:
	@echo PROJECT_NAME: $(PROJECT_NAME)
	@echo OBJS: $(PROJECT_OBJS)
	@echo INCLUDES: $(INCLUDES)
	@echo PROJECT_SRC_FOLDERS: $(PROJECT_SRC_FOLDERS)
	