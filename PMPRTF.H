#define INCL_PM
#define INCL_DOSNLS
#define INCL_DOSSESMGR
#define INCL_DOSFILEMGR
#define INCL_DOS
#define INCL_DEV
#define INCL_WIN
#define INCL_GPI
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_WINERRORS
#define INCL_GPIERRORS
#define INCL_ERRORS
#include <os2.h>

/* c standard libs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_OUTPUT_BUFFER 8192         /* Output buffer in prtf.c                            */
#define MAX_INPUT_BUFFER  8192         /* Input  buffer in prtf.c                            */
#define INITIAL         0L
#define FILE          111L
#define OPEN          222L
#define PRINT_QUEUED  333L
#define CLOSE         444L
#define NEXT_TIME     500L
#define OPEND           1L
#define CLOSED          2L
#define NOT_THREADED    0L
#define THREADED        1L
#define STACK_SIZE_PRTF 32*1024L
#define SET             1L
#define NOT_SET         0L

typedef struct PRTFDAT
{
  HPS         hps;              /* presentation space handle */
  FONTMETRICS fm;
  USHORT      usJob;
  HDC         hdc;              /* Device-context handle for the queue  */
  LONG        lLinefeed;
  LONG        lTab;
  LONG        lPage;
  LONG        lxLeft;
  PPRQINFO3   prq3 ;
  SIZEL       sizel;
  POINTL      pt;                        /* String screen coordinates    */
  HFILE       FHandle;           /* DosOpen file handle */
  INT         iMarginFlag;
  LONG        alWidths[256];   /* array of character width values */
  UCHAR       pInputBuffer[MAX_INPUT_BUFFER * 4];
  UCHAR       pOutputBuffer[MAX_INPUT_BUFFER];
  LONG        lInputBufferPrinted;
  LONG        lInputBufferPrintedPrevious;
  LONG        lInputBufferRead;
  LONG        lOutputBufferLength;
  UCHAR       ucThis_char;
  UCHAR       ucNext_char;
  LONG        lFirst;
  LONG        lTitleLen;
  INT         iCleanupDone;
  LONG        lCloseFileHandle;
  INT         iForceLineFeedNoCtlLf;
  INT         iForceLineFeedCtlLf;
  LONG        lrc;
  HAB         hab;
  INT         iErrFlag;
} PRTFDAT,  * PPRTFDAT;

typedef struct PRINTFILE
{
  LONG     lFattrs;
  CHAR     pszDocName[CCHMAXPATH];
  CHAR     pszFaceName[CCHMAXPATH];
  CHAR     pszQueueName[CCHMAXPATH];
  CHAR     pszSpoolerParms[CCHMAXPATH];
  CHAR     pszPrtTitle[CCHMAXPATH];
  UCHAR    pszTextToPrint[MAX_INPUT_BUFFER];
  CHAR     pszErrMsg[CCHMAXPATH * 2];
  FIXED    fPointSize;
  LONG     lCopies;
  LONG     lAction;
  LONG     lLeftMargin;
  LONG     lTabExpand;
  LONG     lUseFattrs;
  LONG     lThreaded;
  LONG     lDone;
  PFATTRS  pFat;
  PPRTFDAT pPrtfDat;
} PRINTFILE, * PPRINTFILE;


/* Function prototypes */

int main(int, char **);
SHORT  cleanup(PPRINTFILE, APIRET);
SHORT  printhead(PPRINTFILE);
SHORT  newpage(PPRINTFILE);
SHORT  SplOpenDevice(PPRINTFILE);
SHORT  SplSpool(PPRINTFILE);
SHORT  SplCloseDevice(PPRINTFILE);
SHORT  copyright(void);
SHORT  printfiles(PPRINTFILE);
SHORT  readfile(PPRINTFILE);
void   printfiles2(PVOID);
SHORT  setdefaults(PPRINTFILE);
double TwipsToFixed(FIXED);              /* Convert TWIPS to FIXED  representation */
double FixedToTwips(double);             /* do the reverse                         */

