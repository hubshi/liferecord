DVMP DEFINE VARIABLE %\c AS MEMPTR    NO-UNDO.
IOPWH DEFINE INPUT-OUTPUT PARAMETER %\c AS WIDGET    NO-UNDO.
IPHN DEFINE INPUT  PARAMETER %\c AS HANDLE    NO-UNDO.
ONB ON '%\c':U OF %\S
    DO:
    %\iRETURN.
    END.
DWF DO WITH FRAME %\c:
    %\i
    END.
MUTSEL(tabName "Table Name" tmp_table
       browseName "Browse Name" brList
       frameName "Frame Name" fmain
       )
 DEFINE VARIABLE ibrowse       AS INTEGER NO-UNDO.
 DEFINE VARIABLE method-return AS LOGICAL NO-UNDO.
 OPEN QUERY querybrowse1 FOR EACH %(tabName) no-lock.
 DO ibrowse = 1 TO  %(browseName):NUM-SELECTED-ROWS IN FRAME %(frameName) :
     method-return = %(browseName):FETCH-SELECTED-ROW(ibrowse).
     %\c
 END.
IPIN DEFINE INPUT  PARAMETER %\c AS INTEGER   NO-UNDO.
DWH DO WHILE (%\c):
    %\i
    END.
PSU PUT STREAM %\c UNFORMATTED
    %\iSKIP.
CIMBD DEFINE VARIABLE fname AS CHARACTER NO-UNDO.
      DEFINE VARIABLE msage AS CHARACTER NO-UNDO.
      DEFINE VARIABLE fid   AS INTEGER  NO-UNDO.
      DEFINE VARIABLE tid   AS INTEGER  NO-UNDO.
      DEFINE VARIABLE ret   AS LOGICAL  NO-UNDO.
      %\S
      {gprun.i ""hkbdld.p"" "(INPUT fname, OUTPUT fid, OUTPUT tid)"}
      {gprun.i ""hkbdgo.p"" "(INPUT fid,INPUT tid, OUTPUT ret, OUTPUT msage)"}
      %\S
      IF ret THEN DO:
         %\c
      END.
      ELSE DO:
      %\S
      END.
DVL DEFINE VARIABLE %\c LIKE NO-UNDO.
OPMP DEFINE OUTPUT PARAMETER %\c AS MEMPTR    NO-UNDO.
DIW display %\c with width 200 stream-io. %\S
IPLG DEFINE INPUT  PARAMETER %\c AS LOGICAL   NO-UNDO.
DVDT DEFINE VARIABLE %\c AS DATE      NO-UNDO.
GETTABLE(TableName "Table's Name" _det
         DataBaseName "DataBase Logical Name" qaddb
         outputto "out put to" clipboard
         )
 OUTPUT TO "%(outputto)".
 FOR EACH %(DataBaseName)._FILE NO-LOCK WHERE _FILE-NAME = "%(TableName)":
     FOR EACH %(DataBaseName)._FIELD OF %(DataBaseName)._FILE BY _ORDER:
         DISPLAY _ORDER _FIELD-NAME _FORMAT _EXTENT _DATA-TYPE _INITIAL _LABEL _COL-LABEL _DESC WITH WIDTH 260 STREAM-IO.
     END.
     FOR EACH %(DataBaseName)._index WHERE %(DataBaseName)._index._file-recid = RECID(%(DataBaseName)._file):
          FOR EACH %(DataBaseName)._index-field NO-LOCK WHERE %(DataBaseName)._index-field._index-recid = RECID(%(DataBaseName)._index) %\S
             BREAK BY %(DataBaseName)._index._file-recid:
             IF FIRST-OF(%(DataBaseName)._index._file-recid) THEN
             DISPLAY %(DataBaseName)._index._Index-Name
                     yes WHEN recid(%(DataBaseName)._index) = _Prime-Index LABEL "Primary-idx"
                     %(DataBaseName)._index._Unique
                     %(DataBaseName)._index._Idxowner %\S
                     %(DataBaseName)._index._Active.
             FIND FIRST %(DataBaseName)._field NO-LOCK WHERE recid(%(DataBaseName)._field) = %(DataBaseName)._index-field._field-recid NO-ERROR.
             DISPLAY %(DataBaseName)._index-field._Index-Seq   %\S
                     %(DataBaseName)._field._field-name
                     %(DataBaseName)._index-field._Ascending WITH WIDTH 182 STREAM-IO.
          END.
     END.
 END.
 OUTPUT CLOSE.
CDI COLOR DISPLAY INPUT %\c .
DVRI DEFINE VARIABLE %\c AS ROWID     NO-UNDO.
PUS PUT UNFORMAT %\c SKIP.
OUT(tableName "Table Name" tmp_tab
    )
 output to %\c.
 for each %(tableName) no-lock where:
     display %(tableName) with width 200 stream-io.
 end.
 output close.
IPCH DEFINE INPUT  PARAMETER %\c AS CHARACTER NO-UNDO.
IPWH DEFINE INPUT  PARAMETER %\c AS WIDGET    NO-UNDO.
IPDE DEFINE INPUT  PARAMETER %\c AS DECIMAL   NO-UNDO.
&IFND &IF (DEFINED %\c = 0) &THEN
      &ENDIF
DI DISPLAY %\c
CDM COLOR DISPLAY MESSAGE %\c .
CIMIN(cimfile "CIM-LOAD file name" cimfile
      programfile "programfilename" rcssmt.p
      )
 define variable %(cimfile) as character no-undo.
 %\S
 batchrun  = yes.
 input from value(%(cimfile)).
 output to value(%(cimfile) + ".out") keep-messages.
 hide message no-pause.
 {gprun.i ""%\c%(programfile)""}
 hide message no-pause.
 output close.
 input close.
 batchrun  = no.
PXMSG {pxmsg.i &MSGNUM=%\C &ERRORLEVEL=3}
OPDT DEFINE OUTPUT PARAMETER %\c AS DATE      NO-UNDO.
DVHN DEFINE VARIABLE %\c AS HANDLE    NO-UNDO.
OPRI DEFINE OUTPUT PARAMETER %\c AS ROWID     NO-UNDO.
CMT /* %\c */
IOPCH DEFINE INPUT-OUTPUT PARAMETER %\c AS CHARACTER NO-UNDO.
IOPDE DEFINE INPUT-OUTPUT PARAMETER %\c AS DECIMAL   NO-UNDO.
DVIN DEFINE VARIABLE %\c AS INTEGER   NO-UNDO.
DIC(col "How many columns?????" 2
    )
 DISPLAY %\c  WITH %(col) COLUMNS .
DC(col "Cloumns" 4
   )
 DISPLAY %\c  WITH %(col) COLUMNS VIEW-AS DIALOG-BOX WITH WIDTH 130 STREAM-IO.
 color disp input .
 pause 100.
SC SELECT COUNT(*) FROM %\C .
&SC &SCOPED-DEFINE %\c
IPMP DEFINE INPUT  PARAMETER %\c AS MEMPTR    NO-UNDO.
DVLG DEFINE VARIABLE %\c AS LOGICAL   NO-UNDO.
IOPDT DEFINE INPUT-OUTPUT PARAMETER %\c AS DATE      NO-UNDO.
OPHN DEFINE OUTPUT PARAMETER %\c AS HANDLE    NO-UNDO.
SYSGETFILE DEFINE VARIABLE GFILE AS CHARACTER NO-UNDO initial "test.csv".
           DEFINE VARIABLE gfret AS LOGICAL NO-UNDO.
           %\S
           SYSTEM-DIALOG GET-FILE gfile
               TITLE      "另存为..."
               FILTERS    "逗号分隔的文本文件(*.csv)"   "*.csv",
                          "文本文件(*.txt)"   "*.txt"
               MUST-EXIST
               ASK-OVERWRITE
               INITIAL-DIR "."
               DEFAULT-EXTENSION ".CSV"
               SAVE-AS        %\S
               USE-FILENAME
               UPDATE gfret.
               if gfret then do:
                   OUTPUT TO VALUE(gfile).
                   PUT UNFORMAT "" SKIP.
                   FOR EACH %\c NO-LOCK:
                     EXPORT DELIMITER ","  .
                   END.
                   OUTPUT CLOSE.
             end.
             else do:
             end.
MES?(returnvariable "please giveme return variable" ret
     )
 DEFINE VARIABLE ret AS LOGICAL   NO-UNDO.
 MESSAGE "%\c" VIEW-AS ALERT-BOX QUESTION BUTTONS YES-NO title "" UPDATE %(returnvariable).
&GL &GLOBAL-DEFINE %\c
IOPHN DEFINE INPUT-OUTPUT PARAMETER %\c AS HANDLE    NO-UNDO.
&IF &IF %\c &THEN
    &ENDIF
MES MESSAGE "%\c" %\i VIEW-AS ALERT-BOX INFO BUTTONS OK.
IOPIN DEFINE INPUT-OUTPUT PARAMETER %\c AS INTEGER   NO-UNDO.
FNW(table "Table Name" 
    )
 FOR EACH %(table) NO-LOCK WHERE %\C:
     display %(table).
 END.
MSG MESSAGE %\c VIEW-AS ALERT-BOX INFO BUTTONS OK.
&IFD &IF (DEFINED %\c <> 0) &THEN
     &ENDIF
OPIN DEFINE OUTPUT PARAMETER %\c AS INTEGER   NO-UNDO.
IOPLG DEFINE INPUT-OUTPUT PARAMETER %\c AS LOGICAL   NO-UNDO.
DVCH DEFINE VARIABLE %\c AS CHARACTER NO-UNDO.
DVWH DEFINE VARIABLE %\c AS WIDGET    NO-UNDO.
DVDE DEFINE VARIABLE %\c AS DECIMAL   NO-UNDO.
&FR {&FRAME-NAME} %\c
DERR DO ON ERROR UNDO, LEAVE:
     %\i%\c
     END.
WRESIZE(WinName "WIndow name" cwin
        FrameName "Frame Name" f1
        tabName "Table Name" brTab
        )
 FRAME %(FrameName):WIDTH = %(WinName):WIDTH NO-ERROR.
 FRAME %(FrameName):VIRTUAL-WIDTH-CHARS = %(WinName):WIDTH NO-ERROR.
 %(tabName):WIDTH = %(WinName):WIDTH - %\c NO-ERROR.
 FRAME %(FrameName):HEIGHT= %(WinName):HEIGHT NO-ERROR.
 FRAME %(FrameName):VIRTUAL-HEIGHT-CHARS = %(WinName):HEIGHT NO-ERROR.
 %(tabName):HEIGHT = %(WinName):HEIGHT - 6.64 NO-ERROR.
 %\S
FEW FOR EACH %\C EXCLUSIVE-LOCK WHERE  :
        display %\c.
        update .
    END.
OPLG DEFINE OUTPUT PARAMETER %\c AS LOGICAL   NO-UNDO.
IPDT DEFINE INPUT  PARAMETER %\c AS DATE      NO-UNDO.
IOPMP DEFINE INPUT-OUTPUT PARAMETER %\c AS MEMPTR    NO-UNDO.
DFOR DO FOR %\c:
     %\i
     END.
IPRI DEFINE INPUT  PARAMETER %\c AS ROWID     NO-UNDO.
GBC(col "Whitch bolumns?" 1
    )
 DEFINE VARIABLE br-col AS WIDGET.   %\S
 br-col = %\c:GET-BROWSE-COLUMN(%(col)) in frame fmain  .
IOPRI DEFINE INPUT-OUTPUT PARAMETER %\c AS ROWID     NO-UNDO.
hQuery(hQuery "query WIDGET-HANNDLE variable name" hQuery
       strSQL "query SQL string" strSQL
       brList "Browse List Name" brList
       )
 %\L    DEFINE VARIABLE %(strSQL) AS CHARACTER NO-UNDO.
     DEFINE VARIABLE %(hQuery) AS WIDGET-HANDLE.
     %(hQuery) = %(brList):QUERY.
     %(strSQL) = "for each %\c no-lock ".
     %(hQuery):QUERY-PREPARE(%(strSQL)) NO-ERROR.
     %(hQuery):QUERY-OPEN NO-ERROR.
OPCH DEFINE OUTPUT PARAMETER %\c AS CHARACTER NO-UNDO.
OPWH DEFINE OUTPUT PARAMETER %\c AS WIDGET    NO-UNDO.
OPDE DEFINE OUTPUT PARAMETER %\c AS DECIMAL   NO-UNDO.
IPTF %\L INPUT FROM "%\c".
          REPEAT:
              Create .
              import delimiter "," .
          END.
      INPUT CLOSE.
     %\S
