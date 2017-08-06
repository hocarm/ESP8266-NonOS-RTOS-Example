# none
OTA ?= none

# Base directory for the compiler. Needs a / at the end; if not set it'll use the tools that are in
# the PATH.
XTENSA_TOOLS_ROOT ?=

# base directory of the ESP8266 SDK package, absolute
SDK_BASE	?= $(HOME)/ESP8266-Toolchain-217/sdk/ESP8266_RTOS_SDK

#Esptool.py path and port
ESPTOOL		?= $(HOME)/ESP8266-Toolchain-217/esptool/esptool.py
#ESPPORT		?= COM8
ESPPORT		?= /dev/ttyUSB0
#ESPDELAY indicates seconds to wait between flashing the two binary images
ESPDELAY	?= 3
#ESPBAUD		?= 115200
ESPBAUD		?= 460800

# 40m 26m 20m 80m
ESP_FREQ = 40m
# qio qout dio dout
ESP_MODE = qio
#4m 2m 8m 16m 32m
ESP_SIZE = 32m


VERBOSE = yes
FLAVOR = debug
# name for the target project
TARGET		?= esp8266-rtos-app

# which modules (subdirectories) of the project to include in compiling
USER_MODULES		= user driver
USER_INC				= include
USER_LIB				=


SDK_LIBDIR = lib
# SDK_LIBS = c gcc hal phy pp net80211 wpa main lwip crypto wps airkiss smartconfig ssl json
SDK_LIBS = gcc hal phy pp net80211 wpa crypto main freertos lwip minic smartconfig
# SDK_INC = include include/json
SDK_INC = extra_include include include/espressif include/json include/udhcp include/lwip include/lwip/lwip include/lwip/ipv4 include/lwip/ipv6


# Output directors to store intermediate compiled files
# relative to the project directory
BUILD_BASE				= build
FIRMWARE_BASE		= firmware

# Opensdk patches stdint.h when compiled with an internal SDK. If you run into compile problems pertaining to
# redefinition of int types, try setting this to 'yes'.
USE_OPENSDK ?= no

DATETIME := $(shell date "+%Y-%b-%d_%H:%M:%S_%Z")

# select which tools to use as compiler, librarian and linker
CC		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
AR		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-ar
LD		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
OBJCOPY	:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-objcopy



####
#### no user configurable options below here
####
SRC_DIR				:= $(USER_MODULES)
BUILD_DIR			:= $(addprefix $(BUILD_BASE)/,$(USER_MODULES))

INCDIR	:= $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(USER_INC))
MODULE_INCDIR	:= $(addsuffix /include,$(INCDIR))

SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_LIBS 		:= $(addprefix -l,$(SDK_LIBS))

SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INC))

SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
ASMSRC		= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.S))

OBJ		= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
OBJ		+= $(patsubst %.S,$(BUILD_BASE)/%.o,$(ASMSRC))

APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET).a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)



# compiler flags using during compilation of source files
# CFLAGS		= -g			\
# 			-Wpointer-arith		\
# 			-Wundef			\
# 			-Wl,-EL			\
# 			-fno-inline-functions	\
# 			-nostdlib       \
# 			-mlongcalls	\
# 			-mtext-section-literals \
# 			-ffunction-sections \
# 			-fdata-sections	\
# 			-fno-builtin-printf\
# 			-DICACHE_FLASH \
# 			-DBUID_TIME=\"$(DATETIME)\"

CFLAGS = -g -std=gnu90 -Os -Wpointer-arith -Wundef -Werror \
		 -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals \
		 -mno-serialize-volatile -D__ets__ -DICACHE_FLASH -DBUID_TIME=\"$(DATETIME)\" 

# linker flags used to generate the main object file
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static


ifeq ($(FLAVOR),debug)
    LDFLAGS += -g -O0
endif

ifeq ($(FLAVOR),release)
    LDFLAGS += -g -O2
endif

V ?= $(VERBOSE)
ifeq ("$(V)","yes")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

# ifeq ("$(USE_OPENSDK)","yes")
# CFLAGS		+= -DUSE_OPENSDK
# else
# CFLAGS		+= -D_STDINT_H
# endif

define maplookup
	$(patsubst $(strip $(1)):%,%,$(filter $(strip $(1)):%,$(2)))
endef



ESPTOOL_OPTS=--port $(ESPPORT) --baud $(ESPBAUD)

#32m
ESP_INIT_DATA_DEFAULT_ADDR = 0xfc000

ifeq ("$(ESP_SIZE)","16m")
	ESP_INIT_DATA_DEFAULT_ADDR = 0x1fc000
else ifeq ("$(ESP_SIZE)","32m")
	ESP_INIT_DATA_DEFAULT_ADDR = 0x3fc000
endif

OUTPUT := $(addprefix $(FIRMWARE_BASE)/,$(TARGET))
ESPTOOL_WRITE = write_flash --flash_freq $(ESP_FREQ) --flash_mode $(ESP_MODE) --flash_size $(ESP_SIZE) \
								0x00000 $(OUTPUT)0x00000.bin \
								0x20000 $(OUTPUT)0x20000.bin \
								$(ESP_INIT_DATA_DEFAULT_ADDR) $(SDK_BASE)/bin/esp_init_data_default.bin

ESPTOOL_FLASHDEF=
LD_SCRIPT	= -T$(SDK_BASE)/ld/eagle.app.v6.ld



vpath %.c $(SRC_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CFLAGS)  -c $$< -o $$@
endef



.PHONY: all checkdirs clean

# all: touch checkdirs $(OUTPUT)
all: checkdirs $(OUTPUT)

# touch:
# 	$(vecho) "-------------------------------------------\n"
# 	$(vecho) "BUID TIME $(DATETIME)"
# 	$(vecho) "-------------------------------------------\n"
# 	$(Q) touch user/user_main.c

checkdirs: $(BUILD_DIR) $(FIRMWARE_BASE)

$(OUTPUT): $(TARGET_OUT)
	$(vecho) "FW $@"
	$(Q) $(ESPTOOL) elf2image $(ESPTOOL_FLASHDEF) $< -o $(OUTPUT)

$(BUILD_DIR):
	$(Q) mkdir -p $@

$(FIRMWARE_BASE):
	$(Q) mkdir -p $@

$(TARGET_OUT): $(APP_AR)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(SDK_LIBS) $(APP_AR) -Wl,--end-group -o $@

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^

flash:
	$(ESPTOOL) $(ESPTOOL_OPTS) $(ESPTOOL_WRITE)

clean:
	$(Q) rm -rf $(BUILD_DIR)
	$(Q) rm -rf $(FIRMWARE_BASE)
$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))
