# Skip if being included again
ifndef DBUILD_CONFIG_MK
DBUILD_CONFIG_MK=1

# Select Raspberry PI model 1, 2, or 3 (default)
# Can override with command line like "make RPI=2"
RPI=3

# This sets ARM CPU model and defines RPI to the model
ifeq ($(RPI),1)
  CFLAGS += -march=armv6z -g -Wall -Wextra -DRPI=1
else ifeq ($(RPI),2)
  CFLAGS += -march=armv7-a -g -Wall -Wextra -DRPI=2
else ifeq ($(RPI),3)
  CFLAGS += -march=armv8-a -g -Wall -Wextra -DRPI=3
else
  $(error Unknown RPI model "$(RPI)")
endif

$(info Compiling for Raspberry Pi with RPI=$(RPI))

CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)Demo/Drivers/

TOOLCHAIN=arm-none-eabi-

endif
