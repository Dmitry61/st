ROOT=$(shell pwd)
PROJ_NAME = stm32f3

###################################################
# Set toolchain
TC = arm-unknown-eabi

# Set Tools
CC			= $(TC)-gcc
AR			= $(TC)-ar
OBJCOPY		= $(TC)-objcopy
OBJDUMP		= $(TC)-objdump
SIZE		= $(TC)-size
GDB			= $(TC)-gdb

SHELL = /bin/bash

# MUST NOT BE EMPTY
OBJ_DIR = obj

###################################################
# Set Sources
LIB_SRCS	= $(wildcard Libraries/STM32F30x_StdPeriph_Driver/src/*.c Libraries/STM32_USB-FS-Device_Driver/src/*.c)
USER_SRCS	= $(wildcard src/*.c)

# Set Objects
OBJS = $(addprefix $(OBJ_DIR)/, $(LIB_SRCS:.c=.o) $(USER_SRCS:.c=.o) src/startup_stm32f30x.o)


DIRS = $(sort $(dir $(OBJS)))
  
  
DEPENDENCIES = $(OBJS:.o=.d)

# Set Include Paths
INCLUDES 	= -ILibraries/STM32F30x_StdPeriph_Driver/inc/ \
			-ILibraries/STM32_USB-FS-Device_Driver/inc/ \
			-ILibraries/CMSIS/Include \
			-ILibraries/CMSIS/Device/ST/STM32F30x/Include \
			-Iinc/
			
# Set Libraries
LIBS		= -lm -lc

###################################################
# Set Board
MCU 		= -mthumb -mcpu=cortex-m4
FPU 		= -mfpu=fpv4-sp-d16 -mfloat-abi=hard
DEFINES 	= -DSTM32F3XX -DUSE_STDPERIPH_DRIVER -DARM_MATH_CM4 -DUSB_UART

# Set Compilation and Linking Flags
CFLAGS 		= $(MCU) $(FPU) $(DEFINES) $(INCLUDES) \
			-g -Wall -std=gnu11 -O3 -ffunction-sections -fdata-sections -fextended-identifiers
ASFLAGS 	= $(MCU) $(FPU) -g -Wa,--warn -x assembler-with-cpp
LDFLAGS 	= $(MCU) $(FPU) -g -gdwarf-2\
			-Tstm32f30_flash.ld \
			-Xlinker --gc-sections -Wl,-Map=$(PROJ_NAME).map \
			$(LIBS) \
			-o $(PROJ_NAME).elf

###################################################
# Default Target
all: $(PROJ_NAME).elf $(PROJ_NAME).bin info

# elf Target
$(PROJ_NAME).elf: $(OBJS)
	@$(CC) $(OBJS) $(LDFLAGS)
	@echo $@

# bin Target
$(PROJ_NAME).bin: $(PROJ_NAME).elf
	@$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin
	@echo $@

$(PROJ_NAME).hex: $(PROJ_NAME).elf
	@$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	@echo $@

$(PROJ_NAME).lst: $(PROJ_NAME).elf
	@$(OBJDUMP) -h -S $(PROJ_NAME).elf > $(PROJ_NAME).lst
	@echo $@

# Display Memory Usage Info
info: $(PROJ_NAME).elf
	@$(SIZE) --format=berkeley $(PROJ_NAME).elf

ifneq "$(MAKECMDGOALS)" "clean"
  -include $(DEPENDENCIES)
endif

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
	$(CC) -M \
	-MF $3 \
	-MP \
	-MT $2 \
	$(CFLAGS) \
	$1
endef

# Rule for .c files
$(OBJ_DIR)/%.o: %.c
	@echo $<
	@$(call make-depend,$<,$@,$(subst .o,.d,$@))
	@$(CC) $(CFLAGS) -xc -c -o $@ $<

# Rule for .s files
$(OBJ_DIR)/%.o: %.s
	@$(CC) $(ASFLAGS) -c -o $@ $<
	@echo $@

$(OBJS): | $(DIRS)

$(DIRS):
	@mkdir -p $@

# Clean Target
clean:
	rm -f $(OBJS)
	rm -f $(DEPENDENCIES)
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).map
	rm -rf $(OBJ_DIR)


install: $(PROJ_NAME).elf
	st-util 2>&1 >/dev/null & \
	$(GDB) $(PROJ_NAME).elf -ex "set confirm off" -ex "target extended-remote :4242" -ex "load" -ex "quit"

gdb: $(PROJ_NAME).elf
	st-util 2>&1 >/dev/null & \
	cgdb -d $(GDB) $(PROJ_NAME).elf -ex "target extended-remote :4242"

watch:
	while inotifywait --quiet --quiet -re close_write .; do $(MAKE); done
