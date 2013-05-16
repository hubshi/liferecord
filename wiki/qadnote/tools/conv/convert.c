/*******************************************************************************
*
*      C O N V E R T - written by jeff seawards, qad.inc.
*
*      COPYRIGHT qad.inc. ALL RIGHTS RESERVED. THIS IS AN UNPUBLISHED WORK.
*
*    This program is a simple Progress V6 4GL tokenizer that can be taught
* to inspect and/or modify source code. Specifically, this version
* of this program will do the following:
*
* (1) Parse Progress V6 programs written to the MFG/PRO standards and
* restructure the programs and add Progress V7 language elements so that
* the programs look nice running on a GUI platform (MS-Windows, Motif). This
* is called a GUI-Lite conversion.
*
* (2) Move the {mfdtitle.i} or {mfdeclre.i ...} statement to top-of-file.
*
* (3) Perform a scan of all fields in data-entry statements and list them.
*
* (4) Work as a pretty program to reformat ugly code.
*
*   When doing a GUI-Lite conversion (1), this program needs to know if it
* is converting a 'maintenance' type program, or a 'report' type program,
* where the difference is defined as such: report programs prepare printed
* output and only do user input through a single frame 'FRAME A', all other
* programs are maintenence. Different modifications are done based on this
* fact. All other modes don't care about source code type.
*
* Usage: convert -m|r|g|d|f|p[v] Filename ...
*        convert -M|R|G|D|F|P[v] [ Listfile ]
*
*   The capital versions of the command line arguments process a list of
* filenames instead of filenames from the command line. If a Listfile is
* not specified a default one is used. The options m/M, r/R, g/G, specify
* Maintenace, Report, or Full-GUI Report GUI-Lite conversion mode. Their
* default Listfiles are mt.wrk, rp.wrk, and rpmain.wrk respectivly. The
* d/D, f/F, p/P  options specify Declare-move mode, Field-list mode, and
* Pretty mode. Their default Listfile is always src.wrk.
*   The [v] option means verbose.
*
*   A "convert.ini" file must exist in the local directory to run "convert".
* This ini-file specifies input source directories, output target directory,
* and a work directory. There are 5 lines in this ini-file, e.g.:
*   oldSrcDir=
*   curSrcDir=/d25/gui/83/src
*   newSrcDir=
*   targetDir=/d25/gui/cnvtr/tmp
*   workDir=
* Source files will be read from the three source directories in the order,
* newSrcDir, curSrcDir, oldSrcDir, thus allowing a three-level source
* repository. Any of them can be left blank, but not all three.
* Output files will be written to the targetDir, where the new filename is
* the same as the input file, except for Full-GUI report conversion which
* change the iq/rp characters in the filename to gi/gr (if there is no iq/rp
* in the name a _g or g is added to the name, see openOutFile() for the
* exact algorithm). The workDir specifies the direcotry that holds the
* Listfiles, it will default to the current directory if it is blank in
* the convert.ini file. NOTE: Field-list mode writes output to file named
* "fldlist.txt" in the target-directory. All Error, Warning, and Verbose
* messages go to the logfiles m=status.mt r=status.rp g=status.gr, all other
* modes use status.cnv.
*
* Examples:
*  convert -mv myprog.p
*  convert -Rv reports.wrk
*  convert -G
*
* This program is a smart about recognizing blatant report source code
* when operating in Maintenence conversion mode, and it will stop the
* conversion with a "REPORT CODE recognized" message. It notes the header,
* page-top, and page-bottom keywords and frame widths > 80 as report code,
* as well as the MFG/PRO standard report includes.
*======================================================================
*  Borland C 3.1 IDE settings for making convert.exe
*
*  +-[o]------------- Code Generation -----------------+
*  |                                                   |
*  |  Model           Options                          |
*  |   ( ) Tiny        [X] Treat enums as ints         |
*  |   ( ) Small       [ ] Word alignment              |
*  |   ( ) Medium      [ ] Duplicate strings merged    |
*  |   ( ) Compact     [ ] Unsigned characters         |
*  |   (X) Large       [ ] Pre-compiled headers        |
*  |   ( ) Huge        [ ] Generate assembler source   |
*  |                   [ ] Compile via assembler       |
*  |                                                   |
*  |  Assume SS Equals DS                              |
*  |   (X) Default for memory model                    |
*  |   ( ) Never                                       |
*  |   ( ) Always                                      |
*  |                                                   |
*  |  Defines                                          |
*  |                                                   |
*  |                   OK   =    Cancel =     Help  =  |
*  |                 ========    ========    ========  |
*  +---------------------------------------------------+
*  +-[o]------- Entry/Exit Code Generation -----------+
*  |                                                  |
*  |  Prolog/Epilog Code Generation                   |
*  |   (X) DOS standard                               |
*  |   ( ) DOS overlay                                |
*  |   ( ) Windows all functions exportable           |
*  |   ( ) Windows explicit functions exported        |
*  |   ( ) Windows smart callbacks                    |
*  |   ( ) Windows DLL all functions exportable       |
*  |   ( ) Windows DLL explicit functions exported    |
*  |                                                  |
*  |  Calling Convention  Stack Options               |
*  |   (X) C               [X] Standard stack frame   |
*  |   ( ) Pascal          [ ] Test stack overflow    |
*  |   ( ) Register                                   |
*  |                                                  |
*  |                  OK   =    Cancel =     Help  =  |
*  |                ========    ========    ========  |
*  +--------------------------------------------------+
*  +-[o]---------------- Optimization Options ---------------------+
*  |                                                               |
*  |  Optimizations                      Register Variables        |
*  |   [ ] Global register allocation     ( ) None                 |
*  |   [ ] Invariant code motion          ( ) Register keyword     |
*  |   [ ] Induction variables            (X) Automatic            |
*  |   [ ] Loop optimization                                       |
*  |   [ ] Suppress redundant loads      Common Subexpressions     |
*  |   [ ] Copy propagation               (X) No optimization      |
*  |   [ ] Assume no pointer aliasing     ( ) Optimize locally     |
*  |   [ ] Dead code elimination          ( ) Optimize globally    |
*  |   [ ] Jump optimization                                       |
*  |   [ ] Inline intrinsic functions    Optimize For              |
*  |   [X] Standard stack frame           (X) Size                 |
*  |   [ ] Object data calling            ( ) Speed                |
*  |                                                               |
*  |      Default       =   Fastest Code  =   Smallest Code =      |
*  |      ===============   ===============   ===============      |
*  |  -----------------------------------------------------------  |
*  |                                                               |
*  |                               OK   =    Cancel =     Help  =  |
*  |                             ========    ========    ========  |
*  +---------------------------------------------------------------+
*  +-[o]--------------- Source Options --------------------+
*  |                                                       |
*  |  Keywords                     Source Options          |
*  |   ( ) Borland C++              [ ] Nested comments    |
*  |   (X) ANSI                                            |
*  |   ( ) UNIX V                  Identifier Length 32    |
*  |   ( ) Kernighan and Ritchie                           |
*  |                                                       |
*  |                       OK   =    Cancel =     Help  =  |
*  |                     ========    ========    ========  |
*  +-------------------------------------------------------+
*  +-[o]----------- Portability Warnings ----------------+
*  |                                                     |
*  |   [X] Non-portable pointer conversion               |
*  |   [X] Non-portable pointer comparison               |
*  |   [X] Constant out of range in comparison           |
*  |   [ ] Constant is long                              |
*  |   [ ] Conversion may lose significant digits        |
*  |   [ ] Mixing pointers to signed and unsigned char   |
*  |                                                     |
*  |                     OK   =    Cancel =     Help  =  |
*  |                   ========    ========    ========  |
*  +-----------------------------------------------------+
*  +-[o]--------------- ANSI Violations -------------------+
*  |                                                       |
*  |   [X] Void functions may not return a value           |
*  |   [X] Both return and return of a value used          |
*  |   [X] Suspicious pointer conversion                   |
*  |   [X] Undefined structure 'ident'                     |
*  |   [X] Redefinition of 'ident' is not identical        |
*  |   [X] Hexadecimal value more than three digits        |
*  |   [ ] Bit fields must be signed or unsigned int       |
*  |   [X] 'ident' declared as both external and static    |
*  |   [ ] Declare 'ident' prior to use in prototype       |
*  |   [X] Division by zero                                |
*  |   [X] Initializing 'ident' with 'ident'               |
*  |   [ ] Initialization is only partially bracketed      |
*  |                                                       |
*  |                       OK   =    Cancel =     Help  =  |
*  |                     ========    ========    ========  |
*  +-------------------------------------------------------+
*  +-[o]---------------- Frequent Errors --------------------+
*  |                                                         |
*  |   [ ] Function should return a value                    |
*  |   [X] Unreachable code                                  |
*  |   [X] Code has no effect                                |
*  |   [ ] Possible use of 'ident' before definition         |
*  |   [ ] 'ident' is assigned a value which is never used   |
*  |   [ ] Parameter 'ident' is never used                   |
*  |   [X] Possibly incorrect assignment                     |
*  |                                                         |
*  |                         OK   =    Cancel =     Help  =  |
*  |                       ========    ========    ========  |
*  +---------------------------------------------------------+
*  +-[o]--------- Less Frequent Errors -------------+
*  |                                                |
*  |   [ ] Superfluous & with function              |
*  |   [ ] Ambiguous operators need parentheses     |
*  |   [ ] Structure passed by value                |
*  |   [ ] No declaration for function 'ident'      |
*  |   [ ] Call to function with no prototype       |
*  |   [ ] Unknown assembler instruction            |
*  |   [X] Ill-formed pragma                        |
*  |   [X] Condition is always (true/false)         |
*  |   [X] Array variable 'ident' is near           |
*  |   [ ] 'ident' declared but never used          |
*  |                                                |
*  |                OK   =    Cancel =     Help  =  |
*  |              ========    ========    ========  |
*  +------------------------------------------------+
*
*======================================================================
*
* MODS After Oct 20, 1994.
*  make smarter about Full GUI reports, Field trigger for validates.
*  isNextToken() now handles skipping over COMMENTS
*  handle Window-Close problem via global-beam-me-up flag
*  handle EDITING clauses and IF NOT BATCHRUN syntax in Full-GUI reports
*  Added File-list & multi-file processing capabilities
*  Compile on MS-Visual C++ for to make a DOS Application program
*    Set: 386 code/Warn 2/No MS extensions/Large-Model & Linker: + oldnames lib
*    WARNING: uses setjmp/longjmp for error handling, CANNOT make C++  !!
* 13-feb-95 Jeff Seawards    to eliminate false UPDATE/SET/PROMPT-FORs,
*           TwoMany, the counter for UPDATE/SET/PROMPT-FORs, was initialized
*           correctly at the start of each file instead of only once.
* 13-feb-95 Jeff Seawards    the number of characters allowed for parameters
*           that follow {mfrpexit.i XXXX} was increased from 9 to 32 to allow
*           for "false" as a parameter.
* 13-feb-95 Doug Norton    added "\n" after "WARN1 No MFDTITLE" message
* 14-feb-95 Jeff Seawards    added '\0' to the end of the new selprtArg
* 13-feb-95 Jeff Seawards    changed v1.1 => v1.2 to indicate above changes
* 16-feb-95 Doug Norton    added recognition of the RPMainCaller= converter
*           directive to allow for reports that call include files containing
*           the code that does all the work.
* 16-feb-95 Doug Norton    changed the requirement that all reports must have
*           mfdtitle from a fatal error to a warning.
* 16-feb-95 Doug Norton    changed the RENAME message to "contains MFDTitle"
*           for full-gui reports that contain MFDTitle.
* 16-feb-95 Doug Norton    v1.2 => 1.3 to reflect changes since 14-feb-95
* 17-feb-95 Doug Norton    changed the convert.ini variables from separate
*           dirs to a search path. (oldSrcDir=, newSrcDir= to srcSearchPath=)
* 17-feb-95 Doug Norton    v1.3 => v1.4 to reflect the ini changes
* 18-feb-95 Doug Norton    added more info to "too BIG" error.
* 19-feb-95 Doug Norton    put 'twinkler' under conditional display (-notwinkle)
* 19-feb-95 Doug Norton    allowed read of DOS listFile in UNIX
* 22-feb-95 Doug Norton    changed usage of verbose option to -v
* 22-feb-95 Doug Norton    added converter directive to specify convert mode
* 27-feb-95 Doug Norton    added -ini=foo.ini command line option
* 27-feb-95 Doug Norton    v1.4 => 1.43 to reflect changes since 17-feb-95
* 28-feb-95 Doug Norton    Enabled recognition of convertMode tokens.
*           Check for EOF when searching for V8+.
*           1.43 => 1.44 to reflect changes since 27-feb-95.
* 03-mar-95 Doug Norton    restored default -G status file name to status.gr.
*           It had been rpmain.wrk in v1.4 through v1.44
* 03-mar-95 Doug Norton    1.44 => 1.45 to indicate change of 03-mar-95
* 06-mar-95 Doug Norton    Added optional "overwriteExistingTarget=no"
*           line to ini file.  This causes exit on overwrite attempt.
* 07-mar-95 Doug Norton    1.45 => 1.46 to indicate addition of overwrite flag
* 08-mar-95 Doug Norton    added DontRefreshTitle directive to prevent refresh
*           of frame titles.
*           Ported to Borland C.
* 09-mar-95 Doug Norton    1.46 => 1.47 to indicate addition of
*           DontRefreshTitle directive
* 14-mar-95 Doug Norton    added NonStreamIoDirective to identify frames that
*           the converter incorrectly thinks need to be stream-io'd
* 15-mar-95 Doug Norton    added ConcatString directive to concatenate
*           strings within a message file.
* 15-mar-95 Doug Norton    1.47 => 1.48 to indicate changes since 09-mar-95
* 20-mar-95 Doug Norton    added check for close comment inside strings while
*           looking for the next close comment
* 20-mar-95 Doug Norton    1.48 => 1.49 to indicate changes since 15-mar-95
* 24-mar-95 Doug Norton    full gui report mode no longer changes the output
*           file name using the mangle algorithm.
* 25-mar-95 Doug Norton    1.49 => 1.50 indicates the non-"name mangle" change
* 25-mar-95 Doug Norton    1.50 => 1.51 indicates removal of broken-nonStreamIo
* 25-mar-95 Doug Norton    1.51 => 1.52 indicates fix of nonStreamIoFrames,
*           new port to Microsoft Visual C
* 27-mar-95 Doug Norton    1.52 => 1.53 indicates fix of incorrect refresh
*           of titles by using frameNameFoundInList()
* 28-mar-95 Doug Norton    added EditableDownFrame directive to allow some
*           scrolling frames to get the THREE-D attribute.
* 29-mar-95 Doug Norton    1.53 => 1.54 for EditableDownFrame addition
* 29-mar-95 Doug Norton    1.54 => 1.55 EditableDownFrame can now be {&frame}
* 05-apr-95 Jeff Seawards  1.55 => 1.56 Fixed foundInFrameList()
* 12-apr-95 Doug Norton    added StartedPReport global to track if the printing
*           of the "end procedure. / * p-report * /" is needed.
* 12-apr-95 Doug Norton    1.56 => 1.57 to show the new condition on p-report
* 20-apr-95 Doug Norton    Added the "ConvertMode=ConditionalIncludeExclude"
*           to allow only conditional includes and excludes to take place.
*           This is acutally the same as Declare/Move Mode.  1.57 => 1.58
* 22-apr-95 Doug Norton    allowed the -L parameter to specify that a list of
*           source files of misc types will be used as input.  1.58 => 1.59
*           This isn't fully debugged yet.
* 27-apr-95 Doug Norton    changed the processing of NoConvert files to
*           be copied with a 'system' command as soon as the NoConvert token
*           has been found.
*           Force encrypted files to be treated as NoConvert files.
*           changed version number 1.59 => 1.60.
* 15-oct-95 Doug Norton    added new ConvertMode token, ReportAndMaintenance
*           This token causes both maintenance and report attributes to be
*           output to the destination file, and the appropriate attributes
*           chosen at compile time via preprocessor directives.
*           added #define SELECTION_TITLE  for easier translations
*           changed version number 1.60 => 1.63a.
* 17-oct-95 Doug Norton    added \n before &IF REPORT... preproc directive.
*           Don't set CONVERT_MODE = REPORT in ReportAndMaintenance.
*           Conditionally output the title section of the FORM's WITH clause.
*           changed version number 1.63a => 1.63b.
* 19-oct-95 Doug Norton    appended "\n" to BEAM_ME_UP string to avoid
*           Progress "Colon followed by whitespace terminates a stmt" error.
*           Don't generate "NO-BOX" if already encountered.
*           changed version number 1.63b => 1.63c.
* 19-oct-95 Doug Norton    changed version number 1.63c => 1.63.
* 30-oct-95 Rand Clark     only output trail include if original program
*                          had trail
*                          changed version number 1.63 => 1.64
* 31-oct-95 Doug Norton    Don't treat 1 DOWN frames as down frames
*           Print a warning message when 1 DOWN frame found.
*           This allows title to be refreshed properly.
*           Changed version number 1.64 => 1.65.
* 02-nov-95 Doug Norton    Initialize globals properly in -L mode.
*           Changed version number 1.65 => 1.66.
* 09-jan-96 Doug Norton    Fixed erroneous application of "STREAM-IO" to
*           selected "nonStreamIoFrame"s.
*           Added #define PROGRAM_BUFFER_SIZE
*           Changed Copyright statement to 1996.
*           Don't parse the whole file when the convert mode is NoConvert
*           or NoConvertNoOutput
*           Allowed Encrypted files to be recognized again.
*           Changed version number 1.66 => 1.67
* 11-feb-96 Doug Norton    Close output file as soon as ConvertMode=NoConvert
*           is identified to avoid DOS "Sharing violation" error when copying
*           source file to (previously open) output file.
*           Eliminate -D and -d command line options.  (The declMode option
*           was supposed to allow only moving mfdtitle to top of file, but
*           was never used.  The variable used to track the declMode had been
*           hijacked and was used for NoConvert, NoConvertNoOutput, and
*           ConditionalIncludeExclude file parsing, and needed to be recycled
*           on a per-file basis when processing a list of files.
*           Removed -D and -d from the command line syntax help message.
*           Changed version number 1.67 => 1.68.
* 06-mar-96 Rand Clark     Have all reports, not just FullGui, include
*           mfgrptrm.i.
*           Add parsing of mfrpchk arguments and fix same for mfrpexit.
*           Initialize UIlist to space, fixes problem when nothing in list
*           Changed version number 1.68 => 1.69.
* There are now two output destinations in prd_det which direct report
* and inquiry output to the screen: WINDOW and TERMINAL.  When WINDOW
* is selected, output is read into an Editor Widget, allowing
* Cut/Paste/Search and File IO; this functionality was previously only
* available as TERMINAL from FullGui reports.  It is now available to
* all reports and inquiries as WINDOW.  This remedies the problem that
* many inquiries had when using TERMINAL: each screen was unpainted before
* the next batch of data was painted.
*
* TERMINAL behaves as it previously did for GUILite reports; additionally,
* for the sake of consistency, FullGui reports now implement TERMINAL
* by creating a window and directing paged output to it.
*
* To enable your private database to be WINDOW-aware, you need a pr_mstr
* record with pr_type of "gui-wind" and a prd_det with prd_type of
* "gui-wind" and prd_dev of "window".  For guidance, consult public
* databases.
*
* In conclusion, if you want to use the Notepad-like Editor Widget,
* select WINDOW.  If you want to "Press space bar to continue" to move
* through your output data, use TERMINAL.
* 17-feb-98 Doug Norton    allow {&foo} to be recognized as a preprocessor
*           directive.  Enabling this Progress v7+ syntax allows the
*           "Translated Strings" project to be simplified.
*           Warning message "Ignored CLEAR" => "Did not refresh frame title
*           after CLEAR"
*           ported code to Symantec C++ v7.2
*           unix program buffer size 150000 => 524288 (512kb)
*           DOS  program buffer size 65000  => 524288 (512kb)
*           DOS GUI Converter now requires WinNT (recommended) or Win95.
*           Changed version number 1.69 => 1.70, copyright 1996 => 1998
* 28-feb-98 Doug Norton    change " Selection Criteria " to (possibly)
*           be a preprocessor variable
*           version number 1.70 => 1.71
* 06-apr-99 Kevin Schantz srcPath length 80 => MAX_TOK_LEN
*           srcSearchPath length 256 => MAX_TOK_LEN
*           version number 1.72
* 19-aug-99 Doug Norton    comment out "Indent LEVEL mismatch" warning
*           MAX_TOK_LEN 256     => MAX_TOK_LEN 1024
*           MAX_BLOCKS 200      => MAX_BLOCKS MAX_TOK_LEN
*           srcSearchPath[256]  => srcSearchPath[MAX_TOK_LEN]
*           workDir[256]        => workDir[MAX_TOK_LEN]
*           targetDir[256]      => targetDir[MAX_TOK_LEN]
*           version number 1.71 => version number 1.73
* 21-mar-00 Doug Norton    Allow alternate representation, "(getFrameTitle())",
*           for the frame title to enable the externalized labels to be
*           used for frame titles. Changed the "Complex TITLE" message status
*           from warning to error, as frame titles can no longer contain
*           concatenated strings. No MFGPRO source code had frame titles of
*           this format anyway.
*           Add "Windows Enhanced Printing" enhancement, that is, recognize
*           gpselout.i as a substitute for mfselbpr.i.
*           Changed filename variables of fixed size 12 to 256.
*           Removed useless messages: "RENAME, contains MFDTitle" and "RENAME".
*           Changed usage message to
*             [-v] [-notwinkle] [-ini=iniFile] Filename ...
*             [-v] [-notwinkle] [-ini=iniFile] -L [Listfile]
*           Porting note: Microsoft apparently no longer defines the following
*           symbols by default in the compiler, and they must be added to the
*           compile command: _DOS __MSVC__
*           version number 1.73 => version number 1.75
* 17-Jul-01 Doug Norton    Fixed a bug that didn't allow gpselout.i's
*           withBatchOption to have "yes" be recognized as the param value
*           in FullGUIReport files.
*           Changed gpselout.i parameter parsing to not require a separate line
*           for each token, but to allow simple tokens to be separated by
*           whitespace.
*           Changed the maximum string length from 32 to 64 for the following
*           framescope, filedTrigNames, gpseloutParams, tokTypeAsString.
*           Changed usage message lines as shown
* <filename> v1.76, Copyright 1994-2001 QAD Inc., Carpinteria, CA, USA.
*  [-v] [-notwinkle] [-ini=iniFile] Filename [Filename...]
* 21-Jul-03 Doug Norton    Initialize "logfile" in main() to avoid the Linux
*           "initializer element is not constant" error.
*           Ported to DJGPP gcc v3.2.3. Created Windows executable with the
*           command "gcc convert.c -o convert.exe -D_DOS -D__BORLANDC__".
*           Change the FullGUIReport "find" statement printing to search for the
*           end-of-statement instead of ".". For example
* BEFORE: find first gl_ctrl where gl_ctrl.
*    NOW: find first gl_ctrl where gl_ctrl.gl_domain = global_domain no-lock.
*           Copyright 2001 => 2003, v1.76 => v1.77
* 30-Nov-04 Luis Romero   The Selection Criteria label in Full GUI Reports 
*           did not take advantage of translated labels, this new version 
*           takes the label if defined in label master.
*           Copyright 2003 => 2004 v177 =>v1.78
*******************************************************************************/

#if __BORLANDC__ || __SC__
#define _DOS 1
#endif /*__BORLANDC__*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <setjmp.h>
#if _DOS
#include <stdlib.h>
#include <io.h>
#define DIRCHAR '\\'
#define SYSTEM_COPY_CMD "copy"
#else
#define DIRCHAR '/'
#define SYSTEM_COPY_CMD "cp"
#endif

/*------------------------------------------------------------------------------
** This include file contains a preinitialized data strucutre that holds a
** table of Progress V6 Keywords (~350).
**----------------------------------------------------------------------------*/
#include "keywords.h"


#define STAR           '*'
#define EOLN           '\n'
#define EOF_CHAR       '\0'
#define MAX_DIGIT_COUNT 20
#define MAX_EXPONENT    37
#define MAX_TOK_LEN     1024
#define MAX_INDLEV      20
#define MAX_BLOCKS      MAX_TOK_LEN
#define ARGBUF_LEN      64
#define DEFAULT_MAINTENENCE_LISTFILE "mt.wrk"
#define DEFAULT_MAINTENENCE_STATUSFILE "status.mt"
#define DEFAULT_REPORT_LISTFILE "rp.wrk"
#define DEFAULT_REPORT_STATUSFILE "status.rp"
#define DEFAULT_FULLGUI_REPORT_LISTFILE "rpmain.wrk"
#define DEFAULT_FULLGUI_REPORT_STATUSFILE "status.gr"
#define DEFAULT_LISTFILE "src.wrk"
#define DEFAULT_STATUSFILE "status.cnv"
#define INI_FILENAME "convert.ini"
#define NOT_FOUND -1
#define SHORT_STRING_LENGTH 256
#define ENCRYPTION_CHECK_BYTE_COUNT 256
#define PROGRAM_BUFFER_SIZE 524288
#define GPSELOUT_I "gpselout.i"
#define MFSELBPR_I "mfselbpr.i"
#define MFSELPRT_I "mfselprt.i"
#define GET_FRAME_TITLE_FUNCTION_NAME "getFrameTitle"

/* Translatable string */
#ifndef SELECTION_TITLE
#define SELECTION_TITLE \
    "&IF DEFINED(GPLABEL_I)=0 &THEN\n" \
    "   &IF (DEFINED(SELECTION_CRITERIA) = 0)\n" \
    "   &THEN \" Selection Criteria \"\n" \
    "   &ELSE {&SELECTION_CRITERIA}\n" \
    "   &ENDIF \n" \
    "&ELSE \n" \
    "   getTermLabel(\"SELECTION_CRITERIA\", 25).\n" \
    "&ENDIF"
#endif

/*  Basic types of Characters */
typedef enum
{
    NAMECHAR, LETTER, DIGIT, QUOTE, SPECIAL, EOF_CODE
} CHAR_CODE;

/*  Basic types of tokens */
typedef enum
{
    ENDFILE,
    COMMENT,
    LITERAL,
    WORD,
    KEYWORD,
    NUMBER,
    OPERATOR,
    INCLUDE,
    PREPROCESSOR
} TOK_TYPE;

/* Sections of a standard Mfg/Pro Report:
   Preloop - before the outside REPEAT loop
   UIsetup- after the REPEAT & before the update/set statement
   GeneralVals - after UPDATE/SET statement, before batching: 'bcdparm = "".'
   FieldVals - between batching & MFSELxxx, (field specific validations)
   ReportPrt - inside report producing code
*/
typedef enum
{
    Preloop, UIsetup, GeneralVals, FieldVals, ReportPrt
} RPT_SECTIONS;

struct
{
    int indlev; /* indent level of this block */
    int erract; /* it's on ERROR handling */
    int errevt; /* whether or not an ERROR generating event can happen within */
} blck[MAX_BLOCKS];

/* Converter mode descriptions */
#define CONVERT_MODE_MAINTENANCE 0
#define CONVERT_MODE_REPORT 1
#define CONVERT_MODE_FULLGUI_REPORT 2
#define CONVERT_MODE_NO_CONVERT_NO_OUTPUT 3
#define CONVERT_MODE_NO_CONVERT 4
#define CONVERT_MODE_CONDITIONAL 5
#define CONVERT_MODE_REPORT_AND_MAINT 6

static char *converterModeString[] = {
   "Maintenance",
   "Report",
   "FullGUIReport",
   "NoConvertNoOutput",
   "NoConvert",
   "ConditionalIncludeExclude",
   "ReportAndMaintenance",
   ""
};

/* Converter Directives */
#define DIRECTIVE_HIDDEN_DOWN_FRAME 0
#define DIRECTIVE_RPMAIN_CALLER 1
#define DIRECTIVE_CONVERT_MODE 2
#define DIRECTIVE_EDITABLE_DOWN_FRAME 3
#define DIRECTIVE_DONT_REFRESH_TITLE 4
#define DIRECTIVE_NON_STREAM_IO_FRAME 5
#define DIRECTIVE_CONCAT_STRING 6

static char *converterDirectiveString[] = {
   "HiddenDownFrame=",
   "RPMainCaller=",
   "ConvertMode=",
   "EditableDownFrame=",
   "DontRefreshTitle=",
   "NonStreamIoFrame=",
   "ConcatString",
   ""
};


/*------------------------------------------------------------------------------
** Forward declarations
**----------------------------------------------------------------------------*/
void writeFormStmt();
void getSpecial();
void convertFile();
int checkBlock();
void checkForTrigger();
int frameNameFoundInList();
void getNumber();
int getPreprocessor();
int getString();
int getToken();
void getWord();
void guiDE();
void guiDupThings();
void guiPreBuffer();
void guiPreFind();
void guiRpchk();
void guiRpexit();
void guiUI();
void guiWriteTriggers();
void initFile();
void initScanner();
int isKeyword();
int isNextToken();
int is_gpquote();
int openOutFile();
int openSourceFile();
void outOfHere();
int parseConverterDirective();
int parseDE();
int parseGpseloutParams();
void putToken();
int setErrEvent();
void setFrameScope();
void skipBlock();
int skipBraces();
void skipStatement();
void usage();


/*------------------------------------------------------------------------------
** Globals
**----------------------------------------------------------------------------*/
char *skipComment();
char convVersion[]="1.78";

int
   prettyMode=0,            /* Conversion mode flags */
   declMode=0,
   fldListMode=0,
   rParseMode=0,
   mParseMode=0,
   guiRptMode=0,
   concatStringMode = 0,
   miscMode = 0,
   stringsToConcat = 0,
   stringsConcated = 0,
   StartedPReport = 0,
   convertMode = NOT_FOUND,
   doingPrepass = 1,
   plevel,                  /* paren level counter */
   lineNbr,                 /* current line number */
   srcType,                 /* 0 => .p-Program file; 1 => all others */
   indLevel,                /* Indent/block Level */
   iBlock,                  /* Sequential Block numbering */
   errorAction,             /* Error handling action of a block */
   contStatement,           /* Boo-coo flags add naseum ... */
   suppressRepeat,          /* Suppressing output of particular code */
   suppressEnd,             /* Suppressing "if not batchrun then do:"'s END. */
   firstRepeat,             /* Outer repeat of a rept/inq */
   lineFed,                 /* NewLine outputed flag */
   veryTop,                 /* Very top of input file flag */
   verbose,                 /* Tell about every transformation done */
   inForm,                  /* SET when within a FORM statement */
   isBatchable,             /* Batchable report program (with mfselbpr.i) */
   touched,                 /* How many transformation done on this file */
   guiSubDone,              /* The mftrail/mfreset Full GUI subst done */
   gotDisp,                 /* Got one of these statements .... */
   gotDown,
   gotWith,
   gotBcdparm,
   hasGpselout,
   hasMfsel,
   hasMfdtit,
   hasMfddec,
   newBlock,                /* Set at a DO/REPEAT/FOR EACH/EDITING block beg */
   noColLabels,             /* Track op elements/attributes of frame defs */
   noOverlay,
   scrollFrame,
   rectInFrame,
   titleInFrame,
   frameWidth,
   needTitleDisplay,        /* Refresh FRAME-LABEL title after CLEAR FRAME */
   nGlobBuf,                /* Track DEF BUFFERs & FINDS moved in GUI-rpts */
   nGlobFind,
   nFieldTrigs,             /* If-blocks become field triggers in GUI-rpts */
   outputOn,                /* Turns On/Off output from the TOKENIZER engine */
   TwoMany,
   jmp_bufWasSet = 0,       /* Variable telling if we can longjmp yet. */
   okToOverwriteTarget = 1, /* Should conversion abort on overwrite? */
   fileCount=0;             /* How many file conversions attempted */

FILE
    *outfile = NULL,   /* ouput stream */
    *logfile = NULL;   /* message/warning stream */

char
   twinkler[]="|/-\\",                 /* Twinkler to show progress */
   srcSearchPath[MAX_TOK_LEN],         /* These are the directories */
   workDir[SHORT_STRING_LENGTH],       /*   specified in the */
   targetDir[SHORT_STRING_LENGTH],     /*   convert.ini file */
   tokenString[MAX_TOK_LEN],           /* hold a copy of the current token */
   prevToken[MAX_TOK_LEN],             /* hold a copy of the previous token */
   selprtArg[MAX_TOK_LEN],             /* hold args of {mfselprt.i ...  } */
   gpprtrpaParams[MAX_TOK_LEN],        /* hold args for gpprtrpa.i */
   mfguirpaParams[MAX_TOK_LEN],        /* hold args for mfguirpa.i */
   UIlist[2048],                       /* hold fields in update/set stmt */
   argBuf[ARGBUF_LEN],
   *iniFileName = NULL,                /* initialization file name */
   *currentPointer, *srcBuf,           /* source buffer ptrs */
   srcPath[MAX_TOK_LEN], *srcName,     /* input src file name, full & short */
   logFileName[SHORT_STRING_LENGTH]=DEFAULT_STATUSFILE, /* default logfile */
   *mfd, *mfd0,                        /* Mark the mfdeclre/mfdtitle line */
   *codeStart,                         /* First non-comment line of code */
   outfilePath[SHORT_STRING_LENGTH],   /* path name of output file */
   *outfileName,                       /* file name portion of output file */
   titText[SHORT_STRING_LENGTH],       /* frame title text capture of filter */
   *tokBeg, *tokEnd,                   /* mark off the token in input buffer */
   incName[24],                        /* include file name */
   withName[24],                       /* frame name of last with clause */
   dontRefreshTitles[200],             /* supress this frame's title refresh */
   nonStreamIoFrames[200],             /* don't treat as a stream-io frame */
   frameName[24],                      /* frame name within a statement */
   frameScope[20][64],                 /* frames scoped for blocks */
   editableDownFrames[200],            /* scrolling down frames to get 3-D'd */
   downFrames[200],                    /* down frames clearable w/o title */
                                       /* refresh (comma delim list of names */
                                       /* with leading & trailing commas */
   RPMainCaller[FILENAME_MAX],         /* name of File continaing a hidden */
                                       /* RPMain call */
   *globBuf[10],                       /* point back to all DEFINE BUFFERs */
   *globFind[10],                      /* point back to all global FINDs */
   *fieldTrigs[10][2],                 /* point back to If-blocks becoming triggers */
   fieldTrigNames[10][64];             /* names of fields with validation triggers */


/*------------------------------------------------------------------------------
** Declarations for inserted code strings
**----------------------------------------------------------------------------*/

#define CONDITIONAL_INCLUDE_REPORT_NOT_A_TEST "\n" \
    "&IF ((\"{&PP_GUI_CONVERT_MODE}\" = \"REPORT\") AND\n" \
    "(\"{&PP_FRAME_NAME}\" <> \"A\")) &THEN\n"

#define CONDITIONAL_INCLUDE_MAINT_TEST "\n" \
    "&IF (\"{&PP_GUI_CONVERT_MODE}\" <> \"REPORT\") OR\n" \
    "((\"{&PP_GUI_CONVERT_MODE}\" = \"REPORT\") AND\n" \
    "(\"{&PP_FRAME_NAME}\" = \"A\")) &THEN\n"

#define CONDITIONAL_INCLUDE_REPORT_TEST "\n" \
    "&IF (\"{&PP_GUI_CONVERT_MODE}\" = \"REPORT\") &THEN\n"

#define CONDITIONAL_END \
    "&ENDIF /*GUI*/\n"

#define MAINT_RIGHT_MARGIN_ADJ \
    " SKIP(.4)  /*GUI*/\n"

char maintRightMarginAdj[] =
    MAINT_RIGHT_MARGIN_ADJ;

char condMaintRightMarginAdj[] = "\n"
    CONDITIONAL_INCLUDE_MAINT_TEST
    MAINT_RIGHT_MARGIN_ADJ
    CONDITIONAL_END;

#define STREAM_IO \
    " STREAM-IO /*GUI*/ "

char streamIO[] =
    STREAM_IO;

char condStreamIO[] =
    CONDITIONAL_INCLUDE_REPORT_TEST
    STREAM_IO "\n"
    CONDITIONAL_END;

#define WITH_STREAM_IO \
    " WITH STREAM-IO /*GUI*/ "

char withStreamIO[] =
    WITH_STREAM_IO;

char condWithStreamIO[] = "\n"
    CONDITIONAL_INCLUDE_REPORT_TEST
    WITH_STREAM_IO "\n"
    CONDITIONAL_END;

#define WIDTH_80 \
    " WIDTH 80 /*GUI*/"

char width80[] =
    WIDTH_80;

char condWidth80[] = "\n"
    CONDITIONAL_INCLUDE_REPORT_TEST
    WIDTH_80 "\n"
    CONDITIONAL_END;

#define THREE_D \
    " THREE-D /*GUI*/"

char threeD[] =
     THREE_D ".\n";

char condThreeD[] =
    CONDITIONAL_INCLUDE_MAINT_TEST
    THREE_D "\n"
    CONDITIONAL_END
    ".\n";

#define NO_BOX_3D \
    " NO-BOX" THREE_D

char noBox3D[] =
     NO_BOX_3D ".\n";

char condNoBox3D[] =
    CONDITIONAL_INCLUDE_MAINT_TEST
    NO_BOX_3D "\n"
    CONDITIONAL_END
    ".\n";

#define BGCOLOR_8 \
    " BGCOLOR 8 /*GUI*/"

char bgColor8[] =
     BGCOLOR_8 ".\n";

char condBGColor8[] =
    CONDITIONAL_INCLUDE_MAINT_TEST
    BGCOLOR_8 "\n"
    CONDITIONAL_END
    ".\n";

char guiRptInc0[] = "\n"
    "/*GUI global preprocessor directive settings */\n"
    "&GLOBAL-DEFINE PP_PGM_RP TRUE\n"
    "&GLOBAL-DEFINE PP_ENV_GUI TRUE\n"
    "\n";

char guiRptInc0a[] = "\n"
    "/*GUI preprocessor directive settings */\n"
    "&SCOPED-DEFINE PP_GUI_CONVERT_MODE REPORT\n"
    "\n";

char guiFrameADefine[] = "\n"
    "/*GUI preprocessor Frame A define */\n"
    "&SCOPED-DEFINE PP_FRAME_NAME A\n"
    "\n";

char guiFrameAUndefine[] = "\n"
    "/*GUI preprocessor Frame A undefine */\n"
    "&UNDEFINE PP_FRAME_NAME\n"
    "\n";

char guiRptInc1[] = "\n"
    "/*GUI*/ {mfguirpa.i %s }\n"
    "\n";

char guiRptInc2[] = "\n"
    "run p-action-fields (input \"display\").\n"
    "run p-action-fields (input \"enable\").\n"
    "end procedure. /* p-enable-ui, replacement of Data-Entry GUI*/\n";

char guiRptInc3[] = "\n"
    "/*GUI*/ procedure p-report-quote:\n";

char guiRptInc4[] = "\n"
    "/*GUI*/ end procedure. /* p-report-quote */\n";

char guiRptInc5[] = "\n"
    "/*GUI*/ procedure p-report:\n"
    "/*GUI*/   {gpprtrpa.i %s}\n"
    "/*GUI*/   mainloop: do on error undo, return error on endkey undo, return error:\n";

char guiRptInc6[] = "\n"
    "/*GUI*/ {mfguirex.i %s /*Replace mfrpexit*/\n";

char guiRptInc7[] = "\n"
    "/*GUI*/ {mfguichk.i %s /*Replace mfrpchk*/\n";

char guiRptInc8[] = "\n"
    "/*GUI*/ {mfguitrl.i} /*Replace mfrtrail*/\n";

char guiRptInc9[] = "\n"
    "/*GUI*/ {mfgrptrm.i} /*Report-to-Window*/\n";

char guiRptInc10[] = "\n"
    "/*GUI*/ end procedure. /*p-report*/\n"
    "/*GUI*/ {mfguirpb.i &flds=\"%s\"} /*Drive the Report*/\n";

#define TIT_RECT_FRAME \
    " RECT-FRAME       AT ROW 1.4 COLUMN 1.25\n" \
    " RECT-FRAME-LABEL AT ROW 1   COLUMN 3 NO-LABEL\n" \
    " SKIP(.1)  /*GUI*/\n"

char titRectFrame[]  = "\n"
    TIT_RECT_FRAME;

char condTitRectFrame[]  = "\n"
    CONDITIONAL_INCLUDE_MAINT_TEST
    TIT_RECT_FRAME
    CONDITIONAL_END;

#define NO_TIT_RECT_FRAME \
    " RECT-FRAME       AT ROW 1 COLUMN 1.25\n" \
    " RECT-FRAME-LABEL AT ROW 1 COLUMN 3 NO-LABEL VIEW-AS TEXT SIZE-PIXELS 1 BY 1\n" \
    " SKIP(.1)  /*GUI*/\n"

char notitRectFrame[] = "\n"
    NO_TIT_RECT_FRAME;

char condNoTitRectFrame[] = "\n"
    CONDITIONAL_INCLUDE_MAINT_TEST
    NO_TIT_RECT_FRAME
    CONDITIONAL_END;

#define TITLE_RECT_ADJ \
    " DEFINE VARIABLE F-%s-title AS CHARACTER.\n" \
    " F-%s-title = %s.\n" \
    " RECT-FRAME-LABEL:SCREEN-VALUE in frame %s = F-%s-title.\n" \
    " RECT-FRAME-LABEL:WIDTH-PIXELS in frame %s =\n" \
    "  FONT-TABLE:GET-TEXT-WIDTH-PIXELS(\n" \
    "  RECT-FRAME-LABEL:SCREEN-VALUE in frame %s + \" \", RECT-FRAME-LABEL:FONT).\n" \
    " RECT-FRAME:HEIGHT-PIXELS in frame %s =\n" \
    "  FRAME %s:HEIGHT-PIXELS - RECT-FRAME:Y in frame %s - 2.\n" \
    " RECT-FRAME:WIDTH-CHARS IN FRAME %s = FRAME %s:WIDTH-CHARS - .5. /*GUI*/\n"

char titRectAdj[] = "\n"
    TITLE_RECT_ADJ;

char condTitRectAdj[] = "\n"
    CONDITIONAL_INCLUDE_MAINT_TEST
    TITLE_RECT_ADJ
    CONDITIONAL_END;

#define NO_TIT_RECT_ADJ \
    " DEFINE VARIABLE F-%s-title AS CHARACTER INITIAL \"\".\n" \
    " RECT-FRAME-LABEL:SCREEN-VALUE in frame %s = F-%s-title.\n" \
    " RECT-FRAME-LABEL:HIDDEN in frame %s = yes.\n" \
    " RECT-FRAME:HEIGHT-PIXELS in frame %s =\n" \
    "  FRAME %s:HEIGHT-PIXELS - RECT-FRAME:Y in frame %s - 2.\n" \
    " RECT-FRAME:WIDTH-CHARS IN FRAME %s = FRAME %s:WIDTH-CHARS - .5.  /*GUI*/\n"

char notitRectAdj[] ="\n"
    NO_TIT_RECT_ADJ;

char condNoTitRectAdj[] ="\n"
    CONDITIONAL_INCLUDE_MAINT_TEST
    NO_TIT_RECT_ADJ
    CONDITIONAL_END;

char titRefresh[] = ".\n"
    "/*GUI*/ RECT-FRAME-LABEL:SCREEN-VALUE in frame %s = F-%s-title";

#define BEAM_ME_UP "\n" \
    "/*GUI*/ if global-beam-me-up then undo, leave.\n"

char beamMeUp[] =
    BEAM_ME_UP;

char condBeamMeUp[] =
    CONDITIONAL_INCLUDE_MAINT_TEST
    BEAM_ME_UP
    CONDITIONAL_END;

/* Replace "view frame dtitle" with this */
char toolBar[] = "/*GUI: view frame dtitle. */\n"
    "IF global-tool-bar AND global-tool-bar-handle ne ? THEN\n"
    "  view global-tool-bar-handle. /*GUI*/\n";

/* Put this just after mfselxxx, skip .4 done in the include */
char prtInject[] = "\n"
    "/*GUI*/ RECT-FRAME:HEIGHT-PIXELS in frame a = FRAME a:HEIGHT-PIXELS - RECT-FRAME:Y in frame a - 2.\n";

CHAR_CODE char_table[256];
TOK_TYPE tokType;
RPT_SECTIONS Section;


/*------------------------------------------------------------------------------
** MACROS
**----------------------------------------------------------------------------*/
#if _DOS
#if __BORLANDC__ || __SC__
#define strncasecmp(s1,s2,n) strnicmp(s1,s2,n)
#define strcasecmp(s1,s2) stricmp(s1,s2)
#endif  /*__BORLANDC__ || __SC__*/
#if __MSVC__
#define strncasecmp(s1,s2,n) strnicmp(s1,s2,n)
#define strcasecmp(s1,s2) stricmp(s1,s2)
#define stat _stat
#define read _read
#define open _open
#define close _close
#define unlink _unlink
#endif /*__MSVC__*/
#endif /*_DOS*/
#define charCode(ch)      char_table[ch]
#define isPrevToken(str)  !(strcasecmp(str, prevToken) == 0)
#define isToken(str)      !strcasecmp(str, tokenString)
#define isNotToken(str)   strcasecmp(str, tokenString)
#define isnToken(str,num) !strncasecmp(str, tokenString, num)

jmp_buf errRestart; /* Error Trapping Mark for setjmp()/longjmp() */


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void parseCommandLine(argc, argv, listName, firstFilenameIndex, wantTwinkler)
int argc;
char *argv[];
char **listName;         /* List of source files to process */
int *firstFilenameIndex; /* index of 1st cmd-line arg assumed to be a file */
int *wantTwinkler;       /* Enable display of "work-in-progress" icon? */
{
    int index;
    static char lnbuf[SHORT_STRING_LENGTH];

    /* Default is no file list used. */
    *listName = NULL;

    /* Default is to display twinkle. */
    *wantTwinkler = 1;

    /* Parse the command line */
    if (argc < 2 )
    {
        usage(argv[0]);
    }

    for (index = 1; index < argc; ++index)
    {
        if ('-' == argv[index][0])
        {
            /* Parse a non-filename command-line option. */

            if (strcmp("-?", argv[index]) == 0)
            {
                /* Display command line syntax and exit. */
                usage(argv[0]);
            }
            else if (strcasecmp("-v", argv[index]) == 0)
            {
                /* Enable the verbose output option. */
                verbose = 1;
            }
            else if (strcmp("-L", argv[index]) == 0)
            {
                /* List of files whose type will be determined by tokens. */
                *listName = DEFAULT_LISTFILE;
                strcpy(logFileName, DEFAULT_STATUSFILE);
                miscMode = 1;
            }
            else if (strcmp("-M", argv[index]) == 0)
            {
                /* List of maintenance, Processing/Data-entry programs */
                *listName = DEFAULT_MAINTENENCE_LISTFILE;
                strcpy(logFileName, DEFAULT_MAINTENENCE_STATUSFILE);
                mParseMode = 1;
            }
            else if (strcmp("-m", argv[index]) == 0)
            {
                /* Single maintenance, Processing/Data-entry programs */
                mParseMode = 1;
            }
            else if (strcmp("-R", argv[index]) == 0)
            {
                /* List of reports, Printing Programs */
                *listName = DEFAULT_REPORT_LISTFILE;
                strcpy(logFileName, DEFAULT_REPORT_STATUSFILE);
                rParseMode = 1;
            }
            else if (strcmp("-r", argv[index]) == 0)
            {
                /* Single report, Printing Program */
                rParseMode = 1;
            }
            else if (strcmp("-G", argv[index]) == 0)
            {
                /* List of full-gui report main procedures */
                *listName = DEFAULT_FULLGUI_REPORT_LISTFILE;
                strcpy(logFileName, DEFAULT_FULLGUI_REPORT_STATUSFILE);
                guiRptMode = 1;
                rParseMode = 1;
            }
            else if (strcmp("-g", argv[index]) == 0)
            {
                /* Single full-gui report main procedures */
                guiRptMode = 1;
                rParseMode = 1;
            }
            else if (strcmp("-P", argv[index]) == 0)
            {
                /* List of source files to pretty-print */
                *listName = DEFAULT_LISTFILE;
                prettyMode = 1;
            }
            else if (strcmp("-p", argv[index]) == 0)
            {
                /* Single source file to pretty-print */
                prettyMode = 1;
            }
            else if (strcmp("-F", argv[index]) == 0)
            {
                /* Files from which to get field list for field help checking */
                *listName = DEFAULT_LISTFILE;
                fldListMode = 1;
            }
            else if (strcmp("-f", argv[index]) == 0)
            {
                /* File from which to get field list for field help checking */
                fldListMode = 1;
            }
            else if (strcasecmp("-notwinkle", argv[index]) == 0)
            {
                /* Disable "progressing through listFile" icon display */
                *wantTwinkler = 0;
            }
            else if (strncasecmp("-ini=", argv[index], strlen("-ini=")) == 0)
            {
                /* Set the ini file name from the command line. */
                iniFileName = strstr(argv[index], "=");
                ++iniFileName;
            }
            else
            {
                /* Unrecognized comand line switch. */
                printf("Unrecognized command-line switch, \"%s\".\n",
                    argv[index]);
                /* Display command line syntax and exit. */
                usage(argv[0]);
            }
        }
        else
        {
            /* If argument doesn't start with -, assume it is a filename.
            ** If listName has been set to the default, it needs to be changed
            ** to the new listName from the command line.
            */
            if (*listName != NULL)
            {
                strcpy(lnbuf, argv[index]);
                *listName = lnbuf;
            }
        }
    } /* end of for() */

    /* If filenames are not being read from a listFile, find the
    ** first command line arg assummed to be a filename.
    */
    *firstFilenameIndex = 1;
    while ((*firstFilenameIndex < argc) &&
        ('-' == argv[*firstFilenameIndex][0]))
    {
        ++(*firstFilenameIndex);
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
int okToOverwrite(target)
char *target;
{
    FILE *temp = NULL;
    int okToStomp = 0;
    static char errorMessage[SHORT_STRING_LENGTH];

    if (0 == okToOverwriteTarget)
    {
        /* Check file existence */
        temp = fopen(target, "r");
        if (NULL == temp)
        {
            okToStomp = 1;
            fclose(temp);
        }
    }
    else
    {
        okToStomp = 1;
    }

    if (0 == okToStomp)
    {
        sprintf(errorMessage, "Cannot overwrite existing %s.\n", target);
        outOfHere(errorMessage);
    }

    return (okToStomp);
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
main(argc, argv)
int argc;
char *argv[];
{
    int  curArg;            /* cmd-line argument being processed */
    char *listName;         /* List of source files to process */
    FILE *listStream;
    int wantTwinkler = 1;
    int openResult = 0;
    logfile = stdout;        /* message/warning stream */

    /* Parse the command line */
    parseCommandLine(argc, argv, &listName, &curArg, &wantTwinkler);

    initScanner();

    if (listName)
    {
        /* Build list-file name in Work Dir variable, then open it. */
        strcpy(srcPath, workDir);
        strcat(srcPath,listName);
        listStream = fopen(srcPath, "r");
        if (listStream == NULL)
        {
            printf("Cannot open input Listfile: %s\n", srcPath);
            exit(1);
        }
        /* Open logfile as disk file*/
        strcpy(srcPath, workDir);
        strcat(srcPath,logFileName);
        if (okToOverwrite(srcPath))
        {
            if ((logfile=fopen(srcPath, "w")) == NULL)
            {
                printf("Cannot write %s %i\n", srcPath,  errno);
                exit(1);
            }
        }
    }
    else
    {
        logfile = stdout;
    }

    /* Field list mode has one overall output file */
    if (fldListMode)
    {
        strcpy(outfilePath, workDir);
        strcat(outfilePath, DEFAULT_STATUSFILE);

        if (okToOverwrite(outfilePath))
        {
            if ((outfile=fopen(outfilePath, "w")) == NULL)
            {
                printf("Cannot write %s %i\n", outfilePath, errno);
                exit(1);
            }
        }
    }

    if (listName && wantTwinkler)
    {
        fputs("Converting         ", stdout);
    }

    while ((curArg < argc) || listName)
    {
        if (listName && wantTwinkler)
        {
            printf("\b\b\b\b\b\b\b\b%c %6d", twinkler[fileCount % 4], fileCount);
            fflush(stdout);
            ++fileCount;
        }

        if (listName) /* Get next filename from list */
        {
            if (fgets(srcPath, SHORT_STRING_LENGTH, listStream) == NULL)
            {
                break; /* END of Listfile */
            }
            /* Check for CR-LF */
            if (10 == srcPath[0])
            {
                /* Reading a DOS file in UNIX, read again. */
                if (fgets(srcPath, SHORT_STRING_LENGTH, listStream) == NULL)
                {
                    break; /* END of Listfile */
                }
            }
            srcPath[strlen(srcPath) - 1] = '\0'; /* Axe the NEWLINE */
        }
        else
        {
            /* Get next filename from command-line */
            strcpy(srcPath,argv[curArg]);
            ++curArg;
        }

        /* Sets srcPath & srcName */
        convertMode = NOT_FOUND;
        openResult = openSourceFile();

        if (1 == openResult)
        {
            continue;
        }
        else if (ENCRYPTION_CHECK_BYTE_COUNT == openResult)
        {
            /* Found an encrypted file.
            ** Treat as ConvertMode=NoConvert.
            */
            convertMode = CONVERT_MODE_NO_CONVERT;
        }

        if (!prettyMode && !fldListMode)
        {
            outfileName = srcName; /* Sets default outfileName */
        }

        if (! fldListMode)
        {
            if (openOutFile(targetDir)) /* in output destination directory */
                continue;
        }

        /* Run convert in error trapping harness */
        if (setjmp(errRestart) == 0)
        {
            /*  don't hit after longjmp() */
            jmp_bufWasSet = 1;
            convertFile(convertMode);
        }
    }

    if (listName && wantTwinkler)
    {
        putchar('\n');
    }

    exit(0);
} /*end main()*/


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void usage(progName)
char *progName;
{
    printf(" %s v%s, Copyright 1994-2004 QAD Inc., Carpinteria, CA, USA.\n"
        "All rights reserved worldwide. This is an unpublished work.\n"
        "Usage:\n"
        " %s [-v] [-notwinkle] [-ini=iniFile] Filename [Filename...]\n"
        " %s [-v] [-notwinkle] [-ini=iniFile] -L [Listfile]\n",
        progName, convVersion, progName, progName);
    exit(1);
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void convertFile(defaultConvertMode)
int defaultConvertMode;
{
    time_t timer;
    int lineNbrSave,
        i, j;     /* generic loop counters */
    int thisIsFrameA = 0;
    char *anchor, /* an anchor for lookaheads */
         *s1;     /* generic char pointer */
    char systemCommand[SHORT_STRING_LENGTH];


    /* PREPASS through file to get a lay of the land. ---------------------
      Check the position of the mfdtitle/mfdeclre so it can be moved if needed.
      Track what type of END-ERROR handling is done for each block level,
      using bits in endact flag: 00 = ignored | 01 = Retry | 10  = Leave (2)
      Since there can be more than one nested set of blocks within a file,
      and since nested levels can grow and shrink; keep track of all blocks
      not by their level, but by their sequential order within the file.
    */

    initFile(defaultConvertMode);

    if (!prettyMode && !fldListMode)
    {
        doingPrepass = 1;

        /* Check to see if we must move up mfdeclre/mfdtitle's, also check that
         * Gui Report Conversion files have both mfdtitle and mfselXXX.
         */
        outputOn = 0;
        i = 0;
        j = 0;
        /* Get first piece of non-comment code & find line beginning */
        tokType = COMMENT;
        while ((tokType == COMMENT) &&
            (convertMode != CONVERT_MODE_NO_CONVERT) &&
            (convertMode != CONVERT_MODE_NO_CONVERT_NO_OUTPUT))
        {
            getToken();
        }

        if ((convertMode != CONVERT_MODE_NO_CONVERT) &&
            (convertMode != CONVERT_MODE_NO_CONVERT_NO_OUTPUT))
        {
            for (codeStart = currentPointer - 1;
               *codeStart && *codeStart != '\n';
               codeStart--)
            {
                ;
            }
            hasMfdtit = isToken("mfdtitle.i");
            hasMfddec = isToken("mfdeclre.i");
            if (hasMfddec ||  hasMfdtit)
            {
                getToken(); /* Now Skip it! */
            }

            do
            {
                if ( tokType == INCLUDE )
                {
                    /* Find where the mfdeclre/title is now. */
                    /* The actual move work will be done in getToken() */
                    i = isToken("mfdtitle.i");
                    j = isToken("mfdeclre.i");
                    if (i || j)
                    {
                        /* Find line beginning */
                        for (s1 = currentPointer - 1; *s1 && *s1 != '\n'; s1--)
                        {
                           ;
                        }
                        /* mfdeclre/mfdtitle went from mfd -> mfd0. */
                        mfd = s1;
                        mfd0 = codeStart;
                        if (i) hasMfdtit = 1;
                        if (j) hasMfddec = 1;
                    }
                    else if (isToken(MFSELBPR_I) ||
		       isToken(MFSELPRT_I) ||
                       isToken(GPSELOUT_I))
                    {
                        /* Get mfselXXX args for GUI-reports */
                        if (guiRptMode && indLevel != 1)
                        {
                            outOfHere("MFSEL/GPSELOUT too deep:");
                        }

                        for (s1=tokBeg+1; *s1!='i'; s1++) ;
                        s1++;

                        strncpy(selprtArg, s1, (currentPointer - s1 - 1) );
                        selprtArg[ (currentPointer - s1 - 1) ] = '\0';

                        hasMfsel = 1;
                        isBatchable = isToken(MFSELBPR_I);
                        if (isToken(GPSELOUT_I))
                        {
                           hasGpselout = 1;
                           parseGpseloutParams(selprtArg,
                              &isBatchable,
                              gpprtrpaParams,
                              mfguirpaParams);
                        }
                        else
                        {
                           strcpy(gpprtrpaParams, selprtArg);
                           sprintf(mfguirpaParams, "%s %s",
                              isBatchable ? "true" : "false",
                              selprtArg);
                        }
                    }
                    if (!(newBlock || *tokenString == '&' || isdigit(*tokenString)))
                        setErrEvent(indLevel); /* Error producing event */

                        /* don't consider {arg} within do ~~~: as in include files */
                }
                /* block levels ----------------------------*/
                else if (isToken("REPEAT") ||
                    (isToken("FOR") && isNextToken("EACH")))
                {
                    iBlock++;
                    indLevel++;
                    newBlock = 1;
                    /* default error action is undo,retry */
                    errorAction = 2;
                    if (iBlock >= MAX_BLOCKS)
                    {
                       outOfHere("MAX_BLOCKS Exceeded:");
                    }
                }
                else if (isToken("EDITING") || isToken("DO"))
                {
                    iBlock++;
                    indLevel++;
                    newBlock = 1;
                    errorAction = 0; /* default error action is ignore */
                    if (iBlock == MAX_BLOCKS) outOfHere("MAX_BLOCKS Exceeded:");
                }
                else if (isToken("END"))
                {
                    if (--indLevel < 0)
                    {
/*--------------------------------
                        fprintf(logfile,
                            "%s %d Indent LEVEL mismatch\n",srcName,lineNbr);
--------------------------------*/
                        indLevel = 0;
                    }
                }
                else if (isnToken("TRANS",5))
                {
                    errorAction = 2; /* default is now Retry */
                }
                else if (isToken("ON") && isNextToken("ERROR")) /*jzs 11/23/94*/
                { /* Check ERROR action for the block */
                    getToken();
                    for (s1=currentPointer;
                       *s1 != ',' && *s1 != '.' && *s1 != ':';
                       s1++)
                    {
                       ;
                    }
                    if (*s1 == ',')
                    {
                        do s1++;
                        while (isspace(*s1)); /* NO COMMENTS! */
                        if (strncasecmp(s1, "NEXT", 4 ) == 0)
                        { /* Examine these, much like Retry's */
                            errorAction = 2;
                            fprintf(logfile,"%s %d ERROR-NEXT\n",srcName,lineNbr);
                        }
                        else if (strncasecmp(s1, "RETRY", 5 ) == 0)
                        {
                            errorAction = 2;
                        }
                        else if (strncasecmp(s1, "RETURN", 6 ) == 0)
                        {
                            /* like Ignore */
                            errorAction = 0;
                        }
                        else if (strncasecmp(s1, "LEAVE", 5 ) == 0)
                        {
                            errorAction = 1;
                        }
                    }
                    else
                    {
                        errorAction = 2; /* default is Retry */
                    }
                }
                else if (isToken("SET") || isToken("UPDATE") || isToken("PROMPT-FOR"))
                {
                    /* Error producing event */
                    setErrEvent(indLevel);
		}
                else if ((isToken(".") || isToken(":")) && isspace(*currentPointer) && newBlock)
                {
                    /* Avoid periods within compound table.field names */
                    blck[iBlock].indlev = indLevel;
                    blck[iBlock].erract = errorAction;
                    blck[iBlock].errevt = 0;
                    newBlock = 0;
                }
                getToken();
            } while ((tokType != ENDFILE) &&
                (convertMode != CONVERT_MODE_NO_CONVERT_NO_OUTPUT) &&
                (convertMode != CONVERT_MODE_NO_CONVERT));
        }

        /* Reset things for real pass through the file */
        doingPrepass = 0;
        indLevel = iBlock = 0;
        outputOn = 1;
        currentPointer = srcBuf;
        lineNbr = 1;
        if (guiRptMode)
        {
            /* Should be a top level program with a {mfdtitle.i} */
            if (!hasMfdtit)
            {
                fprintf(logfile,"%s WARN1 No MFDTITLE\n",srcName);
            }
            if (!hasMfsel)
            {
                fprintf(logfile,"%s WARN1 No GPSELOUT, MFSELPRT, or MFSELBPR\n",srcName);
            }
        }

        switch (convertMode)
        {
        case CONVERT_MODE_MAINTENANCE:
            mParseMode = 1;
            break;

        case CONVERT_MODE_REPORT:
            rParseMode = 1;
            break;

        case CONVERT_MODE_FULLGUI_REPORT:
            guiRptMode = 1;
            rParseMode = 1;
            break;

        case CONVERT_MODE_NO_CONVERT_NO_OUTPUT:
            /* Signal EOF to avoid having the file parsed and converted. */
            currentPointer = NULL;
            declMode = 1;
            fclose(outfile);
            break;

        case CONVERT_MODE_NO_CONVERT:
            /* Copy the file straight thru without changing anything. */
            fclose(outfile);
            sprintf(systemCommand, "%s %s %s", SYSTEM_COPY_CMD, srcPath, outfilePath);
            system(systemCommand);

            /* Signal EOF to avoid having the file parsed and converted. */
            currentPointer = NULL;
            declMode = 1;
            break;

        case CONVERT_MODE_CONDITIONAL:
            /* Move mfdeclare/mftitle to top of file,
            ** process conditional includes and excludes.
            */
            declMode = 1;
            break;

        case CONVERT_MODE_REPORT_AND_MAINT:
            /* Needs to serve as both Report and Maint based on preprocessor
            ** directives set at compile time.
            */
            rParseMode = 1;
            mParseMode = 1;
            break;

        default:
            fprintf(logfile,
                "%s WARN1 \"%s\" NOT FOUND.\n",
                srcName,
                converterDirectiveString[DIRECTIVE_CONVERT_MODE]);
        }

        /* Check to see that a convert mode has been specified. */
        if ((0 == mParseMode) && (0 == rParseMode) && (0 == declMode))
        {
            outOfHere("No convert mode specified:");
        }
    }

    /* REAL PASS through file to do our work. ------------------------------*/
    Section = Preloop; /* Report section, for guiRpt */
    while ((currentPointer != NULL) && (*currentPointer != '\0'))
    {
        getToken();

        if ((mParseMode || rParseMode || declMode) && (veryTop))
        {
            /* Time-stamp the top of file */
            veryTop--;
            time(&timer);
            strcpy(prevToken, ctime(&timer));
            *(prevToken+strlen(prevToken) - 1) = '\0';
            fprintf(outfile,
                "/* GUI CONVERTED from %s (converter v%s) %s */\n",
                srcName, convVersion, prevToken);
        }

        if (declMode) /*** Just move up mfdtitle/mfdeclre includes ***/
        {
            putToken(); /* move work is done in get/put token routines */
        }
        else if (fldListMode) /*** Create list of fields updated on screen ***/
        {
            if (tokType == KEYWORD &&
                (isToken("SET") || isToken("UPDATE") || isToken("PROMPT-FOR")))
            {
                parseDE();
            }
        }
        else if (prettyMode) /*** Clean up ugly code, removing all comments ***/
        {
            if (isToken("WITH"))
            {
                inForm = 0;
            }
            else if (isToken("END"))
            {
                indLevel--;
            }

            if (tokType != COMMENT || lineNbr < 4 || !strncmp("/*!",tokBeg,3))
            {
                if (lineFed) /* Starting a new line, indent it */
                {
                    for (i=0; i<indLevel; i++) fputs("   ",outfile);
                    if (inForm)
                    {
                        fputs("   ",outfile);
                    }
                    else if (contStatement)
                    {
                        putc(' ',outfile);
                    }
                    lineFed = 0;
                }
                putToken();
            }
            if (tokType == OPERATOR && (*tokenString=='.' || *tokenString==':'))
            {
                contStatement = 0;
            }
            else if ( isToken("EDITING") || isToken("REPEAT") ||
                isToken("DO") ||
                (isToken("FOR") && isNextToken("EACH")) )
            {
                indLevel++;
            }
            else if (isToken("FORM"))
            {
                inForm = 1;
            }
            else if (tokType != COMMENT && tokType != INCLUDE)
            {
                contStatement = 1;
            }
        }
        else if (mParseMode || rParseMode ) /*** GuiLite Parse/Transform ***/
        {
            /* Track frame scoping and block levels -------------------------*/
            if ( isToken("REPEAT") || (isToken("FOR") && isNextToken("EACH")) )
            {
                if ( guiRptMode && isToken("REPEAT")  && firstRepeat )
                {
                    guiUI();
                }

                /* indent Level used for frame scopes */
                indLevel++;
                if (indLevel == MAX_INDLEV)
                {
                    outOfHere("MAX_INDLEV Exceeded:");
                }
                iBlock++;     /* sequential block tracking */
                newBlock = 1; /* Frame scoping block */
                if ( !suppressRepeat )
                {
                    putToken();
                }
                else
                {
                    /* guiRpt stopping the outer REPEAT: */
                    suppressRepeat = 0;
                }
            }
            /*-----------------------------------------------------------*/
            else if (isToken("EDITING") || isToken("DO"))
            {
                if (isToken("EDITING"))
                {
                    /* EDITING only works reliably with the character UI. */
                    fprintf(logfile,
                        "%s  %d WARN1 \"EDITING\" FOUND.\n",
                        srcName, lineNbr);
                }
                indLevel++;   /* indent Level used for frame scopes */
                iBlock++;     /* sequential block tracking */
                newBlock = 2; /* non-Frame scoping block */
                putToken();
            }
            /*-----------------------------------------------------------*/
            else if (isToken("END"))
            {
                if (guiRptMode && suppressEnd && indLevel == 1)
                {
                    /* Skip END token and it's "." */
                    suppressEnd = 0;
                    getToken();
                }
                else if (guiRptMode && (indLevel == 1) &&
                    !firstRepeat && (Section != ReportPrt))
                {
                    outOfHere("Premature REPEAT:");
                }
                else
                {
                    /* skip the . after END */
                    putToken();
                    getToken();
                    putToken();
                }
                if (mParseMode)
                {
                    /* Check beam-up flag after error handling blocks with
                    ** input events, must have mfdeclre within the file or
                    ** be an include-file.
                    */
                    if (checkBlock(indLevel) &&
                        (hasMfdtit || hasMfddec || srcType))
                    {
                        if (isNextToken("ELSE"))
                        {
                            fprintf(logfile,"%s %d ELSE in the way\n",
                                srcName,lineNbr);
                        }
                        else
                        {
                            touched++;
                            if (convertMode != CONVERT_MODE_REPORT_AND_MAINT)
                            {
                                fprintf(outfile, beamMeUp);
                            }
                            else
                            {
                                fprintf(outfile, condBeamMeUp);
                            }
                        }
                    }
                }
                if (--indLevel < 0)
                {
                    indLevel = 0;
                }
            }
            /*-----------------------------------------------------------*/
            else if (tokType == INCLUDE && isToken("gprun.i") &&
                mParseMode && (hasMfdtit || hasMfddec || srcType) )
            {
                putToken();
                touched++;
                if (convertMode != CONVERT_MODE_REPORT_AND_MAINT)
                {
                    fprintf(outfile, beamMeUp);
                }
                else
                {
                    fprintf(outfile, condBeamMeUp);
                }

                /* Watch out for {~~~}.  */
                if (isNextToken("."))
                {
                    /* Progress allows extra period after Include, */
                    /* but not after a statement */
                    getToken();
                }
            }
            /* Track certain keywords within statements  --------------------*/
            else if (isToken("WITH"))
            {
                gotWith = 1;
                putToken();
            }
            /*-----------------------------------------------------------*/
            else if (gotWith && isToken("FRAME") )
            {
                putToken();
                getToken();
                strcpy(withName, tokenString);
                putToken();
            }
            /*-----------------------------------------------------------*/
            else if (tokType == KEYWORD && isnToken("DISP",4) ) /* Abbrev's ! */
            {
                gotDisp = 1;
                putToken();
            }
            /* Look for actions to take at the end of statements  -----------*/
            else if ((isToken(".") || isToken(":")) && isspace(*currentPointer))
            {
                /* Avoid periods within compound table.field names */
                if (rParseMode && gotDisp)
                {
                    /* End of Report display statement */
                    if (*withName)
                    {
                        /* Has statement level frame phrase */
                        if (strcasecmp(withName, "A") == 0)
                        {
                            thisIsFrameA = 1;
                        }
                        else
                        {
                            thisIsFrameA = 0;
                        }
                    }
                    else
                    {
                        /* Compare implicit frame in scope. */
                        if (strcasecmp(frameScope[indLevel], "A") == 0)
                        {
                            thisIsFrameA = 1;
                            fprintf(logfile,
                                "%s %d IMPLICIT frame A\n",srcName,lineNbr);
                        }
                        else
                        {
                            thisIsFrameA = 0;
                        }
                    }

                    /* Not FRAME A and this isn't a nonStreamIoFrame */
                    if ((0 == thisIsFrameA) &&
                        (frameNameFoundInList(withName, nonStreamIoFrames) == 0))
                    {
                        if (gotWith)
                        {
                            if (CONVERT_MODE_REPORT_AND_MAINT == convertMode)
                            {
                                fputs (condStreamIO, outfile);
                            }
                            else
                            {
                                fputs (streamIO, outfile);
                            }
                        }
                        else
                        {
                            if (CONVERT_MODE_REPORT_AND_MAINT == convertMode)
                            {
                                fputs (condWithStreamIO, outfile);
                            }
                            else
                            {
                                fputs (withStreamIO, outfile);
                            }
                        }
                        touched++;
                        if (verbose)
                        {
                            fprintf(logfile,
                                "STREAM-IO added at line %d\n", lineNbr);
                        }
                    }
                }
                else if (newBlock)
                {
                    setFrameScope();
                }
                else if ((needTitleDisplay) &&
                    (frameNameFoundInList(frameName, dontRefreshTitles) == 0))
                {
                    touched++;
                    fprintf(outfile, titRefresh, frameName, frameName);
                    needTitleDisplay = 0;
                }

                /* Put the statement terminator */
                putToken();
                if (newBlock && blck[iBlock].erract && blck[iBlock].errevt &&
                    mParseMode && (hasMfdtit || hasMfddec || srcType))
                {
                    touched++;
                    if (convertMode != CONVERT_MODE_REPORT_AND_MAINT)
                    {
                        fprintf(outfile, beamMeUp);
                    }
                    else
                    {
                        fprintf(outfile, condBeamMeUp);
                    }
                }
                newBlock = gotDisp = gotWith = errorAction = 0;
                *withName = '\0';
            }
            /* Inspect all EXPLICIT Row clauses----------------------------- */
            else if ( isToken("ROW") )
            {
                putToken();
                getToken();
                if (tokType == NUMBER)
                {
                    putToken();
                    if (atoi(tokenString) < 4)
                    {
                        fprintf(logfile,
                            "%s %d WARN1 Low ROW\n",srcName,lineNbr);
                    }
                }
                else if ( isToken("FRAME-ROW") || isToken("FRAME-LINE") )
                {
                    putToken();
                    getToken();
                    putToken();
                    for (plevel=1; plevel>0; ) /* pass the ()'s */
                    {
                        getToken();
                        putToken();
                        if ( isToken("(") )
                        {
                            plevel++;
                        }
                        else if ( isToken(")") )
                        {
                            plevel--;
                        }
                    }
                    if ( isNextToken("+") || isNextToken("-") )
                    {
                        fprintf(logfile,
                            "%s %d WARN1 Complex ROW\n",srcName,lineNbr);
                    }
                }
                else putToken();
            }
            /*-----------------------------------------------------------*/
            /* Full Gui Report conversion must cut up report main procedure --*/
            else if (guiRptMode &&
                (isToken("SET") || isToken("UPDATE") || isToken("PROMPT-FOR")))
            {
                guiDE();
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && (isToken("mainloop") ||
                isToken("main-loop")))
            {
                /* Remove mainloop labels in Full Gui Reports, subproc has it */
                if (isNextToken(":") && !newBlock)
                {
                    /* A mainloop label, not a next/leave command */
                    getToken();
                    touched++,fputs("\n/*GUI mainloop removed */\n", outfile);
                }
                else
                {
                    putToken();
                }
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && (Section != ReportPrt) && isToken("FIND"))
            {
                /* Replicate this FIND in the Report Block */
                guiPreFind();
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && (Section != ReportPrt) &&
                (isToken("DEFINE") ||
                isToken("DEF")) && isNextToken("BUFFER"))
            {
                /* Move this DEFINE BUFFER to the Report Block */
                guiPreBuffer();
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && isToken("bcdparm") )
            {
                putToken(); /*  output the bcdparm */
                            /* switch from general to field validations */
                Section = FieldVals;
                gotBcdparm = 1;
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && ( ! gotBcdparm ) &&
                ((tokType == INCLUDE && isToken("gprun.i")  &&
                is_gpquote(tokBeg)) ||
                (tokType == INCLUDE && isToken("mfquoter.i")) ))
            {
                outOfHere("BCDPARM missing: ");
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && isToken("NEXT-PROMPT") &&
                (Section == GeneralVals || Section == FieldVals))
            {
                skipStatement("NEXT-PROMPT"); /* Suppress Next-Prompts */
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && isToken("UNDO") &&
                (Section == GeneralVals || Section == FieldVals))
            {
                skipStatement("UNDO");
                fputs(" RETURN ERROR.", outfile);
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && (Section == UIsetup) &&
               isToken("if") && isNextToken("not batchrun"))
            {
                /* skip past the 'then do:' the skip the whole block */
                fprintf(logfile,
                    "%s %d Batchrun Section removed\n",srcName,lineNbr);
                outputOn = 0;
                skipStatement(NULL);
                skipBlock(1);
                if ( ! isNextToken("ELSE"))
                {
                    outOfHere("Bad Batchrun Section in Report");
                }
                do getToken();
                while ( isNotToken("ELSE"));
                if (isNextToken("DO"))
                {
                    /* Skip DO: and the closing END. */
                    getToken();
                    getToken();
                    suppressEnd = 1;
                }
                outputOn = 1;
                touched++;
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && isToken("batchrun") &&
                Section != Preloop && Section != ReportPrt)
            {
                outOfHere("Bad Batchrun Statement in Report");
            }
            /*-----------------------------------------------------------*/
            else if (tokType == INCLUDE && (!strcasecmp(incName, "mfrpexit.i")))
            {
                if (rParseMode)
                {
                    guiRpexit();
                }
                else
                {
                    outOfHere("REPORT CODE: ");
                }
            }
            /*-----------------------------------------------------------*/
            else if (tokType == INCLUDE && (!strcasecmp(incName, "mfrpchk.i")))
            {
                if (rParseMode)
                {
                    guiRpchk();
                }
                else
                {
                    outOfHere("REPORT CODE: ");
                }
            }
            /*-----------------------------------------------------------*/
            else if (tokType == INCLUDE &&
                (!strcasecmp(incName,"mfrtrail.i") ||
                !strcasecmp(incName,"mftrl080.i") ||
                !strcasecmp(incName,"mfreset.i")) )
            {
                if (guiRptMode)
                {
                    if (!guiSubDone)
                    {
                        guiSubDone=1;
                        /* Only trail includes get mfguitrl */
                        if (strcasecmp(incName,"mfreset.i"))
                        {
                            fputs(guiRptInc8, outfile);
                        }
                        fputs(guiRptInc9, outfile);
                    }
                    else
                    {
                        outOfHere("Double TRAIL-RESET");
                    }

                    if (!strcasecmp(incName,"mfreset.i"))
                    {
                        putToken();
                    }
                }
                else if ((mParseMode) &&
                    (convertMode != CONVERT_MODE_REPORT_AND_MAINT))
                {
                    outOfHere("REPORT CODE: ");
                }
                else
                {
                    putToken();
                    /* If we are here for mfreset include mfgrptrm
                     * Don't need to do it for the trails because they already
                     * include mfreset, causing them to come right here and
                     * get mfgrptrm for themselves
                     */
                    if (!strcasecmp(incName,"mfreset.i"))
                    {
                        fputs(guiRptInc9, outfile);
                    }
                }
            }
            /*-----------------------------------------------------------*/
            else if (tokType == INCLUDE &&
                (!strcasecmp(incName,"mfreset2.i") ||
                !strcasecmp(incName,"mfselp01.i")) &&
                guiRptMode)
            {
                outOfHere("Interrupted PRINT: ");
            }
            /*-----------------------------------------------------------*/
            else if (rParseMode && tokType == INCLUDE &&
                (!strcasecmp(incName, MFSELPRT_I) ||
                !strcasecmp(incName, MFSELBPR_I) ||
                !strcasecmp(incName, GPSELOUT_I) ))
            {
                if (guiRptMode)
                {
                    /* Close quote-proc */
                    fputs(guiRptInc4, outfile);
                    guiWriteTriggers();
                    fputs("/*GUI MFSELxxx removed*/", outfile);

                    /* Start Report Proc */
                    fprintf(outfile, guiRptInc5, gpprtrpaParams);
                    Section = ReportPrt;
                    StartedPReport = 1;

                    /* Buffers & Finds */
                    guiDupThings();
                }
                else /* GuiLite report mode */
                {
                    putToken();
                    if (rectInFrame)
                    {
                        touched++;
                        fputs(prtInject, outfile);
                        if (verbose)
                        {
                            fprintf(logfile,
                            "%s %d PRINTER Selection adjusted\n",
                            srcName, lineNbr);
                        }
                    }
                }
            }
            /*-----------------------------------------------------------*/
            else if (guiRptMode && Section == FieldVals &&
                isToken("IF") && indLevel == 1)
            { /* Examine all IF statements in this area, suppress Field
                  Validation Triggers if found, and mark them. */
                checkForTrigger();
            }
            /*----- PARSE COMPLETE FORM STATEMENT --------------------------*/
            else if (isToken("FORM") )
            {
                /* Mark our position and silently do some look-ahead */
                anchor = currentPointer;
                outputOn = 0;
                lineNbrSave = lineNbr;
                scrollFrame = 0;
                noColLabels = 0;
                titleInFrame = 0;
                gotWith = 0;
                frameWidth = 0;
                noOverlay = 1;
                *frameName = '\0';
                do
                {
                    getToken();
                    /* move title only in frames without column labels */
                    if ( isnToken("SIDE-LABEL", 10) || isToken("NO-LABELS") )
                    {
                        noColLabels = 1;
                    }
                    else if ( isToken("DOWN") )
                    {
                        gotDown = 1;
                        if (strcasecmp(prevToken, "1") == 0)
                        {
                            fprintf(logfile,"%s %d WARN 1 DOWN\n",
                                srcName, lineNbr);
                            gotDown = 0;
                        }
                    }
                    else if ( isToken("OVERLAY") )
                    {
                        noOverlay = 0;
                    }
                    else if ( isToken("FRAME") )
                    {
                        getToken();
                        if (tokType == INCLUDE)
                        {
                            *frameName = '{';
                            strcpy(frameName+1, tokenString);
                            i = strlen(frameName);
                            *(frameName+i) = '}';
                            *(frameName+i+1) = '\0';
                        }
                        else
                        {
                            strcpy(frameName, tokenString);
                        }
                    }
                    else if ( isToken("WIDTH") )
                    {
                        getToken();
                        frameWidth = atoi(tokenString);
                        if ((frameWidth > 80) && mParseMode &&
                            (convertMode != CONVERT_MODE_REPORT_AND_MAINT))
                        {
                            outOfHere("REPORT CODE: ");
                        }
                    }
                    else if ((mParseMode &&
                        (convertMode != CONVERT_MODE_REPORT_AND_MAINT)) &&
                        (isToken("HEADER") || isToken("PAGE-TOP") || isToken("PAGE-BOTTOM")))
                    {
                        outOfHere("REPORT CODE: ");
                    }
                    else if ( isToken("TITLE") )
                    {
                        titleInFrame = 1;
                    }
                    else if ( isToken("SCROLL") )
                    {
                        scrollFrame = 1;
                    }
                } while ( isNotToken(".") );

                if (guiRptMode)
                {
                    titleInFrame = 1;
                    strcpy(titText, SELECTION_TITLE);
                }

                /* Now go back and write this puppy out */
                touched++;
                currentPointer = anchor;
                outputOn = 1;
                lineNbr = lineNbrSave;
                writeFormStmt(&anchor);

                gotDown = gotWith = 0;
            } /* End process FORM keyword -----------------------------------*/

            /*-----------------------------------------------------------*/
            else if (isToken("CLEAR") && isNextToken("frame"))
            {
                /* Refresh GUI titles after CLEAR FRAME X ...
                ** Except when the frame name is Inc-Param or with ALL
                ** since ALL => a down frame which has no GUI title
                ** or if frame is know to be down via downFrame list.
                */
                putToken(); /* CLEAR */
                getToken();
                putToken(); /* frame */
                getToken();
                putToken(); /* Frame-name */

                /* Build frame-name with surrounding commas for name check */
                *frameName = ',';
                strcpy(frameName+1, tokenString);
                i = strlen(frameName);
                *(frameName+i) = ',';
                *(frameName+i+1) = '\0';
                if ( isNextToken("all") ||
                    (frameNameFoundInList(frameName, downFrames) == 1))
                {
                    fprintf(logfile,"%s: Did not refresh frame title after CLEAR at line %d\n",
                        srcName,
                        lineNbr);
                }
                else
                {
                    needTitleDisplay = 1;
                    if (verbose)
                    {
                        fprintf(logfile,
                            "%s %d GUI TITLE refreshed\n",
                            srcName,lineNbr);
                    }
                }
                /* Save REAL frame name for later use */
                if (tokType == INCLUDE)
                {
                    *(frameName+i) = '}';
                    *frameName = '{';
                }
                else
                {
                    strcpy(frameName, tokenString);
                }
            }
            /*-----------------------------------------------------------*/
            else if (isToken("VIEW") && isNextToken("frame dtitle."))
            {
                touched++;
                fputs(toolBar, outfile);
                outputOn = 0;
                getToken();
                getToken();
                outputOn = 1;
                getToken();
                if (verbose)
                {
                    fprintf(logfile,"%s %d VIEW FRAME DTITLE replaced\n",
                    srcName,lineNbr);
                }
            }
            /* ----------------*** The BIG ELSE ***----------------------*/
            else
            {
                /* Nothing special, just output the token */
                putToken();
            }

        } /* End of Parsing modes (M and R) */
    } /* end while over stream of tokens in file */

    /* If this is a gui report and
    ** RPMain isn't called from an include file and
    ** the p-report procedure declaration was begun.
    */
    if ((guiRptMode) && ('\0' == RPMainCaller[0]) && (1 == StartedPReport))
    {
        fprintf(outfile, guiRptInc10, UIlist);
    }

    if (verbose && downFrames[1])
    {
        fprintf(logfile,"D-Frames: %s\n", downFrames);
    }
    if ((convertMode != CONVERT_MODE_NO_CONVERT_NO_OUTPUT) &&
        (convertMode != CONVERT_MODE_NO_CONVERT))
    {
        fclose(outfile);
    }

    if (convertMode != NOT_FOUND)
    {
        fprintf(logfile,"%s Done. %i %s Transformations, %i lines\n",
            outfileName,
            touched,
            converterModeString[convertMode],
            lineNbr - 1);
    }
    else
    {
        fprintf(logfile,"%s Done. %i Transformations, %i lines\n",
            outfileName,
            touched,
            lineNbr - 1);
    }

    /* Remove 0-length output file if in 'no output' mode */
    if ((CONVERT_MODE_NO_CONVERT_NO_OUTPUT == convertMode) && ( ! fldListMode ))
    {
        unlink(outfilePath);
    }

} /* end of convertFile() */


/******************************************************************************
 *  INTERNAL FILE HANDLING & PARSING ROUTINES                                 *
 *****************************************************************************/

/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
char *readUntil(string, targetChar)
char *string;
char targetChar;
{
   while ((*string != targetChar) && (*string != EOF))
   {
      ++string;
   }

   return (string);
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
int copySimpleToken(appendToMe, appendFromMe)
char *appendToMe;
char *appendFromMe;
{
   int returnStatus = 0;

   /*
   ** Bypass leading whitespace.
   */
   while (isspace(*appendFromMe))
   {
      ++appendFromMe;
   }

   /*
   ** Check for a quoted string token.
   */
   if ('"' == *appendFromMe)
   {
      /*
      ** Copy until close-quote.
      */
      strncat(appendToMe, appendFromMe, 1);
      ++appendFromMe;
      for (;
         (*appendFromMe != '"') && 
         (*appendFromMe != '\0');
         ++appendFromMe)
      {
         /*
         ** Add the new character to the output string.
         */
         strncat(appendToMe, appendFromMe, 1);
      }


      /*
      ** Check for close-quote.
      */
      if ('"' == *appendFromMe)
      {
         /*
         ** Add the close-quote to the output string.
         */
         strncat(appendToMe, appendFromMe, 1);
      }
      else
      {
         returnStatus = 1;
      }
   }
   else
   {
      /*
      ** Copy until the next whitespace.
      */
      for (;
         (! isspace(*appendFromMe)) &&
         (*appendFromMe != '\0');
         ++appendFromMe)
      {
         strncat(appendToMe, appendFromMe, 1);
      }
   }

   return (returnStatus);
}


/*------------------------------------------------------------------------------
** gpselout.i parameters are of the form
** {gpselout.i
**   &printType = "printer"
**   &printWidth = 132
**   &pagedFlag = "page"
**   &stream = "stream repout"
**   &appendToFile = "append"
**   &streamedOutputToFile = "yes"
**   &withBatchOption = "yes"
**   &displayStatementType = 1
**   &withCancelMessage = "yes"
**   &pageBottomMargin = 6
**   &withEmail = "yes"
**   &withWinprint = "yes"
**   &defineVariables = "yes"}
** and need to be converted to different strings, as shown below
**
** mfguirpa.i  gpselout.i
**    {1}      &withBatchOption
**    {2}      &printType
**    {3}      &printWidth
**    {4}      &pagedFlag
**    {5}      &stream
**    {6}      &appendToFile
**    {7}      &streamedOutputToFile
** Thus after conversion the mfguirpa.i call would look like
** {mfguirpa.i true "printer" 132 "page" "stream repout" "append" "yes"}
** 
** gpprtrpa.i   gpselout.i
**    {1}       &printType
**    {2}       &printWidth
**    {3}       &pagedFlag
**    {4}       &stream
**    {5}       &appendToFile
** Thus after conversion the gpprtrpa.i call would look like
** {gpprtrpa.i "printer" 132 "page" "stream repout" "append" }
**----------------------------------------------------------------------------*/
int parseGpseloutParams(paramString, isBatchable, gpprtrpaParams, mfguirpaParams)
char *paramString; /* input, holds gpselout.i parameters. */
int *isBatchable;  /* input/output, tells if report is batchable. */
char *gpprtrpaParams;
char *mfguirpaParams;
{
#define PRINT_TYPE 0
#define PRINT_WIDTH 1
#define PAGED_FLAG 2
#define STREAM 3
#define APPEND_TO_FILE 4
#define STREAMED_OUTPUT_TO_FILE 5
#define WITH_BATCH_OPTION 6
#define DISPLAY_STATEMENT_TYPE 7
#define WITH_CANCEL_MESSAGE 8
#define PAGE_BOTTOM_MARGIN 9
#define WITH_EMAIL 10
#define WITH_WIN_PRINT 11
#define DEFINE_VARIABLES 12
#define NUMBER_OF_GPSELOUT_FIELDS 13

   int thisField = 1;
   char gpseloutParams[NUMBER_OF_GPSELOUT_FIELDS][64];
   char *paramStringTraveller = paramString;
   int returnStatus = 0;

   strcpy(gpprtrpaParams, "");
   strcpy(mfguirpaParams, "");
   strcat(paramString, "\n");

   for (thisField = 0;
      (thisField < NUMBER_OF_GPSELOUT_FIELDS) && (0 == returnStatus);
      ++thisField)
   {
      strcpy(gpseloutParams[thisField], "");
      paramStringTraveller = readUntil(paramStringTraveller, '=');
      if (*paramStringTraveller != EOF)
      {
         ++paramStringTraveller;
         returnStatus =
            copySimpleToken(gpseloutParams[thisField], paramStringTraveller);
      }
      else
      {
        /* set the error flag */
        returnStatus = 1;
      }
   }

   if (0 == returnStatus)
   {
      /*
      ** Create the param strings.
      */
      if (strcasecmp(gpseloutParams[WITH_BATCH_OPTION], "\"yes\"") == 0)
      {
         *isBatchable = 1;
         /*
         ** Replace the string "yes" with the logical true.
         */
         strcpy(gpseloutParams[WITH_BATCH_OPTION], "true");
      }
      else
      {
         *isBatchable = 0;
         /*
         ** Replace the string "no" with the logical false.
         */
         strcpy(gpseloutParams[WITH_BATCH_OPTION], "false");
      }

      sprintf(gpprtrpaParams, "%s %s %s %s %s %s ", 
         gpseloutParams[PRINT_TYPE],
         gpseloutParams[PRINT_WIDTH],
         gpseloutParams[PAGED_FLAG],
         gpseloutParams[STREAM],
         gpseloutParams[APPEND_TO_FILE],
         gpseloutParams[STREAMED_OUTPUT_TO_FILE]);

      sprintf(mfguirpaParams, "%s %s %s %s %s %s ", 
         gpseloutParams[WITH_BATCH_OPTION],
         gpseloutParams[PRINT_TYPE],
         gpseloutParams[PRINT_WIDTH],
         gpseloutParams[PAGED_FLAG],
         gpseloutParams[STREAM],
         gpseloutParams[APPEND_TO_FILE]);
   }
   else
   {
      outOfHere("Error parsing GPSELOUT parameters");
   }

   return (returnStatus);
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
int buildLongName(srcSearchPath, targetFilename, longName)
char *srcSearchPath;  /* Contains comma-separated search dirs. */
char *targetFilename; /* Pointer to the short name. */
char *longName;       /* Build the full name into this variable */
{
    int found = 0;
    char *search_dir = NULL;
    char tmp_srcSearchPath[MAX_TOK_LEN];
#if _DOS && !__BORLANDC__
    struct _stat statbuf;
#else
    struct stat statbuf;
#endif

    strcpy(tmp_srcSearchPath, srcSearchPath);

    search_dir = strtok(tmp_srcSearchPath, ",");
    while ((search_dir != NULL) && (0 == found))
    {
        strcpy(longName, search_dir);

        if (longName[ strlen(search_dir) - 1 ] != DIRCHAR)
        {
            longName[ strlen(search_dir) ] = DIRCHAR;
            longName[ strlen(search_dir) + 1 ] = '\0';
        }
        strcat(longName, targetFilename);

        if (stat(longName, &statbuf) != -1)
        {
            found = 1;
        }
        else
        {
            search_dir = strtok(NULL, ",");
        }
    }

    return (found);
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
static int encryptCheck(srcBuf, bufSize, srcName, logfile)
char srcBuf[];
unsigned int bufSize;
char *srcName;
FILE *logfile;
{
    int status = 0;
    unsigned int index;

    for (index = 0;
        (index < ENCRYPTION_CHECK_BYTE_COUNT) &&
        (index < bufSize) &&
        (0 == status);
        ++index)
    {
        if (srcBuf[index] > 126 || srcBuf[index] < 1)
        {
            /* If SIGNED char out of range assume an encrypted file. */
            fprintf(logfile,"%s ENCRYPTED File (%d), treat as NoConvert\n",
                srcName,
                (int) srcBuf[index]);
            status = ENCRYPTION_CHECK_BYTE_COUNT;
        }
    }

    return (status);
}


/*------------------------------------------------------------------------------
** Open the source file and fetch its contents
**----------------------------------------------------------------------------*/
int openSourceFile()
{
    int status = 0;
    char tmpname[MAX_TOK_LEN];
    long bytesread;
    int i, fd_in;

#if _DOS && !__BORLANDC__
    struct _stat statbuf;
#else
    struct stat statbuf;
#endif

    /*  Get it's size and read the whole thing into a buffer to make working
    **  on it easy. Progress V6 source files SHOULD all be short enough to
    **  easily fit into memory due to 63K r-code limit.
    */
    if ( (srcName=strrchr(srcPath, DIRCHAR)) == NULL )
    {
        /* given a short-filename, find source directory */
        strcpy(tmpname, srcPath);
        if (1 == buildLongName(srcSearchPath, tmpname, srcPath))
        {
            srcName = strrchr(srcPath, DIRCHAR);
            i = stat(srcPath, &statbuf);
        }
        else
        {
            /* Indicate file not found. */
            i = 1;
        }
    }
    else
    {
        /* Name had path in it. */
        i = stat(srcPath, &statbuf);
        strcpy(tmpname, srcPath);
    }

    if (i)
    {
        fprintf(logfile,"QUIT: Unable to read srcSearchPath/%s (%d)\n",
            tmpname, errno);
        return 1;
    }
    srcName++; /* point to short name */
    srcType = strstr(srcName, ".p") == NULL;
    if (statbuf.st_size >= PROGRAM_BUFFER_SIZE)
    {
        fprintf(logfile,"QUIT: %s (%ld) too BIG for buffer (%ld).\n",
            srcName, (unsigned long) statbuf.st_size,
            (unsigned long) PROGRAM_BUFFER_SIZE);
        return 1;
    }
#if _DOS && !__BORLANDC__
    if ((fd_in = open(srcPath,_O_RDONLY|_O_TEXT)) == -1)
#else
    if ((fd_in = open(srcPath,O_RDONLY)) == -1)
#endif
    {
        fprintf(logfile,"Error: Cannot open file: %s %d\n", srcPath, errno);
        return 1;
    }
    /* Trap actual bytes read for DOS which translates CR-LF to LF on read() */
    bytesread = read(fd_in, srcBuf, statbuf.st_size);
    if (bytesread == -1)
    {
        fprintf(logfile,"Error: Cannot read file: %s (%d)\n", srcPath, errno);
        return 1;
    }
    srcBuf[bytesread] = '\0'; /* Null terminate the source buffer to not EOF */
    close(fd_in);

    /* Check to see if the file is encrypted */
    status = encryptCheck(srcBuf, statbuf.st_size, srcName, logfile);
    return (status);

} /* end openSourceFile() */


/*------------------------------------------------------------------------------
** Add trailing Directory Char
**----------------------------------------------------------------------------*/
static void addTrailingDirChar(dirName)
char *dirName;
{
    int originalDirNameLength;

    if ((dirName != NULL) && (strlen(dirName) > 0))
    {
        originalDirNameLength = strlen(dirName);
        if (dirName[originalDirNameLength - 1] != DIRCHAR)
        {
            dirName[originalDirNameLength] = DIRCHAR;
            dirName[originalDirNameLength + 1] = '\0';
        }
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
static char *baseNameOf(pathAndName)
char *pathAndName;
{
    char *baseName = pathAndName;

    if ((pathAndName != NULL) && (strrchr(pathAndName, DIRCHAR) != NULL))
    {
        baseName = strrchr(pathAndName, DIRCHAR);
        ++baseName;
    }

    return (baseName);
}


/*------------------------------------------------------------------------------
** Open the output file if necessary
**----------------------------------------------------------------------------*/
int openOutFile(name)
char *name; /* Destination path from command line */
{
    strcpy(outfilePath, name);
    addTrailingDirChar(outfilePath);

    /* Set a pointer to just the base name. */
    outfileName = baseNameOf(outfilePath);

    /* Same output name as input, just in destination path */
    strcat(outfilePath, srcName);

    if (okToOverwrite(outfilePath))
    {
        if ((outfile=fopen(outfilePath, "w")) == NULL)
        {
            fprintf(logfile,"Cannot open %s %i\n", outfilePath, errno);
            return 1;
        }
    }
    return 0;
} /*end openOutFile()*/


/*------------------------------------------------------------------------------
** Initialize the scanner globals.
**----------------------------------------------------------------------------*/
void initScanner()
{
    int ch, i, ii;
    char argName[MAX_TOK_LEN], *argValue, *bad_filename_char;
    FILE *iniStream;

    /*
    **  Initialize character table.
    */
    for (ch = 0;   ch < 256;  ++ch) char_table[ch] = SPECIAL;
    for (ch = '0'; ch <= '9'; ++ch) char_table[ch] = DIGIT;
    for (ch = 'A'; ch <= 'Z'; ++ch) char_table[ch] = LETTER;
    for (ch = 'a'; ch <= 'z'; ++ch) char_table[ch] = LETTER;
    char_table['_'] = LETTER;
    char_table['#'] = NAMECHAR;
    char_table['$'] = NAMECHAR;
    char_table['%'] = NAMECHAR;
    char_table['&'] = NAMECHAR;
    char_table['-'] = NAMECHAR;
    char_table['\''] = QUOTE;
    char_table['\"'] = QUOTE;
    char_table[EOF_CHAR] = EOF_CODE;

    /*
    **  Initialize keyword table.
    */
    for (i=0,ii=0; i<26; i++)
    {
        ch = 'A' + i;
        /* the SUN compiler chokes on NULL comparison, so ~ is the sentinel */
        while (*keywordTable[ii] != '~' && *keywordTable[ii] < ch)
        {
             ii++;
        }
        if (*keywordTable[ii] == ch)
        {
            keytab[i] = ii;
        }
        else
        {
            /* No keywords starting with this letter */
            keytab[i] = -1;
        }
    }

    srcBuf = (char *) malloc((size_t) PROGRAM_BUFFER_SIZE);
    if (srcBuf == NULL)
    {
        fprintf(logfile,"Malloc error (%d)\n", errno);
        exit(1);
    }

    if (NULL == iniFileName)
    {
        iniFileName = INI_FILENAME;
    }
    iniStream = fopen(iniFileName, "r");
    if (iniStream == NULL)
    {
#if _DOS
        fprintf(logfile,
            "Error reading initialization file, \"%s\", %s.\n",
            iniFileName,
            strerror(errno));
#else   /*_DOS*/
        fprintf(logfile,"Unable to open initialization file, \"%s\"\n",
            iniFileName);
#endif  /*_DOS*/
        exit(1);
    }

    while (fgets(argName, SHORT_STRING_LENGTH, iniStream) != NULL)
    {
        if (strlen(argName) < 3 || *argName == '#')
        {
            /* skip blank lines  and comments */
            continue;
        }
        if ((argValue=strchr(argName, '=')) == NULL)
        {
            fprintf(logfile,"Unrecognized %s entry: %s\n",iniFileName,argName);
            exit(1);
        }
        *argValue++ = '\0';
        argValue[strlen(argValue) - 1] = '\0'; /* Axe the NEWLINE */

        /* Remove possible bad filename char from DOS => UNIX migration. */
        bad_filename_char = strchr(argValue, 13);
        if (bad_filename_char != NULL)
        {
            *bad_filename_char = '\0';
        }

        if (!strcasecmp(argName,"srcSearchPath"))
        {
            strcpy(srcSearchPath, argValue);
        }
        else if (!strcasecmp(argName,"workDir"))
        {
            strcpy(workDir, argValue);
            addTrailingDirChar(workDir);
        }
        else if (!strcasecmp(argName,"targetDir"))
        {
            strcpy(targetDir, argValue);
            addTrailingDirChar(targetDir);
        }
        else if (!strcasecmp(argName,"overwriteExistingTarget"))
        {
            if (!strcasecmp(argValue, "no"))
            {
                okToOverwriteTarget = 0;
            }
        }
        else
        {
            fprintf(logfile,"Illegal %s option: %s\n",
                iniFileName, argName);
            exit(1);
        }
    }
    fclose(iniStream);

    /* Must have target and work dir's, and at least one source dir */
    if ( (! *targetDir) || (! *srcSearchPath) )
    {
        fprintf(logfile,
            "Incomplete %s file:\n"
            "  srcSearchPath=%s\n"
            "  targetDir=%s\n"
            "  workDir=%s\n",
            iniFileName,
            srcSearchPath,
            targetDir,
            workDir);
        exit(1);
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
char *copyFrameName(destination, source)
char *destination;
char *source;
{
    /*
    ** Assume this routine will be called only for correctly formattted
    ** V8:directive=value
    ** lines
    */
    while ((! isspace(*source)) && (*source != '*'))
    {
        *destination = *source;
        ++destination;
        ++source;
    }
    *destination = '\0';

    return (source);
}


/*------------------------------------------------------------------------------
** returns 1 if found, else 0
**----------------------------------------------------------------------------*/
int frameNameFoundInList(frameName, frameList)
char *frameName;
char *frameList;
{
    int returnCode = 0;
    char targetFrame[48];

    /* frameName must be valid, and
    ** the comma delimited frameList must have more than a leading comma.
    */
    if ((strlen(frameName) > 0) && (strlen(frameList) > 1))
    {
        /* the frame name must be delimited by commas */
        if (frameName[0] != ',')
        {
            sprintf(targetFrame, ",%s", frameName);
        }
        else
        {
            strcpy(targetFrame, frameName);
        }
        if (targetFrame[strlen(targetFrame) - 1] != ',')
        {
            strcat(targetFrame, ",");
        }

        if (strstr(frameList, targetFrame) != NULL)
        {
            returnCode = 1;
        }
    }

    return (returnCode);
}


/*------------------------------------------------------------------------------
** Initialize globals for processing a new source file
**----------------------------------------------------------------------------*/
void initFile(defaultConvertMode) /* Initialize for a new source file */
int defaultConvertMode;
{
    declMode = 0;
    rParseMode = 0;                   /* ConvertMode=Report */
    mParseMode = 0;                   /* ConvertMode=Maintenance */
    guiRptMode = 0;                   /* ConvertMode=FullGUIReport */
    concatStringMode = 0;             /* combine utility explination strings */
    stringsToConcat = 0;              /* how many string to concat */
    stringsConcated = 0;              /* how many done */
    StartedPReport = 0;               /* section of a FullGUIReport */
    convertMode = defaultConvertMode; /* how to GUI convert the file */
    lineNbr = 1;                      /* current line number */
    indLevel = 0;                     /* Indent/block Level */
    iBlock = 0;                       /* Sequential Block numbering */
    errorAction = 0;                  /* Error handling action of a block */
    contStatement = 0;                /* Boo-coo flags add naseum ... */
    suppressRepeat = 0;               /* Suppressing output of certain code */
    suppressEnd = 0;                  /* Suppressing "if not batchrun then do:"'s END. */
    firstRepeat = 1;                  /* Outer repeat of a rept/inq */
    lineFed = 0;                      /* NewLine outputed flag */
    veryTop = 1;                      /* Very top of input file flag */
    inForm = 0;                       /* SET when within a FORM statement */
    isBatchable = 0;                  /* Batchable report program (with mfselbpr.i) */
    touched = 0;                      /* How many transformation done on this file */
    guiSubDone = 0;                   /* The mftrail/mfreset Full GUI subst done */
    gotDisp = 0;                      /* Got one of these statements .... */
    gotDown = 0;
    gotWith = 0;
    gotBcdparm = 0;
    hasGpselout = 0;
    hasMfsel = 0;
    hasMfdtit = 0;
    hasMfddec = 0;
    newBlock = 0;                     /* Set at a DO/REPEAT/FOR EACH/EDITING block beg */
    noColLabels = 0;                  /* Track op elements/attributes of frame defs */
    noOverlay = 1;
    scrollFrame = 0;
    rectInFrame = 0;
    frameWidth = 0;
    needTitleDisplay = 0;             /* Refresh FRAME-LABEL title after CLEAR FRAME */
    nGlobBuf= 0;                      /* Track DEF BUFFERs & FINDS moved in GUI-rpts */
    nGlobFind = 0;
    nFieldTrigs = 0;                  /* If-blocks become field triggers in GUI-rpts */
    outputOn = ! fldListMode;         /* Turns On/Off output from the TOKENIZER engine */
    TwoMany = 0;
    *tokenString = '\0';              /* hold a copy of the current token */
    *prevToken = '\0';                /* hold a copy of the previous token */
    *selprtArg = '\0';                /* hold args of {mfselprt.i ...  } */
    *gpprtrpaParams = '\0';           /* hold args for gpprtrpa.i */
    *mfguirpaParams = '\0';           /* hold args for mfguirpa.i */
    strcpy(UIlist, " ");              /* hold fields in update/set stmt */
    currentPointer = srcBuf;          /* source buffer ptrs */
    mfd = NULL;                       /* Mark the mfdeclre/mfdtitle line */
    mfd0 = NULL;
    codeStart = NULL;                 /* First non-comment line of code */
    *withName = '\0';                 /* frame name of last with clause */
    strcpy(dontRefreshTitles, ",");   /* supress this frame's title refresh */
    strcpy(nonStreamIoFrames, ",");   /* don't treat as a stream-io frame */
    strcpy(frameScope[0], "unnamed"); /* frames scoped for blocks */
    strcpy(editableDownFrames, ",");  /* scrolling down frames to get 3-D'd */
    strcpy(downFrames, ",");          /* down frames clearable w/o title */
                                      /* refresh (comma delim list of names */
                                      /* with leading & trailing commas */
    *RPMainCaller = '\0';             /* name of File continaing a hidden */
                                      /* RPMain call */
}


/*------------------------------------------------------------------------------
** Extract the next token from the source buffer.
**----------------------------------------------------------------------------*/
int getToken()
{
    int  blankLine, outputOnSave;
    char *s1, *localTokBegin; /* Hold internally for recursion */

    /* Skip past any whitespace at the current source location. */
    blankLine = 0;
    while (isspace(*currentPointer)) /* Handle special Whitespace conditions */
    {
        if (prettyMode)
        {
            /* Trim LEADING whitespace and re-indent */
            if (!lineFed || (blankLine && *currentPointer == EOLN))
            {
                putc(*currentPointer, outfile);
            }
        }
        else if (outputOn) /* Let whitespace characters pass through */
        {
            putc(*currentPointer, outfile);
        }

        if (*currentPointer == EOLN)
        {
            lineNbr++;
            blankLine = lineFed = 1;

            /* Must insert preprocessor directives here */
            if (currentPointer == codeStart)
            {
                if (guiRptMode)
                {
                    fputs(guiRptInc0, outfile);
                }
                if (rParseMode &&
                    (convertMode != CONVERT_MODE_REPORT_AND_MAINT))
                {
                    fputs(guiRptInc0a, outfile);
                }
            }

            if (currentPointer == mfd0) /* Must move mfdtitle here */
            {
                touched++;
                for (s1=mfd+1; *s1 != '\n'; s1++) putc(*s1, outfile);
                fputs(" /*GUI moved to top.*/\n", outfile);
            }
            else if (currentPointer == mfd) /* Must remove mfdtitle here */
            {
                do currentPointer++;
                while (*currentPointer != '\n');
                fputs("/*GUI moved mfdeclre/mfdtitle.*/\n", outfile);
                if (verbose)
                {
                    fprintf(logfile,"%s %d moved mfdtitle/mfdeclre\n",
                    srcName, lineNbr);
                }
                lineNbr++;
            }
        }
        currentPointer++;
    }

    /* Don't do in Pretty Mode */
    /* Check if we hit a converter directive */
    if (!prettyMode && (strncmp(currentPointer, "/*V8:", 5) == 0))
    {
        currentPointer += 5;
        if (1 == doingPrepass)
        {
            parseConverterDirective(currentPointer);
        }
        /* Find End-Comment symbol */
        currentPointer = skipComment(currentPointer);
        tokType = COMMENT;
        while (isspace(*tokBeg) || ('.' == *tokBeg))
        {
            ++tokBeg;
        }
        tokEnd = currentPointer;
        return 1;
    }

    /* Check if we hit a Conditional include section for V8 */
    if (!prettyMode && (strncmp(currentPointer, "/*V8!", 5) == 0))
    {
        if (1 == declMode)
        {
            /* Track Conditional include in declMode */
            ++touched;
        }

        /* Don't do in Pretty Mode */
        *currentPointer++ = ' ';
        *currentPointer++ = ' ';
        *currentPointer++ = ' ';
        *currentPointer++ = ' ';
        *currentPointer++ = ' ';
        s1 = skipComment(currentPointer); /* Find End-Comment symbol */
        *(--s1) = ' ' ;
        *(--s1) = ' ' ;                   /* rub it out */

        while (isspace(*currentPointer))
        {
            currentPointer++; /* skip new whitespace */
        }
            /* Continue on our merry way, now seeing the comment text as code */
    }

    /* Check if we hit a Conditional Remove section for V8 */
    if (!prettyMode && (strncmp(currentPointer, "/*V8-*/", 7) == 0))
    {
        /* Even skip this during lookahead */
        if (outputOn && verbose)
        {
           /* Don't report during look-aheads */
           fprintf(logfile, "V8 Remove COMMENT at line %d\n", lineNbr);
        }
        currentPointer += 7;
        outputOnSave = outputOn;
        outputOn = 0;
        while (1)
        {
            do
            {
                getToken();
            } while ((tokType != COMMENT) && (tokType != ENDFILE));

            if (ENDFILE == tokType)
            {
                outOfHere("EOF found searching for /*V8+*/:");
            }

            if (strncmp(tokBeg, "/*V8+*/", 7) == 0)
            {
                break;
            }
        }
        outputOn = outputOnSave;
        if (1 == declMode)
        {
            /* Track Conditional exclude in declMode */
            ++touched;
        }

        return 1;
    }

    /*
    ** Mark beginning and end of token since it can be very long, like for a
    ** comment or literal string, and it may not fit in the token buffer.
    ** In fact Comments, Strings & Inc-Arg's are not put into tokenString.
    */
    localTokBegin = currentPointer;
    switch (charCode(*currentPointer))
    {
    case LETTER:
        getWord();
        break;
    case DIGIT:
        getNumber();
        break;
    case QUOTE:
        getString();
        break;
    case EOF_CODE:
        tokType = ENDFILE;
        break;
    default:
        if (*currentPointer == '.' && charCode(*(currentPointer+1)) == DIGIT)
            getNumber();
        else
            getSpecial();
        break;
    }
    tokBeg = localTokBegin;
    tokEnd = currentPointer;

    return (tokType != ENDFILE);
}


/*------------------------------------------------------------------------------
** findStringIndex()  find index of string within an array of strings
*-----------------------------------------------------------------------------*/
static int findStringIndex(searchString, possibleValues)
char *searchString;
char *possibleValues[];
{
    int returnIndex = NOT_FOUND;
    int index = 0;

    while ((strlen(possibleValues[index]) > 0) &&
        (NOT_FOUND == returnIndex))
    {
        if (strcmp(searchString, possibleValues[index]) == 0)
        {
            returnIndex = index;
        }
        else
        {
            ++index;
        }
    }

    return (returnIndex);
}


/*------------------------------------------------------------------------------
** take special converter action based on a recognized converter keyword
**----------------------------------------------------------------------------*/
int parseConverterDirective(Directive)
char *Directive;
{
#define TMP_DIRECTIVE_LEN 128

    int returnCode = 0;
    char tmpDirective[TMP_DIRECTIVE_LEN];
    int index = 0;
    int converterDirective;

    strncpy(tmpDirective, Directive, TMP_DIRECTIVE_LEN - 2);
    for (index = 0; isalpha(tmpDirective[index]); ++index)
    {
    /* Move index to end of directive to null-terminate it. */
    }
    if (tmpDirective[index] == '=')
    {
        ++index;
    }
    tmpDirective[index] = '\0';
    converterDirective = findStringIndex(tmpDirective, converterDirectiveString);

    switch (converterDirective)
    {
    case DIRECTIVE_HIDDEN_DOWN_FRAME:
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the hidden down frame name into the "tokenString" global. */
        getWord();

        strcat(downFrames, tokenString); /* Build down frame list */
        strcat(downFrames, ",");
        break;

    case DIRECTIVE_RPMAIN_CALLER:
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the name of the file that calls RPMain. */
        getWord();

        /* Copy the RPMain calling program name to a global variable. */
        strcpy(RPMainCaller, tokenString);
        break;

    case DIRECTIVE_CONVERT_MODE:
        /* Determine the converter run mode, set the global variable. */
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the convert mode that follows the directive. */
        getWord();

        convertMode = findStringIndex(tokenString, converterModeString);
        switch (convertMode)
        {
        case CONVERT_MODE_MAINTENANCE:
        case CONVERT_MODE_REPORT:
        case CONVERT_MODE_FULLGUI_REPORT:
        case CONVERT_MODE_NO_CONVERT_NO_OUTPUT:
        case CONVERT_MODE_NO_CONVERT:
        case CONVERT_MODE_CONDITIONAL:
        case CONVERT_MODE_REPORT_AND_MAINT:
            break;

        default:
            fprintf(logfile,
                "%s %d WARN1 Unrecognized %s option\n",
                srcName,
                lineNbr,
                converterDirectiveString[DIRECTIVE_CONVERT_MODE]);
        }
        break;

    case DIRECTIVE_EDITABLE_DOWN_FRAME:
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the name of the frame whose refresh is to be supressed. */
        currentPointer = copyFrameName(tokenString, currentPointer);

        /* Copy the frame name to the global variable. */
        strcat(editableDownFrames, tokenString);
        strcat(editableDownFrames, ",");
        break;


    case DIRECTIVE_DONT_REFRESH_TITLE:
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the name of the frame whose refresh is to be supressed. */
        getWord();

        /* Copy the frame name to the global variable. */
        strcat(dontRefreshTitles, tokenString);
        strcat(dontRefreshTitles, ",");
        break;

    case DIRECTIVE_NON_STREAM_IO_FRAME:
        currentPointer = Directive +
            strlen(converterDirectiveString[converterDirective]);

        /* Read the name of the frame not to be treated as a stream-io frame. */
        getWord();

        /* Copy the frame name to the global variable. */
        strcat(nonStreamIoFrames, tokenString);
        strcat(nonStreamIoFrames, ",");
        break;

    case DIRECTIVE_CONCAT_STRING:
        concatStringMode = 1;
        break;

    default:
        returnCode = 1;
        fprintf(logfile,
            "%s %d WARN1 Unrecognized Directive\n", srcName, lineNbr);
        break;
    }

    return (returnCode);
}


/*------------------------------------------------------------------------------
**  getWord - Extract a word token from input, file it into token buffer
**----------------------------------------------------------------------------*/
void getWord()
{
    char *tokenp = tokenString;

    strcpy(prevToken, tokenString);
    while ((charCode(*currentPointer) == LETTER) ||
        (charCode(*currentPointer) == DIGIT)  ||
        (charCode(*currentPointer) == NAMECHAR) ||
        ((*currentPointer == '.') && (charCode(*(currentPointer+1)) == LETTER)))
    /* watch for compound names <datefile.field> */
    {
        *tokenp++ = *currentPointer++;
    }
    *tokenp = '\0';
    tokType = (isKeyword(tokenString)) ? KEYWORD : WORD;
}


/*------------------------------------------------------------------------------
** Extract a number token from input file it into token buffer
**----------------------------------------------------------------------------*/
void getNumber()
{
    char *tokenp = tokenString;

    strcpy(prevToken, tokenString);
    *tokenp++ = *currentPointer++; /* Careful of leading +/- */
    while ( (charCode(*currentPointer) == DIGIT)  ||
        ((*currentPointer == '.')  && charCode(*(currentPointer+1)) == DIGIT) )
    {
        *tokenp++ = *currentPointer++;
    }
    *tokenp = '\0';
    tokType = NUMBER;
}


/*-----------------------------------------------------------------------------
** test to see if we are at the beginning of (getFrameTitle("string", xx))
**----------------------------------------------------------------------------*/
int isGetFrameTitleFunctionCall(testChar, currentPointer)
char testChar;
char *currentPointer;
{
   return (
      (strncmp(
         currentPointer,
         GET_FRAME_TITLE_FUNCTION_NAME,
         strlen(GET_FRAME_TITLE_FUNCTION_NAME)
         ) == 0
      )
      &&
      ('(' == testChar)
   );
}


/*------------------------------------------------------------------------------
** Parse a PROGRESS function call that will return a string to
** be used for the frame title.
** PROGRESS code must be of the form (getFrameTitle("INVENTORY_DATA",21))
**----------------------------------------------------------------------------*/
void getGetFrameTitleFunctionCall()
{
   char *tmpPtr = NULL;
   int gotFirstQuote = 0;
   int gotSecondQuote = 0;

   /*
   ** Copy *(*getFrameTitle("INVENTORY_DATA",21)) to frame title.
   */
   strcpy(titText, tokenString);

   /*
   ** Copy (*getFrameTitle*("INVENTORY_DATA",21)) to frame title.
   */
   getToken();
   strcat(titText, tokenString);

   /*
   ** Copy (getFrameTitle*(*"INVENTORY_DATA",21)) to frame title.
   */
   getToken();
   if (strcmp("(", tokenString) == 0)
   {
      strcat(titText, tokenString);

      /*
      ** Copy (getFrameTitle(*"INVENTORY_DATA"*,21)) to frame title.
      */
      tmpPtr = currentPointer;
      while ((gotSecondQuote != 1) && (*tmpPtr != '.'))
      {
         strncat(titText, tmpPtr, 1);
         if ('"' == *tmpPtr)
         {
            if (0 == gotFirstQuote)
            {
               gotFirstQuote = 1;
            }
            else
            {
               gotSecondQuote = 1;
            }
         }
         ++tmpPtr;
      }

      /*
      ** Check to see if a second quote was found.
      */
      if (1 == gotSecondQuote)
      {
         /*
         ** Allow the global variables to catch up with the string
         ** parsing just done to extend the title text.
         */
         getToken();

         /*
         ** Copy (getFrameTitle("INVENTORY_DATA"*,*21)) to frame title.
         */
         getToken();
         if (strcmp(",", tokenString) == 0)
         {
            strcat(titText, tokenString);

            /*
            ** Copy (getFrameTitle("INVENTORY_DATA",*21*)) to frame title.
            ** Assume the next token is correct!
            */
            getToken();
            strcat(titText, tokenString);

            /*
            ** Copy (getFrameTitle("INVENTORY_DATA",21*)*) to frame title.
            */
            getToken();
            if (strcmp(")", tokenString) == 0)
            {
               strcat(titText, tokenString);
               /*
               ** Copy (getFrameTitle("INVENTORY_DATA",21)*)* to frame title.
               */
               getToken();
               if (strcmp(")", tokenString) == 0)
               {
                  strcat(titText, tokenString);
               }
               else
               {
                  outOfHere("CANNOT PARSE " GET_FRAME_TITLE_FUNCTION_NAME ": ");
               }
            }
            else
            {
               outOfHere("CANNOT PARSE " GET_FRAME_TITLE_FUNCTION_NAME ": ");
            }
         }
         else
         {
            outOfHere("CANNOT PARSE " GET_FRAME_TITLE_FUNCTION_NAME ": ");
         }
      }
      else
      {
         outOfHere("CANNOT PARSE " GET_FRAME_TITLE_FUNCTION_NAME ": ");
      }
   }
   else
   {
      outOfHere("CANNOT PARSE " GET_FRAME_TITLE_FUNCTION_NAME ": ");
   }
}


/*------------------------------------------------------------------------------
** Extract a string token from leading QUOTE on.
**----------------------------------------------------------------------------*/
int getString()
{
    char *cptr, qt_ch;

    qt_ch = *currentPointer;
    cptr = currentPointer+1;
    strcpy(prevToken, tokenString);
    *tokenString = '\0'; /* Clear Global Token String, don't fill it! */
    while( *cptr )
    {
        /* Skip over escaped quote characters, they are in the string */
        if ( (*cptr == '~' ) || ( *cptr == qt_ch && *(cptr+1) == qt_ch ) )
        {
            cptr++;
        }
        else if ( *cptr == qt_ch ) /* found end of string */
        {
            currentPointer = cptr+1; /* return position past ending quote */
            tokType = LITERAL;

            if ((1 == doingPrepass) && (1 == concatStringMode))
            {
                ++stringsToConcat;
            }
            return 0;
        }
        else if ( *cptr == EOLN )
        {
            lineNbr++;
        }
        /* else just a character in the string */
        cptr++;
    }
    outOfHere("Error in quote:");
    return (1);
}


/*------------------------------------------------------------------------------
** Extract a special token.  Comments, single-character
** and multi-character operators, and Include/Sub-Args.
**----------------------------------------------------------------------------*/
void getSpecial()
{
    char *tokenp = tokenString;

    if ( (*currentPointer == '/') && ( *(currentPointer+1) == STAR) )
    {
        /* A Comment Begin */
        currentPointer = skipComment(currentPointer+2);
        tokType = COMMENT;

        /* Clear out Global Token and Don't fill it */
        *tokenp = '\0';
    }
    else
    {
        strcpy(prevToken, tokenString);
        if ((*currentPointer == '{') && (*(currentPointer + 1) != '&'))
        {
            skipBraces();
        }
        else if ((*currentPointer == '{') && (*(currentPointer + 1) == '&'))
        {
            getPreprocessor();
        }
        else
        {
            switch (*currentPointer)
            {

            /* Single byte operators that MAY juxtapose (serve as delim) */
            case '-': /* Careful of unary plus-minus */
            case '+':
                if ( charCode(*(currentPointer+1)) == DIGIT)
                {
                    getNumber();
                    break;
                }
            case '*':
            case '/':
            case '=':

            case ';': /* This needed for unix ... commands */
            case '&': /* This needed for unix ... commands */
            case '|': /* This needed for unix ... commands */
            case '~':
            case '@':
            case '^':

            case '.':
            case ':':

            case ',':
            case '[':
            case ']':
            case '(':
            case ')':
            case '?':
                *tokenp++ = *currentPointer++;
                *tokenp = '\0';
                tokType = OPERATOR;
                break;

            case '<': /* Possible Two character operators */
            case '>':
                *tokenp++ = *currentPointer++;
                if ( *currentPointer == '=' ||  *currentPointer == '>' )
                    *tokenp++ = *currentPointer++;
                break;

            default:
                outOfHere("Unexpected operator:");
                break;
            }
        }
    }
} /* getSpecial() */


/*------------------------------------------------------------------------------
** Given a pointer into source text after a Start-Comment,
** find the end of the possibly nested comment body.
**----------------------------------------------------------------------------*/
char *skipComment(s)
char *s;
{
    int clevel; /* Comment nesting level */

    for (clevel=1; clevel; s++)
    {
        if (*s == EOLN)
        {
            lineNbr++;
        }
        else if ((*s == '/') && (*(s+1) == STAR))
        {
            clevel++;
            s++;
        }
        else if ((*s == STAR) && (*(s+1) == '/'))
        {
            clevel--;
            s++;
        }
        else if (*s == EOF_CHAR)
        {
            outOfHere("End of file within COMMENT");
        }
    }
    return (s);
}


/*------------------------------------------------------------------------------
** Called with currentPointer at a beginning Include-Brace, pass over
** the matching {}, and set currentPointer past the closing Right-Brace.
** Build the whole pp directive in tmpToken, then copy to tokenString.
**----------------------------------------------------------------------------*/
int getPreprocessor()
{
    char *pp_begin = currentPointer;
    char tmpToken[SHORT_STRING_LENGTH];

    /* first character of preprocessor token is '{' */
    strcpy(tmpToken, "{");
    currentPointer++;

    /* noted recursion here */
    getToken();
    strcat(tmpToken, tokenString);

    /* noted recursion here */
    getToken();
    strcat(tmpToken, tokenString);

    /* close the { with the matching } */
    for ( ; *(currentPointer - 1) != '}' ; currentPointer++)
    {
        /* add unexpected characters to token */
        tmpToken[strlen(tmpToken) + 1] = '\0';
        tmpToken[strlen(tmpToken)] = *currentPointer;
    }

    /* set global variables */
    strcpy(tokenString, tmpToken);
    strcpy(tmpToken, "");
    tokBeg = pp_begin;
    tokEnd = currentPointer;
    tokType = PREPROCESSOR;

    return ( *(currentPointer - 1) == '}' );
} /* end getPreprocessor() */



/*------------------------------------------------------------------------------
** Called with currentPointer at a beginning Include-Brace, pass over
** Brace scope and set currentPointer past the closing Right-Brace.
** put the include file name in tokenString, or parm-name if
** it is an include-argument.
**----------------------------------------------------------------------------*/
int skipBraces()
{
    int  nlbraces;

    nlbraces = 1;
    currentPointer++;

    /* noted recursion here */
    getToken();

    strcpy(incName, tokenString);
    if (*incName == '&')
    {
        /* get full parm name if not a number */

        /* noted recursion here */
        getToken();
        strcat(incName, tokenString);
        strcpy(tokenString,incName);
    }
    if ( mParseMode &&
	   ((!strncasecmp(incName, "mfsel", 5)) || (!strncasecmp(incName, GPSELOUT_I, strlen(GPSELOUT_I)))) &&
       (convertMode != CONVERT_MODE_REPORT_AND_MAINT))
    {
        outOfHere("REPORT CODE: ");
    }

    tokType = INCLUDE;
    while( *currentPointer )
    {
        if (*currentPointer == '{')
        {
           /* Process all sub-braces */
            nlbraces++;
        }
        else if (*currentPointer == '}')
        {
            if ( nlbraces == 1 )
            {
                /* * * * * * Should leave loop here ONLY! * * * * * */

                /* set return position  */
                currentPointer++;
                return 0;

                /* NB: could still be in quotes due to Progress accepting
                ** illegal syntax within braces when quotes are left open.
                */
            }
            nlbraces--;
        }
        else if ( *currentPointer == EOLN )
            lineNbr++;
        currentPointer++;
    } /* not End-of-Brace loop, should never terminate, returns within loop */

    outOfHere("Error in Include Braces:");
    return (1);

} /* end skipBraces() */


/*------------------------------------------------------------------------------
** Skip the rest of a statement without outputting.
** If passed GUI comment text then mark code with it,
** else if passed NULL don't comment or turn ouput back on.
** Leaves source pointer on the "." after the statement.
**----------------------------------------------------------------------------*/
void skipStatement(txt)
char *txt;
{
    outputOn = 0;
    do  getToken();
    while (isNotToken(".") && isNotToken(":") );
    if (txt != NULL)
    {
        fprintf(outfile, "/*GUI %s removed */", txt);
        outputOn = 1;
    }
}


/*------------------------------------------------------------------------------
** Skip a Progress 'BLOCK' with outputting. Pass in the Block
** Level to skip through. i=1 will skip the 'rest' of a block.
** If on the DO/REPEAT/FOR EACH/EDITING token, pass in i=0.
** Leaves source pointer on the "." after the "END".
**----------------------------------------------------------------------------*/
void skipBlock(indent_level)
int indent_level; /* Starting Indent level */
{
    do
    {
        getToken();
        if (isToken("DO") || isToken("REPEAT") || isToken("EDITING") ||
            (isToken("FOR") && isNextToken("EACH")) )
        {
            indent_level++;
        }
        else if ( isToken("END") )
        {
            indent_level--;
        }
    } while (indent_level);
    getToken(); /* skip to the '.' */
}


/*------------------------------------------------------------------------------
** isKeyword   Check to see if a word token is a keyword
**----------------------------------------------------------------------------*/
isKeyword(tok)
char *tok;
{
    int i, ch1;

    ch1 = toupper(*tok);
    if ( *tok != '_' && (i=keytab[ch1-'A']) > -1)
    {
        for (; *keywordTable[i] == ch1; i++)
        {
            if (strcasecmp(tok, keywordTable[i]) == 0)
            {
                return(1); /* yes, a keyword */
            }
        }
    }
    return(0); /* no, it's not */
}


/*------------------------------------------------------------------------------
** Look ahead to see if next token(s) is 'str'
**----------------------------------------------------------------------------*/
int isNextToken(str)
char *str;
{
   char *s1;

   s1 = tokEnd;
   /* Skip whitespace and Comments */
   while (1)
   {
      /* Skip Leading Whitespace */
      while ( isspace(*s1) )
      {
         s1++;
      }
      if ( (*s1 == '/') && ( *(s1+1) == STAR) )
      {
         /* A Comment Begin */
         s1 = skipComment(s1+2);
      }
      else
      {
         break;
      }
   }
   return ( !strncasecmp(s1, str, strlen(str)) );
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void concatStringPutToken()
{
    /* Only send strings to the output file, concatted into one
    ** giant string.
    */
    if (LITERAL == tokType)
    {
        ++touched;
        if (stringsConcated > 0)
        {
            *tokBeg = ' ';
        }
        ++stringsConcated;

        if (stringsConcated < stringsToConcat)
        {
            --tokEnd;
        }

        for (; tokBeg < tokEnd; tokBeg++)
        {
            /* Spit out token*/
            putc(*tokBeg, outfile);
        }
    }
    else if (0 == stringsConcated)
    {
        for (; tokBeg < tokEnd; tokBeg++)
        {
            /* Spit out token*/
            putc(*tokBeg, outfile);
        }
    }
    else
    {
        ++touched;
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void putToken()
{
    if (1 == concatStringMode)
    {
        concatStringPutToken();
    }
    else
    {
        for (; tokBeg < tokEnd; tokBeg++)
        {
            /* Spit out token*/
            putc(*tokBeg, outfile);
        }
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void setFrameScope()
{
   if (*withName)
   {
      strcpy(frameScope[indLevel], withName);
   }
   else if (newBlock == 1)
   {
      /* with default frame scoping */
      strcpy(frameScope[indLevel], "unnamed");
   }
   else if (newBlock == 2)
   {
      /* inherit parent frame scope */
      strcpy(frameScope[indLevel], frameScope[indLevel - 1]);
   }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
int setErrEvent(thisLevel)
int thisLevel; /* Indent level of Block needing TRAPping */
{
    int i;

    /* Nothing to do at external level */
    if (thisLevel == 0)
    {
        return 0;
    }

    /* Find the Block descriptor for this current level block */
    for (i=iBlock; blck[i].indlev != thisLevel; i--)
    {
       ;
    }

    /* Current Block */
    blck[i].errevt = 1;

    /* Work backwards thru all block nested within same outer block */
    for (; blck[i].indlev > 1; i--)
        if (blck[i].indlev < thisLevel) /* Skip sibling blocks and their kids */
        {
            blck[i].errevt = 1;
            thisLevel = blck[i].indlev;
        }

    /* Block at base indent level */
    blck[i].errevt = 1;

    return 0;
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
checkBlock(thisLevel)
int thisLevel; /* Indent level of Block needing TRAPping */
{
    int i;

    /* Find the Block descriptor for this current level block */
    for (i=iBlock; blck[i].indlev != thisLevel; i--)
    {
       ;
    }

    return (blck[i].errevt && blck[i].erract);
}


/*------------------------------------------------------------------------------
** Parse a Data Entry statement (set/update/prompt-for). 
**----------------------------------------------------------------------------*/
int parseDE()
{
    int firstWord;
    char *s1;

    /* Check first possible field/var for 'recname EXCEPT' */
    firstWord = 1;
    do
    {
        getToken();
        if (tokType == WORD)
        {
            /* Write token to list */
            if (firstWord && isNextToken("EXCEPT"))
            {
                fprintf(logfile,"%s %d WARN3 EXCEPT used\n", srcName, lineNbr);
                return (0);
            }
            if (isNextToken("="))
            {
                /* ' update company = "" '  type syntax */
                fprintf(logfile,"%s %d WARN1 SKIPPING update-assign\n",
                    srcName, lineNbr);

                /* DON'T continue with this update statement */
                return (0);
            }
            /* Print Field<tab>Source-file without and datafile prefix */
            if ((s1=strrchr(tokenString, '.')) == NULL)
            {
                s1 = tokenString;
            }
            else
            {
                s1++;
            }
            fprintf(outfile, "%s\t%s\n", s1, srcName);
        }
        else if (tokType == KEYWORD) /* Take action on some keywords */
        {
            if ( isToken("GO-ON") || isToken("WITH") || isToken("EDITING") )
            {
                /* Can stop now, the rest of statement is fluff */
                return (0);
            }

            if ( isToken("STREAM") || isToken("LIKE") )
            {
                /* Waste the next token, it is a stream/fld name */
                getToken();
            }
            else if ( isToken("WHEN") )
            {
                if (!isNextToken("("))
                {
                    /* A More complex expression, do by hand */
                    fprintf(logfile,"%s %d Open WHEN expression\n", srcName, lineNbr);
                    for (currentPointer--;
                       *currentPointer && *currentPointer!='\n';
                       currentPointer++)
                    {
                       /* Skip rest of line */
                       ;
                    }
                }
            }
        }
        else if ( isToken("(") ) /* Skip paren'ed expressions */
        {
            for (plevel=1; plevel>0; )
            {
                getToken();
                if ( isToken("(") )
                {
                    plevel++;
                }
                else if ( isToken(")") )
                {
                    plevel--;
                }
            }
        }
        else if (tokType == INCLUDE)
        {
            fprintf(logfile,"%s %d Unknown INCLUDE-ARG\n", srcName,lineNbr);
        }
    } while ( strcmp(tokenString, ".") );
    return (0);
} /* end parseDE() */


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
int countEOLNs(start)
char *start;
{
    int count = 1;
    char *current = start;

    for (count = 1, current = start;
        *current != EOF_CHAR;
        ++current)
    {
        if (EOLN == *current)
        {
            ++count;
        }
    }

    return (count);
}


/*------------------------------------------------------------------------------
** Routines used in Full Gui Report Coversion.
**----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
** Examine all IF statements in the  Field Validation Section of a report.
** If they are followed by a DO block containing a NEXT-PROMPT, then mark
** them to become Field Triggers, and skip past the IF and it's DO block
** including any ELSE clause, suppressing the entire code snippet.
**----------------------------------------------------------------------------*/
void checkForTrigger()
{
    char *anchor; /* an anchor for lookaheads */
    int  i, gotTrigger, lineNbrSave;

    /* Mark our position and silently do some look-ahead */
    anchor = currentPointer;
    outputOn = 0;
    lineNbrSave = lineNbr;
    gotTrigger = 0;
    do
    {
        getToken();
        if (isToken("DO"))
        {
           /* hit DO before '.' */
            if (nFieldTrigs == 9)
            {
               /* Verify its a good trigger */
               outOfHere("Too many Field Validations:");
            }

            /* Find field-name for trigger attachment */


            /* Block level 1, inside DO block */
            i = 1;

            /* Skip to nearest NEXT-PROMPT */
            do
            {
                /* Must find one within this block-level! */
                getToken();
                if (isToken("DO") || isToken("REPEAT") ||
                    (isToken("FOR") && isNextToken("EACH")) )
                {
                    i++;
                }
                else if ( isToken("END") )
                {
                    i--;
                }
            } while (i && isNotToken("NEXT-PROMPT") );

            if (i == 0)
            {
                /* End of DO block, not a Field Validation */
                break;
            }
            getToken();
            strcpy(fieldTrigNames[nFieldTrigs], tokenString);
            fieldTrigs[nFieldTrigs][0] = anchor; /* After the 'if' */
            skipBlock(i);
            if (isNextToken("ELSE DO"))
            {
                /* Put else clause in trigger too! */
                skipStatement(NULL);
                skipBlock(1);
            }
            else if (isNextToken("ELSE"))
            {
                skipStatement(NULL);
            }
            /* end of trigger */
            fieldTrigs[nFieldTrigs++][1] = currentPointer;
            gotTrigger = 1;
        }
    } while (isNotToken(".") &&  isNotToken(":") && (!gotTrigger));
    if (!gotTrigger)
    {
        /* Not a field trigger so spit it out */
        fputs("if ", outfile);
        currentPointer = anchor;
        lineNbr = lineNbrSave;
        /* contine to write out the rest of the code */
    }
    else if (verbose)
    {
        fprintf(logfile,"%s %d Field Validation => Trigger\n", srcName, lineNbr);
    }
    outputOn = 1;
} /* checkForTrigger() */


/*------------------------------------------------------------------------------
** Have found a global Find above the report generating block of a
** report, mark it to be replicated in the report block.
**----------------------------------------------------------------------------*/
void guiPreFind()
{
   if (nGlobFind == 9)
   {
      outOfHere("Too many global finds:");
   }
   globFind[nGlobFind++] = tokBeg;

   while ( isNotToken(".") )
   {
      putToken();
      getToken();
   }
   putToken();
}


/*------------------------------------------------------------------------------
** Have found a BUFFER define above the report generating block of a
** report, move it to report block.
**----------------------------------------------------------------------------*/
void guiPreBuffer()
{
   if (nGlobBuf == 9)
   {
      outOfHere("Too many global buffers:");
   }
   globBuf[nGlobBuf++] = tokBeg;
   outputOn = 0;
   while ( isNotToken(".") )
   {
      getToken();
   }
   outputOn = 1;
}


/*------------------------------------------------------------------------------
** Create field validation trigger procedures
**----------------------------------------------------------------------------*/
void guiWriteTriggers()
{
   int lineNbrSave, i;
   char *anchor;

   anchor = currentPointer;
   lineNbrSave = lineNbr;
   fputs("/*GUI - Field Trigger Section */\n\n", outfile);
   for (i=0; i<nFieldTrigs; i++)
   {
      fprintf(outfile, "on leave of %s do:\nassign %s.\nif",
         fieldTrigNames[i], fieldTrigNames[i]);
      currentPointer = fieldTrigs[i][0];
      do
      {
          getToken();
          if (isToken("NEXT-PROMPT"))
          {
             skipStatement("NEXT-PROMPT");
          }
          else if (isToken("UNDO"))
          {
             skipStatement("UNDO");
             fputs(" Validation-Error = yes.", outfile);
             fputs(" RETURN NO-APPLY.", outfile);
          }
          else
          {
             putToken();
          }
      } while (currentPointer < fieldTrigs[i][1]);
      fputs("\nend.\n", outfile);
   }
   currentPointer = anchor;
   lineNbr = lineNbrSave;
}


/*------------------------------------------------------------------------------
** Send string to output file, until end of statement.
**----------------------------------------------------------------------------*/
void printToDot(startLoc, outfile)
char *startLoc;
FILE *outfile;
{
   char *s1 = startLoc;
   char *next = s1 + 1;
   int done = 0;

   while (! done )
   {
      if ((*s1 == '.') && isspace(*next))
      {
         /*
         ** Assume "." followed by whitespace is the end of the statement.
         */
         done = 1;
      }
      else
      {
         putc(*s1, outfile);
         ++s1;
         ++next;
      }
   }
   fputs(".\n", outfile);
}


/*------------------------------------------------------------------------------
** Replicate, DEFINE BUFFERs & FINDs etc, in the Report Block
**----------------------------------------------------------------------------*/
void guiDupThings()
{
   int i;
   char *s1;

   for (i=0; i<nGlobFind; i++)
   {
      printToDot(globFind[i], outfile);
   }
   putc('\n', outfile);
   for (i=0; i<nGlobBuf; i++)
   {
      printToDot(globBuf[i], outfile);
   }
   putc('\n', outfile);
}


/*------------------------------------------------------------------------------
** Have found first repeat in a report, comment it out and start
** a procedure for the report UI. Careful of unexpected code.
**----------------------------------------------------------------------------*/
void guiUI()
{
   if (indLevel > 0)
   {
      outOfHere("First REPEAT nested:");
   }
   fprintf(outfile, guiRptInc1, mfguirpaParams);
   fputs("/*GUI repeat ", outfile);
   getToken();
   if ( isNotToken(":") )
   {
      /* Not just a simple 'repeat:' so WARN */
      /* with frame A is OK */
      fprintf(logfile,"%s %d WARN1 MAIN Repeat Clause:\n", srcName, lineNbr);
      do
      {
         /* spit out repeat clause */
         fputs(tokenString,logfile);
         putc(' ', logfile);
         putToken();
      }
      while (getToken() != ENDFILE && isNotToken(":")) ;
      {
         ;
      }
      putc('\n', logfile);
   }
   fputs(": */\n/*GUI*/ procedure p-enable-ui:\n", outfile);
   suppressRepeat = 1;
   firstRepeat = 0;
   Section = UIsetup;
}


/*------------------------------------------------------------------------------
** Parse the Data-Entry statement (update/set/prompt-for) in FRAME A of
** a report being changed to a full GUI report. Drop WHENs.
**----------------------------------------------------------------------------*/
void guiDE()
{
   int saveFlag = outputOn;

   if (++TwoMany > 1)
   {
      outOfHere("Too many UPDATE/SET/PROMPT-FORs");
   }
   if (indLevel > 1)
   {
      outOfHere("Bad UI nesting:");
   }

   outputOn = 0;
   /* Build up a list of the fields in the Data-Entry statement */
   while (getToken() && isNotToken(".") && isNotToken("EDITING")
      && isNotToken("WITH") )
   {
      if (isToken("WHEN"))
      {
         /* WHEN condition must be a field or in ()'s */
         getToken();
      }
      else
      {
         strcat(UIlist, tokenString);
      }
      strcat(UIlist, " ");
   }

   /* Skip to the DE statement terminator, ABORT on EDITING Clauses */
   do
   {
      if (isToken("EDITING"))
      {
         indLevel++;
         outputOn = 0;
         skipBlock(1);
         outputOn = 1;
      }
   } while (isNotToken(".") && getToken()) ;

   outputOn = saveFlag;
   /* END the p-enable-ui() proc.  */
   fputs(guiRptInc2, outfile);

   /* Open the Definition of quote proc */
   fputs(guiRptInc3, outfile);
   Section = GeneralVals;
   touched++;
   if (verbose)
   {
      fprintf(logfile,"%s %d DE statement replaced\n", srcName, lineNbr);
   }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
is_gpquote(tokenPoint)
char *tokenPoint;
{
    char *s;

    if ( (s = strchr(tokenPoint, '\"')) == NULL)
    {
       return 0;
    }
    return ( ! strncmp(s+2, "gpquote.p", 9) );
}


/*------------------------------------------------------------------------------
** Swap {mfrpexit.i} with GUI stuff in full GUI reports.
**  get the possible argument list from the trailing curly-brace backwards.
**----------------------------------------------------------------------------*/
void guiRpexit()
{
    char *s, *t, *stop;
    static char errorMessage[40];

    t = argBuf + ARGBUF_LEN - 1;
    *t = '\0';
    /* Build up mfrpexit.i parameters into tail end of argBuf */
    if ( (stop = strstr(tokBeg, ".i")) != NULL)
    {
        stop += 1;
        for (s = tokEnd - 1; (s > stop) && (t >= argBuf); )
        {
           *--t = *s--;
        }
        if (t < argBuf)
        {
            sprintf(errorMessage, "mfrpexit param longer than %d", ARGBUF_LEN);
            outOfHere(errorMessage);
        }
        fprintf(outfile, guiRptInc6, t);
    }
}


/*------------------------------------------------------------------------------
** Swap {mfrpchk.i} with GUI stuff in full GUI reports.
**  get the possible argument list from the trailing curly-brace backwards.
**----------------------------------------------------------------------------*/
void guiRpchk()
{
   char *s, *t, *stop;
   static char errorMessage[40];

   t = argBuf + ARGBUF_LEN - 1;
   *t = '\0';
   if ( (stop = strstr(tokBeg, ".i")) != NULL)
   {
      stop += 1;
      /* Build up mfrpchk.i parameters into tail end of argBuf */
      for (s = tokEnd - 1; (s > stop) && (t >= argBuf); )
      {
         *--t = *s--;
      }
      if (t < argBuf)
      {
         sprintf(errorMessage, "mfrpchk param longer than %d", ARGBUF_LEN);
         outOfHere(errorMessage);
      }
      fprintf(outfile, guiRptInc7, t);
   }
}


/*------------------------------------------------------------------------------
**                            ERROR EXIT ROUTINE
** Abort this conversion due to stated reason, displaying the rest of the
** current line, and remove any half-finished output file.
**----------------------------------------------------------------------------*/
void outOfHere(str)
char *str; /* message telling why aborting */
{
    char typ;

    /* Show source file type, M=maintenance, B=batchable report, N=inquiry */
    if (mParseMode)
    {
        typ = 'M';
    }
    else
    {
        typ = (isBatchable) ? 'B' : 'N';
    }
    fprintf(logfile,"%s %d (%c) QUIT %s",srcName, lineNbr, typ, str);
    for (; *currentPointer && *currentPointer!='\n'; currentPointer++)
    {
       putc(*currentPointer,logfile) ;
    }
    putc('\n', logfile);
    if ( ! fldListMode )
    {
        fclose(outfile);
        unlink(outfilePath);
    }

    /* Can't call longjmp() if the stack pointer, etc haven't been set. */
    if (jmp_bufWasSet)
    {
        /* This will jump back to File-loop in main() */
        longjmp(errRestart,1);
    }
    else
    {
        /* This quits the program. */
        exit (1);
    }
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
void writeFormStmt(anchor)
char **anchor;
{
    int gotNoBox = 0;

    /* set preprocessor directive if this is frame 'A'. */
    if (strcasecmp(frameName,"A") == 0)
    {
        fputs(guiFrameADefine, outfile);
    }

    fputs("FORM /*GUI*/ ", outfile);

    if (CONVERT_MODE_REPORT_AND_MAINT == convertMode)
    {
        /* BEGIN- MASSAGE FOR USER-INPUT FRAMES */
        if (verbose)
        {
            fprintf(logfile, "FORM 3Ded  at line %d\n", lineNbr);
        }
        if (rParseMode && (!noOverlay) && verbose)
        {
            fprintf(logfile,
                "Report OVERLAY Form at line %d\n", lineNbr);
        }

        /* Lookahead one token past comments for a SKIP w/o ()'s */
        while ( getToken() && tokType == COMMENT)
        {
            putToken();
        }
        if (noColLabels && !gotDown)
        {
            fputs(titleInFrame ? condTitRectFrame : condNoTitRectFrame,
                outfile);
        }

        if (isToken("SKIP") && ( ! isNextToken("(")) )
        {
            getToken(); /* It's useless so skip it */
        }

        /* spit out form-items and WTIH clause except title */
        do
        {
            putToken(); /* write out that lookahead we did */
            getToken(); /* get the next one and examine it */

            /* Track that we got a NO-BOX token so it won't be dup'd. */
            if ( isToken("NO-BOX") )
            {
                gotNoBox = 1;
            }

            if (isToken("WITH") && !gotWith)
            {
                /* end-of-form maintenance / full gui report code */
                if ((mParseMode || guiRptMode) &&
                    noColLabels &&
                    !gotDown)
                {
                    /* watch out for 2-withs in a frame-phrase */
                    fputs(condMaintRightMarginAdj, outfile);
                    gotWith = 1;
                }
            }
            else if ( isToken("TITLE")  && noColLabels && !gotDown)
            {
                (*anchor) = currentPointer;
                /* Conditionally put out the rest of the WITH clause,
                ** including the title.
                */
                fputs(CONDITIONAL_INCLUDE_REPORT_NOT_A_TEST, outfile);
                do
                {
                    putToken(); /* write out that lookahead we did */
                    getToken(); /* get the next one and examine it */
                } while ( isNotToken("."));
                fputs("\n" CONDITIONAL_END, outfile);
                currentPointer = (*anchor);

                outputOn = 0;
                do  getToken();
                while (tokType == COMMENT);

                /* watch out for "color normal" */
                if ( isToken("COLOR") )
                {
                    do getToken();
                    while (tokType == COMMENT); /* normal */
                    do getToken();
                    while (tokType == COMMENT); /* 'title' */
                }
                if (isGetFrameTitleFunctionCall(*tokBeg, currentPointer))
                {
                   getGetFrameTitleFunctionCall();
                }
                else if (tokType == LITERAL ||
                    tokType == WORD ||
                    tokType == PREPROCESSOR)
                {
                    /* text of title or char-variable */
                    for ((*anchor)=titText; tokBeg<tokEnd;)
                    {
                        *(*anchor)++ = *tokBeg++;
                    }
                    *(*anchor) = '\0';
                }
                else
                {
                    /* Complex titles hand-done. Bad for xLate!! */
                    outOfHere("Complex Title ");
                }
                outputOn = 1;
                getToken();
            }
        } while ( isNotToken("."));

        if (!frameWidth)
        {
            if ((rParseMode || mParseMode) && noOverlay)
            {
                touched++;
                fputs(condWidth80, outfile);
                if (verbose)
                {
                    fprintf(logfile,"%s %d Frame WIDENED to 80\n",
                        srcName, lineNbr);
                }
            }
            else
            {
                fprintf(logfile,"%s %d WARN1 Framed un-WIDTHED\n",
                    srcName, lineNbr);
            }
        }

        /* end-of-form report code */
        if (strcasecmp(frameName,"A") &&
            noOverlay &&
            (frameNameFoundInList(frameName, nonStreamIoFrames) == 0))
        {
            fputs(condStreamIO, outfile);
        }

        if (noColLabels && !gotDown)
        {
            touched++;
            if (gotNoBox)
            {
                fputs(condThreeD, outfile);
            }
            else
            {
                fputs(condNoBox3D, outfile);
            }

            rectInFrame = 1;
            if (titleInFrame)
            {
                fprintf(outfile,
                    condTitRectAdj, frameName, frameName, titText,
                    frameName, frameName, frameName, frameName,
                    frameName, frameName, frameName, frameName,
                    frameName);
            }
            else
            {
                fprintf(outfile,
                    condNoTitRectAdj, frameName, frameName, frameName,
                    frameName, frameName, frameName, frameName,
                    frameName, frameName);
            }
        }
        else /* Mostly choose-frames and stat windows */
        {
            /* Determine if this is a scrolling down frame
            ** (a choose frame) and needs BGCOLOR 8, or
            ** if this is an editable down frame and gets
            ** THREE-D.
            */
            if ((scrollFrame) &&
                (frameNameFoundInList(frameName,
                editableDownFrames) == 0))
            {
                fprintf(logfile,"%s %d WARN1 SCROLL Frame\n",
                    srcName, lineNbr);
                touched++;
                fputs(condBGColor8, outfile);
            }
            else
            {
                touched++;
                fputs(condThreeD, outfile);
            }
            strcat(downFrames, frameName); /* Build dnframe list */
            strcat(downFrames, ",");
        }

            /* END- MASSAGE FOR USER-INPUT FRAMES */
    }
    else
    /* Not ReportAndMaintenance convert mode */
    {
        if (rParseMode && strcasecmp(frameName,"A") && noOverlay &&
            (frameNameFoundInList(frameName, nonStreamIoFrames) == 0))
        {
            /* STREAM-IO rpt output frames, "A" & overlays are input */
            if (verbose)
            {
                fprintf(logfile, "FORM STREAM-IOed at line %d\n", lineNbr);
            }
            do /* spit out all form-items adding to the  WTIH clause. */
            {
                /* get the next one and examine it */
                getToken();
                putToken();
                if (isToken("WITH") && !gotWith)
                {
                    /* watch out for 2 'with's in a frame-phrase */
                    fputs(streamIO, outfile);
                    gotWith = 1;
                }
            } while ( isNotToken("."));
        }
        else /* THIS IS THE MASSAGE FOR USER-INPUT FRAMES */
        {
            if (verbose)
            {
                fprintf(logfile,"FORM 3Ded  at line %d\n", lineNbr);
            }
            if (rParseMode && (!noOverlay) && verbose)
            {
                fprintf(logfile, "Report OVERLAY Form at line %d\n", lineNbr);
            }

            /* Lookahead one token past comments for a SKIP w/o ()'s */
            while ( getToken() && tokType == COMMENT)
            {
                putToken() ;
            }
            if (noColLabels && !gotDown)
            {
                fputs(titleInFrame ? titRectFrame : notitRectFrame,
                    outfile);
            }

            if (isToken("SKIP") && ( ! isNextToken("(")) )
            {
                getToken(); /* It's useless so skip it */
            }

            /* spit out form-items and WTIH clause except title */
            do
            {
                putToken(); /* write out that lookahead we did */
                getToken(); /* get the next one and examine it */

                /* Track that we got a NO-BOX token so it won't be dup'd. */
                if ( isToken("NO-BOX") )
                {
                    gotNoBox = 1;
                }

                if ( isToken("WITH") && noColLabels && !gotWith &&
                    !gotDown && (mParseMode || guiRptMode) )
                {
                    /* watch out for 2-withs in a frame-phrase */
                    fputs(MAINT_RIGHT_MARGIN_ADJ, outfile);
                    gotWith = 1;
                }
                else if ( isToken("TITLE")  && noColLabels && !gotDown)
                {
                    /* Skip the whole title clause, inc color phrase */
                    outputOn = 0; /* Run silent */
                    do  getToken();
                    while (tokType == COMMENT);

                    /* watch out for "color normal" */
                    if ( isToken("COLOR") )
                    {
                        do getToken();
                        while (tokType == COMMENT); /* normal */
                        do getToken();
                        while (tokType == COMMENT); /* 'title' */
                    }
                    if (isGetFrameTitleFunctionCall(*tokBeg, currentPointer))
                    {
                       getGetFrameTitleFunctionCall();
                    }
                    else if (tokType == LITERAL ||
                        tokType == WORD ||
                        tokType == PREPROCESSOR)
                    {
                        /* text of title or char-variable */
                        for ((*anchor)=titText; tokBeg<tokEnd;)
                            *(*anchor)++ = *tokBeg++;
                        *(*anchor) = '\0';
                    }
                    else
                    {
                        /* Complex titles hand-done. Bad for xLate!! */
                        outOfHere("Complex TITLE ");
                    }
                    outputOn = 1;
                    getToken();
                }
            } while ( isNotToken("."));

            if (!frameWidth)
            {
                if (rParseMode && noOverlay)
                {
                    touched++;
                    fputs(width80, outfile);
                    if (verbose)
                    {
                        fprintf(logfile,"%s %d Frame WIDENED to 80\n",
                           srcName, lineNbr);
                    }
                }
                else
                {
                    fprintf(logfile,"%s %d WARN1 Framed un-WIDTHED\n",
                        srcName, lineNbr);
                }
            }

            if (noColLabels && !gotDown)
            {
                touched++;
                if (gotNoBox)
                {
                    fputs(threeD, outfile);
                }
                else
                {
                    fputs(noBox3D, outfile);
                }

                rectInFrame = 1;
                if (titleInFrame)
                {
                    fprintf(outfile,
                    titRectAdj, frameName, frameName, titText,
                    frameName, frameName, frameName, frameName,
                    frameName, frameName, frameName, frameName,
                    frameName);
                }
                else
                {
                    fprintf(outfile,
                    notitRectAdj, frameName, frameName, frameName,
                    frameName, frameName, frameName, frameName,
                    frameName, frameName);
                }
            }
            else /* Mostly choose-frames and stat windows */
            {
                /* Determine if this is a scrolling down frame
                ** (a choose frame) and needs BGCOLOR 8, or
                ** if this is an editable down frame and gets
                ** THREE-D.
                */
                if ((scrollFrame) &&
                    (frameNameFoundInList(frameName,
                    editableDownFrames) == 0))
                {
                    fprintf(logfile,"%s %d WARN1 SCROLL Frame\n",
                        srcName, lineNbr);
                    touched++;
                    fputs(bgColor8, outfile);
                }
                else
                {
                    touched++;
                    fputs(threeD, outfile);
                }
                /* Build dnframe list */
                strcat(downFrames, frameName);
                strcat(downFrames, ",");
            }
        } /* End massage user-input frames */
    }

    /* unset preprocessor directive if this is frame 'A'. */
    if (strcasecmp(frameName,"A") == 0)
    {
        fputs(guiFrameAUndefine, outfile);
    }
}


/******************************************************************************/
/*
#define DZN_DEBUG 1
*/
#if DZN_DEBUG

/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
char * getTokTypeAsString(tokType)
int tokType;
{
    char tokTypeAsString[64];

    switch (tokType)
    {
    case ENDFILE:
        sprintf(tokTypeAsString,"%d-EOF", tokType);
        break;
    case COMMENT:
        sprintf(tokTypeAsString,"%d-COMMENT", tokType);
        break;
    case LITERAL:
        sprintf(tokTypeAsString,"%d-LITERAL", tokType);
        break;
    case WORD:
        sprintf(tokTypeAsString,"%d-WORD", tokType);
        break;
    case KEYWORD:
        sprintf(tokTypeAsString,"%d-KEYWORD", tokType);
        break;
    case NUMBER:
        sprintf(tokTypeAsString,"%d-NUMBER", tokType);
        break;
    case OPERATOR:
        sprintf(tokTypeAsString,"%d-OPERATOR", tokType);
        break;
    case INCLUDE:
        sprintf(tokTypeAsString,"%d-INCLUDE", tokType);
        break;
    case PREPROCESSOR:
        sprintf(tokTypeAsString,"%d-PREPROCESSOR", tokType);
        break;
    default:
        sprintf(tokTypeAsString,"%d-???", tokType);
        break;
    }

    return (/* tokTypeAsString */ "move cmt markers to cmt out this string");
}


/*------------------------------------------------------------------------------
**
**----------------------------------------------------------------------------*/
dzn_debug_token_check(destination)
FILE *destination;
{
#define PEEK_AREA_LENGTH 16
    char head[PEEK_AREA_LENGTH];
    char tail[PEEK_AREA_LENGTH];
    char current[PEEK_AREA_LENGTH];
    long tokenLength = PEEK_AREA_LENGTH - 1;

    if ((tokEnd - tokBeg) > 0)
    {
       if ((tokEnd - tokBeg) < PEEK_AREA_LENGTH - 1)
       {
           tokenLength = tokEnd - tokBeg;
       }

       strncpy(head, tokBeg, tokenLength);
       head[tokenLength] = '\0';
       strncpy(tail, (char *) (tokEnd - tokenLength), tokenLength);
       tail[tokenLength] = '\0';
       strncpy(current, currentPointer, PEEK_AREA_LENGTH - 1);
       current[PEEK_AREA_LENGTH - 1] = '\0';

       fprintf(destination, "\n");
       fprintf(destination, "tokBeg(%p)=>%s<\n",tokBeg, head);
       fprintf(destination, "tokEnd(%p)=>%s<\n",tokEnd, tail);
       fprintf(destination, "tokLen=%d\n", tokEnd - tokBeg);
       fprintf(destination, "currentPointer(%p)=>%s<\n", currentPointer, current);
    }
    else
    {
       fprintf(destination, "tokBeg = %p, tokEnd = %p ???\n", tokBeg, tokEnd);
    }

    if (destination == stdout)
    {
        fprintf(destination, "press \"Enter\"?");
        getchar();
    }
}
#endif /*DZN_DEBUG*/

/* end of file convert.c */
