########################################################################
# This Makefile creates valid core and stab files to be used with the  #
# uMPS2 emulator using Process Control Block and Active Semaphore List #
# interfaces. Made for phase1, use it with phase2.                     #
#                              					       				   #
# Gruppo: lso12az12			                               			   #
########################################################################

# Macros
MODULES = ./obj
SOURCES = ./src
INCLUDES = ./include
SCRIPT = ./script
LDSCRIPT = $(SCRIPT)/elf32ltsmip.h.umpscore.x
KERNELELF = ./bin/kernel

#PRETTY OUTPUT
GREEN=\033[32m
NORMAL=\033[37m

# Alias for kernelelf
all : compile link kernela
	@echo "\nDONE"

# Compile source and create obj
compile :
	@echo -n "COMPILING...  "
	@cd obj/ ; mipsel-linux-gcc -I ../$(INCLUDES) ../src/*.c -c
	@echo "$(GREEN)[OK]$(NORMAL)"

# Joins the obj files to create the kernel elf
link :
	@echo -n "LINKING...    "
	@mkdir -p bin
	@mipsel-linux-ld -T $(LDSCRIPT) $(MODULES)/*.o $(MODULES)/umps2/*.o -o $(KERNELELF)
	@echo "$(GREEN)[OK]$(NORMAL)"

# core and stab from the kernel elf file
kernela : 
	@echo -n "KERNEL...     "
	@mkdir -p bin kernel 
	@umps2-elf2umps -k $(KERNELELF)
	@mv ./bin/*.umps ./kernel/
	@echo "$(GREEN)[OK]$(NORMAL)"
	
# Clean and tidy (doesn't remove umps2 related object files)
clean :
	rm -rf bin kernel
	rm -rf ./obj/*.o
