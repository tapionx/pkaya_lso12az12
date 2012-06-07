########################################################################
# This Makefile creates valid core and stab files to be used with the  #
# uMPS2 emulator using Process Control Block and Active Semaphore List #
# interfaces. Made for phase1, use it with phase2.                     #
#                                 								       #
# So Long and Thanks for All The Fish!                                 #
########################################################################

# Macros
MODULES = ./modules
OBJECTS = /usr/lib/umps2/crtso.o /usr/lib/umps2/libumps.o \
	$(MODULES)/pcb/pcb.o $(MODULES)/asl/asl.o $(MODULES)/p1test/p1test.o 
LDSCRIPT = /usr/share/umps2/elf32ltsmip.h.umpscore.x
KERNELELF = ./bin/kernel


# Alias for kernelelf
all : kernelelf

# core and stab from the kernel elf file
kernel : all
	-mkdir bin kernel
	umps2-elf2umps -k ./bin/kernel
	mv ./bin/*.umps ./kernel/
	echo "\nFinished creating the core and symbol table files! CHECK FOR ERRORS BEFORE EXECUTING!\n"
	
# Joins the obj files to create the kernel elf
kernelelf : pcbdir asldir p1testdir
	-mkdir bin
	mipsel-linux-ld -T $(LDSCRIPT) $(OBJECTS) -o $(KERNELELF)
	echo "\nFinished creating the kernel elf file! CHECK FOR ERRORS CONVERTING IT INTO CORE AND STAB FILES!\n"
	
# Creates pcb obj file
pcbdir :
	cd $(MODULES)/pcb ; make all

# Creates asl obj file
asldir : 
	cd $(MODULES)/asl ; make all
	
# Creates p1test obj file
p1testdir : 
	cd $(MODULES)/p1test ; make all

# Clean and tidy
clean :
	rm -Rf bin kernel
	find ./ -name "*.o" | xargs rm -f
