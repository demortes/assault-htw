CC      = gcc
PROF    =
NOCRYPT =

# Debugging flags possible:  DEBUG_MEM DEBUG_MEM_CORRUPT DEBUG_MEM_DUP_FREE
# DEBUG_MEM is most basic, just checks magic numbers when freeing
# DEBUG_MEM_CORRUPT checks every free block to see if magic numbers ok, every
#                   call to alloc_mem
#		    Also makes calls to void check_free_mem(void) check for
#		    corrupted free memory blocks.
# DEBUG_MEM_DUP_FREE checks to see if freed blocks are overlapping every call
# 		     to mem_alloc.
#

#IMC2 - Comment out to disable IMC2 support
#IMC = 1
#  -DDEBUG_MEM -DDEBUG_MEM_CORRUPT -DDEBUG_MEM_DUP_FREE
C_FLAGS = -O0 -g3 -Wall -DACK_43 $(PROF) $(NOCRYPT) 
#C_FLAGS = -g3 -Wall ($PROF) $(NOCRYPT) -DOLD_RAND -Dunix
#-DDEBUG_MEM -DDEBUG_MEM_CORRUPT
# MCCP
# L_FLAGS = -O -g $(PROF)
L_FLAGS = -O0 -lz -lgd
# End MCCP
#L_FLAGS = $(PROF)

O_FILES = act_alliance.o act_comm.o act_info.o act_misc.o act_mob.o act_move.o act_obj.o \
          act_skills.o act_wiz.o comm.o areachk.o areasave.o board.o brands.o build.o \
          buildare.o buildings.o buildtab.o const.o db.o email.o engineer.o fight.o games.o \
          hack.o handler.o hash.o interp.o lists.o logs.o macros.o mapper.o mccp.o mxp.o \
          pdelete.o save.o scheck.o social-edit.o space.o ssm.o strfuns.o sysdata.o \
          terrain.o update.o web.o wizutil.o write.o

#   C_FILES := imc.c md5.c $(C_FILES)
#   O_FILES := imc.o md5.o $(O_FILES)
#   C_FLAGS := $(C_FLAGS) -DIMCACK

ack: $(O_FILES)
	rm -f ack
	$(CC) -o ack $(O_FILES) -lm -lcrypt -lz -lgd
	rm -f /ack

.c.o: ack.h
	$(CC) -c $(C_FLAGS) $<

scan: scan.o
	rm -f scan
	$(CC) $(L_FLAGS) -o scan scan.o -lcrypt

clean:
	rm -f $(O_FILES) ack ../area/core
#	make

