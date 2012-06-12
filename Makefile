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


# Alias for kernelelf
all : format kernelelf
	@echo "\n\n>>>>>>>>>>>>>> END OF COMPILATION <<<<<<<<<<<<\n\n"

# Format the output
format : 
	@echo "\n\n************ STARTING TO COMPILE ************\n\n"

# core and stab from the kernel elf file
kernel : all
	@echo "\n\n************ CREATING THE KERNEL ************\n\n"
	-mkdir bin kernel 
	umps2-elf2umps -k $(KERNELELF)
	-mv ./bin/*.umps ./kernel/
	@echo "\nFinished creating the core and symbol table files! CHECK FOR ERRORS BEFORE EXECUTING!\n"
	@echo "\n\n>>>>>>>>>>>>>> KERNEL CREATED <<<<<<<<<<<<\n\n"
	
# Joins the obj files to create the kernel elf
kernelelf : source
	-mkdir bin
	mipsel-linux-ld -T $(LDSCRIPT) $(MODULES)/*.o $(MODULES)/umps2/*.o -o $(KERNELELF)
	@echo "\nFinished creating the kernel elf file! CHECK FOR ERRORS CONVERTING IT INTO CORE AND STAB FILES!\n"
	
# Creates obj files
source :
	cd obj/ ; mipsel-linux-gcc -I ../$(INCLUDES) ../src/*.c -c

# Clean and tidy (doesn't remove umps2 related object files)
clean :
	rm -rf bin kernel
	rm -rf ./obj/*.o
