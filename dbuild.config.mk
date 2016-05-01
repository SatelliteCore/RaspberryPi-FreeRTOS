
#CFLAGS += -march=armv6z -g -Wall -Wextra -DRPI=3
CFLAGS += -march=armv7-a -g -Wall -Wextra -DRPI=3
CFLAGS += -I $(BASE)FreeRTOS/Source/portable/GCC/RaspberryPi/
CFLAGS += -I $(BASE)FreeRTOS/Source/include/
CFLAGS += -I $(BASE)Demo/Drivers/

TOOLCHAIN=arm-none-eabi-
