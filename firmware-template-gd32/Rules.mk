PREFIX ?= arm-none-eabi-

CC	 = $(PREFIX)gcc
CPP	 = $(PREFIX)g++
AS	 = $(CC)
LD	 = $(PREFIX)ld
AR	 = $(PREFIX)ar

FAMILY?=gd32f20x
BOARD?=BOARD_GD32F207C_EVAL

FAMILY:=$(shell echo $(FAMILY) | tr A-Z a-z)
FAMILY_UC=$(shell echo $(FAMILY) | tr a-w A-W)

$(info $$FAMILY [${FAMILY}])
$(info $$FAMILY_UC [${FAMILY_UC}])

# Output 
TARGET=$(FAMILY).bin
LIST=$(FAMILY).list
MAP=$(FAMILY).map
BUILD=build_gd32/

# Input
SOURCE=./
FIRMWARE_DIR=./../firmware-template-gd32/
ifeq ($(strip $(BOARD)),BOARD_GD32F207C_EVAL)
	LINKER=$(FIRMWARE_DIR)gd32f207vc_flash.ld
else
	LINKER=$(FIRMWARE_DIR)gd32f207rg_flash.ld
endif

include ../firmware-template/libs.mk

LIBS+=c++ c gd32

$(info [${LIBS}])
	
DEFINES:=$(addprefix -D,$(DEFINES))
DEFINES+=-DCONFIG_STORE_USE_ROM

include ../firmware-template-gd32/Includes.mk

# The variable for the libraries include directory
LIBINCDIRS:=$(addprefix -I../lib-,$(LIBS))
LIBINCDIRS+=$(addsuffix /include, $(LIBINCDIRS))

# The variables for the ld -L flag
LIBGD32=$(addprefix -L../lib-,$(LIBS))
LIBGD32:=$(addsuffix /lib_gd32, $(LIBGD32))

# The variable for the ld -l flag 
LDLIBS:=$(addprefix -l,$(LIBS))

# The variables for the dependency check 
LIBDEP=$(addprefix ../lib-,$(LIBS))

$(info $$LIBDEP [${LIBDEP}])

COPS=-DBARE_METAL -DGD32 -DGD32F20X_CL -D$(BOARD)
COPS+=$(DEFINES) $(MAKE_FLAGS) $(INCLUDES)
COPS+=$(LIBINCDIRS)
COPS+=-Os -mcpu=cortex-m3 -mthumb
COPS+=-nostartfiles -ffreestanding -nostdlib
COPS+=-fstack-usage
COPS+=-Wstack-usage=1024
COPS+=-ffunction-sections -fdata-sections

CPPOPS=-std=c++11 
CPPOPS+=-Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference -fno-rtti -fno-exceptions -fno-unwind-tables
#CPPOPS+=-Wuseless-cast -Wold-style-cast
CPPOPS+=-fno-threadsafe-statics

LDOPS=--gc-sections --print-gc-sections

PLATFORM_LIBGCC+= -L $(shell dirname `$(CC) $(COPS) -print-libgcc-file-name`)

$(info $$PLATFORM_LIBGCC [${PLATFORM_LIBGCC}])

C_OBJECTS=$(foreach sdir,$(SRCDIR),$(patsubst $(sdir)/%.c,$(BUILD)$(sdir)/%.o,$(wildcard $(sdir)/*.c)))
C_OBJECTS+=$(foreach sdir,$(SRCDIR),$(patsubst $(sdir)/%.cpp,$(BUILD)$(sdir)/%.o,$(wildcard $(sdir)/*.cpp)))
ASM_OBJECTS=$(foreach sdir,$(SRCDIR),$(patsubst $(sdir)/%.S,$(BUILD)$(sdir)/%.o,$(wildcard $(sdir)/*.S)))

BUILD_DIRS:=$(addprefix $(BUILD),$(SRCDIR))

OBJECTS:=$(ASM_OBJECTS) $(C_OBJECTS)

define compile-objects
$(BUILD)$1/%.o: $(SOURCE)$1/%.cpp
	$(CPP) $(COPS) $(CPPOPS) -c $$< -o $$@	

$(BUILD)$1/%.o: $(SOURCE)$1/%.c
	$(CC) $(COPS) -c $$< -o $$@
	
$(BUILD)$1/%.o: $(SOURCE)$1/%.S
	$(CC) $(COPS) -D__ASSEMBLY__ -c $$< -o $$@
endef


all : builddirs prerequisites $(TARGET)
	
.PHONY: clean builddirs

builddirs:
	mkdir -p $(BUILD_DIRS)

.PHONY:  clean

clean: $(LIBDEP)
	rm -rf $(BUILD)
	rm -f $(TARGET)
	rm -f $(MAP)
	rm -f $(LIST)

#
# Libraries
#

.PHONY: libdep $(LIBDEP)

lisdep: $(LIBDEP)

$(LIBDEP):
	$(MAKE) -f Makefile.GD32 $(MAKECMDGOALS) 'FAMILY=${FAMILY}' 'BOARD=${BOARD}' 'MAKE_FLAGS=$(DEFINES)' -C $@ 

# Build uImage

$(BUILD_DIRS) :
	mkdir -p $(BUILD_DIRS)

$(BUILD)startup_$(FAMILY)_cl.o : $(FIRMWARE_DIR)/startup_$(FAMILY)_cl.S
	$(AS) $(COPS) -D__ASSEMBLY__ -c $(FIRMWARE_DIR)/startup_$(FAMILY)_cl.S -o $(BUILD)startup_$(FAMILY)_cl.o
	
$(BUILD)main.elf: Makefile.GD32 $(LINKER) $(BUILD)startup_$(FAMILY)_cl.o $(OBJECTS) $(LIBDEP)
	$(LD) $(BUILD)startup_$(FAMILY)_cl.o $(OBJECTS) -Map $(MAP) -T $(LINKER) $(LDOPS) -o $(BUILD)main.elf $(LIBGD32) $(LDLIBS) $(PLATFORM_LIBGCC) -lgcc 
	$(PREFIX)objdump -D $(BUILD)main.elf | $(PREFIX)c++filt > $(LIST)
	$(PREFIX)size -A -x $(BUILD)main.elf

$(TARGET) : $(BUILD)main.elf 
	$(PREFIX)objcopy $(BUILD)main.elf -O binary $(TARGET)	
	
$(foreach bdir,$(SRCDIR),$(eval $(call compile-objects,$(bdir))))