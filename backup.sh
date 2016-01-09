#!/bin/bash
#Purpose = Backup of Important Data
#Created on 17-1-2012
#Author = Hafiz Haider
#Version 1.0
#START
TIME=`date +%b-%d-%y`            # This Command will add date in Backup File Name.
FILENAME=assaultbackup-$TIME.tar.gz    # Here i define Backup file name format.
SRCDIR=/home/demortes/assault-htw  # Location of Important Data Directory (Source of backup).
DESDIR=/home/demortes/Backups # Destination of backup file.
cd $SRCDIR/src;make clean
tar -cpzf $DESDIR/$FILENAME $SRCDIR
cd $SRCDIR/src;make -j10

#END
