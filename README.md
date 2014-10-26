README

These files are meant to increase the fixed point arithmetic capability of concorde. They
are stored in the following locations:

INCLUDE/bigguy.h
BIGGUY/bigguy.c
BIGGUY/Makefile.in
BIGGUY/128bg_test.c

After replacing the makefile, you will need to run ./configure again if you want to use 
unit tests in 128bg_test.c (or you can add the target to the makefile yourself). Don't
forget to remake the project after replacing the files.

If you have been experiencing overflow in BIGGUY_addmult, this patch should resolve your
issues. You may use the files provided here for any purpose, subject the restrictions
noted in bigguy.c and 128bg_test.c. Basically, if you get a non-academic licence from the
authors of concorde, you don't need to contact me to obtain permission to use the files 
provided here. However, using these files in concorde does not liberate you from the 
default "academic use only" of concorde. Also, I am never to be held liable for anything
related to the use of the software provided here -- it comes with no warranty.

(c) Jeffrey Scholz 2014
jss2245 at columbia dot edu
