/* GUI Converter v1.78 readme.txt                                             */
/* Copyright 1986-2004 QAD Inc., Carpinteria, CA, USA.                        */
/* All rights reserved worldwide.  This is an unpublished work.               */


================================================================================
BUILD INFO
================================================================================
$ uname -a
SunOS ohsu04 5.8 Generic_108528-16 sun4u sparc SUNW,Ultra-Enterprise
$ gcc -v
Reading specs from /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/3.0.3/specs
Configured with: ../configure --with-as=/usr/local/bin/as --with-ld=/usr/local/bin/ld --enable-libgcj
Thread model: posix
gcc version 3.0.3
$ gcc convert.c   
$ file a.out
a.out:          ELF 32-bit MSB executable SPARC Version 1, dynamically linked, not stripped
$ mv a.out gui_convert.178.SunOS

$ uname -a
HP-UX ohhp40 B.11.00 U 9000/889 266804311 unlimited-user license
$ cc convert.c -V
cpp: HP92453-01 A.11.00.00 HP C (Bundled) Preprocessor
ccom: HP92453-01 A.11.00.00 HP C (Bundled) Compiler
/usr/ccs/bin/ld: 92453-07 linker linker ld B.11.30 011005
$ file a.out
a.out:          PA-RISC2.0 shared executable dynamically linked -not stripped
$ mv a.out gui_convert.178.HP-UX

$ uname -a
Linux coli49 2.4.21-20.ELsmp #1 SMP Wed Aug 18 20:46:40 EDT 2004 i68
$ gcc --version
gcc (GCC) 3.2.3 20030502 (Red Hat Linux 3.2.3-42)
Copyright (C) 2002 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
$ gcc convert.c
$ file a.out
a.out: ELF 32-bit LSB executable, Intel 80386, version 1 (SYSV), for GNU/Linux 2.2.5, dynamically linked (uses shared libs), not stripped
$ mv a.out gui_convert.178.Linux

C:\DJGPP\GUICONV>echo %os%
Windows_NT
C:\DJGPP\GUICONV>gcc --version
gcc.exe (GCC) 3.4.2
Copyright (C) 2004 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
C:\DJGPP\GUICONV>gcc convert.c -o convert.exe -D_DOS -D__BORLANDC__
C:\DJGPP\GUICONV>dir convert.exe
11/30/2004  03:50 PM           189,991 convert.exe



================================================================================
HOW TO ORDER
================================================================================

MFGPRO release date        GUI Converter QAD item number
-------------------------- ------------- ---------------
< March, 1996              v1.66         99-5059
March 1996 - February 1998 v1.69         99-5060
> February 1998            v1.78         99-50??



================================================================================
SAMPLE CONVERT.INI FILES
================================================================================
srcSearchPath=h:\work92
targetDir=h:\work92\mfg4win
workDir=h:\work92

srcSearchPath=/users/dzn/work91
targetDir=/users/dzn/work91/mfg4win
workDir=/users/dzn/work91


================================================================================
GUI CONVERTER REVISION HISTORY
================================================================================
v1.77-	Released: 25-Jul-03
*  Initialize "logfile" in main() to avoid the Linux "initializer element is not constant" error
* Change the FullGUIReport "find" statement printing to search for the end-of-statement instead of ".". For example, qualified fields are now output as expected.
BEFORE: find first gl_ctrl where gl_ctrl.
NOW:    find first gl_ctrl where gl_ctrl.gl_domain = global_domain no-lock.
* version number 1.76 => version number 1.77
* Ported to DJGPP gcc v3.2.3. Created Windows executable with the command
*  gcc convert.c -o convert.exe -D_DOS -D__BORLANDC__

V1.78 Released: 30-Nov-04 
*           The Selection Criteria label in Full GUI Reports
*           did not take advantage of translated labels, this new version
*           takes the label if defined in label master.
*           Copyright 2003 => 2004 v177 =>v1.78

