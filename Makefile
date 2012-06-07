########################################################################
# This Makefile creates valid core and stab files to be used with the  #
# uMPS2 emulator using Process Control Block and Active Semaphore List #
# interfaces. Made for phase1, use it with phase2.                     #
#                              					       #
# Gruppo: lso12az12			                               #
########################################################################

# Macros
MODULES = ./lib
SCRIPT = ./script
OBJECTS = $(MODULES)/umps2/crtso.o $(MODULES)/umps2/libumps.o \
	$(MODULES)/pcb/pcb.o $(MODULES)/asl/asl.o $(MODULES)/p2test/myp2test.o \
	$(MODULES)/utils/utils.o
LDSCRIPT = $(SCRIPT)/elf32ltsmip.h.umpscore.x
KERNELELF = ./bin/phase2_kernel


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
kernelelf : pcbdir asldir p2testdir utilsdir
	-mkdir bin
	mipsel-linux-ld -T $(LDSCRIPT) $(OBJECTS) -o $(KERNELELF)
	@echo "\nFinished creating the kernel elf file! CHECK FOR ERRORS CONVERTING IT INTO CORE AND STAB FILES!\n"
	
# Creates pcb obj file
pcbdir :
	cd $(MODULES)/pcb ; make all

# Creates asl obj file
asldir : 
	cd $(MODULES)/asl ; make all
	
# Creates utils obj file
utilsdir :
	cd $(MODULES)/utils ; make all
	
# Creates p1test obj file
p1testdir : 
	cd $(MODULES)/p1test ; make all
	
# Creates p2test obj file
p2testdir :
	cd $(MODULES)/p2test ; make all

# Clean and tidy (doesn't remove umps2 related object files)
clean :
	rm -Rf bin kernel
	find $(MODULES)/pcb -name "*.o" | xargs rm -f
	find $(MODULES)/asl -name "*.o" | xargs rm -f
	find $(MODULES)/p1test -name "*.o" | xargs rm -f
	find $(MODULES)/p2test -name "*.o" | xargs rm -f
	find $(MODULES)/utils -name "*.o" | xargs rm -f
