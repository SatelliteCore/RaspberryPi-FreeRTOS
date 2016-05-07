
# Select Raspberry PI model (1, 2, or 3)
#CFLAGS += -march=armv6z -g -Wall -Wextra -DRPI=1
#CFLAGS += -march=armv7-a -g -Wall -Wextra -DRPI=2
CFLAGS += -march=armv8-a -g -Wall -Wextra -DRPI=3

CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)Demo/Drivers/

TOOLCHAIN=arm-none-eabi-
