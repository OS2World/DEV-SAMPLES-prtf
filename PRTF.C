/* c standard libs */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "pmprtf.h"

int main(int argc, char *argv[])
{

   APIRET rc;

   PRINTFILE  Prtf;
   PPRINTFILE pPrtf;
   PRTFDAT    PrtfDat;
   PPRTFDAT   pPrtfDat;
   CHAR szDocName[CCHMAXPATH];
   CHAR * pszDeviceName;
   CHAR * pszFaceName="Courier";
   CHAR * pszToken = "*";
   int n;
   CHAR pBuffer[256];
   int iInteractive = TRUE;
   PPRTFDAT pSav;

   Prtf.pPrtfDat = &PrtfDat;

   Prtf.lUseFattrs = FALSE;
   Prtf.lFattrs = 0;
   Prtf.lLeftMargin = 0;
   Prtf.lTabExpand = 3;
   Prtf.pszDocName[0] = '\0';
   Prtf.pszFaceName[0] = '\0';
   Prtf.pszQueueName[0] = '\0';
   Prtf.pszPrtTitle[0] = '\0';
   Prtf.fPointSize = 8.0;
   Prtf.lCopies = 1;
   Prtf.pszSpoolerParms[0] = '\0';
   Prtf.lThreaded = NOT_THREADED;

   if (argc<2)
   {
      printf("Syntax: prtf file [-d \"Document\"   ]     [no -d option invokes interactive mode]\n"
             "                  [-f \"Face name\"  ]     [-p \"Pointsize\"]\n"
             "                  [-q \"Queue name\" ]     Queue name = physical name in printer settings\n"
             "                  [-h \"Heading\"    ]     [-a \"PRTY=xx FORM=xx\"]\n"
             "                  [-l \"Left margin\"]     in sizes of a single blank, e.g. 3 times a blank]\n"
             "                  [-x Threaded       ]  [-c \"Copies\"]\n"
             "                  [-t \"Expand tabs\"]     to the number of blanks, e.g. 3\n"
             "                  [-i -u -b -o -s]       where\n"
             "                  (-i=Italic -u=Underscore -b=Bold -o=Outline -s=Strikeout)\n\n"
             "Only filename chooses Courier 8.0 on default  queue.\n"
             "No document name invokes interactive printing.\n\n"
             "Sample: prtf -d c:\\config.sys -h \"my print\" -p 10.0 -f Courier -x\n"
             "        prtf -h \"my print\" -p 14.0 -f Helv\n\n"
             "Threaded (-x) invokes one thread in \"FILE\" mode and  one thread for"
             "every string to be spooled in a \"OPEN\" \"SPOOL\" \"CLOSE\" sequence!\n"
             "Everything you enter will be spooled and printed.\n\n"
             "Hint from your environmental dept.: Open your printer icon and choose\n"
             "\"change status\". Set all jobs to \"hold\". After the formatting and\n"
             "Spooling is done, you can preview your print. Saves paper, ink and money.\n\n");
   
      printf("If you want to start in interactive mode with\n"
             "default values type \"yes\".\n\nEnter --> ");
      fflush(stdout);
      gets(pBuffer);
      fflush(stdin);
      if (0 != strcmp(pBuffer,"yes"))
      {
         copyright();
         exit(0);
      }
      printf("\n\n");
   }
   /* recognize the arguments (dirty hack) */

   for (n=1;n < argc;n++)
   {
       if (0==strcmp(argv[n],"-i"))
          Prtf.lFattrs+=FATTR_SEL_ITALIC;
       if (0==strcmp(argv[n],"-u"))
          Prtf.lFattrs+=FATTR_SEL_UNDERSCORE;
       if (0==strcmp(argv[n],"-b"))
          Prtf.lFattrs+=FATTR_SEL_BOLD;
       if (0==strcmp(argv[n],"-s"))
          Prtf.lFattrs+=FATTR_SEL_STRIKEOUT;
       if (0==strcmp(argv[n],"-o"))
          Prtf.lFattrs+=FATTR_SEL_OUTLINE;
       if (0==strcmp(argv[n],"-f"))
           strcpy(Prtf.pszFaceName,argv[++n]);
       if (0==strcmp(argv[n],"-l"))
           Prtf.lLeftMargin=atol(argv[++n]);
       if (0==strcmp(argv[n],"-t"))
           Prtf.lTabExpand=atol(argv[++n]);
       if (0==strcmp(argv[n],"-h"))
           strcpy(Prtf.pszPrtTitle,argv[++n]);
       if (0==strcmp(argv[n],"-d"))
       {
           strcpy(Prtf.pszDocName,argv[++n]);
           iInteractive = FALSE;
       }
       if (0==strcmp(argv[n],"-q"))
           strcpy(Prtf.pszQueueName,argv[++n]);
       if (0==strcmp(argv[n],"-x"))
             Prtf.lThreaded = THREADED;
       if (0==strcmp(argv[n],"-p"))
           Prtf.fPointSize=atof(argv[++n]);
       if (0==strcmp(argv[n],"-c"))
           Prtf.lCopies=atoi(argv[++n]);
       if (0==strcmp(argv[n],"-a"))
           strcpy(Prtf.pszSpoolerParms,argv[++n]);
   }


/* In a real application, you would either use the threaded version and therefore allocate 
   the memory for the structures PRINTFILE and PRTFDATA. When the spooling is done, printfiles
   frees the memory. If you don't set the THREADED parameter, you can pass in the Adress of the
   Prtf structure. Hence, there is no need to free the structure , since you
   wait after the printfile call until the work is done. If you use the THREADED parameter, you
   can have left the subroutine which invoked printfile - that's why in this case you MUST allocate
   the memory as shown below, so I free it when all is done.
   For simplicity I copy the already extrcted parameters from the Prtf structure.
*/
   
   if (Prtf.lThreaded == THREADED)
   {
       fflush(stdout);
       printf("Threaded run! Parent waits until \"lDone\" is set to \"THREADED\".\n");
       if (rc = DosAllocMem((VOID *) &pPrtf, (ULONG) sizeof(PRINTFILE),PAG_READ|PAG_WRITE|PAG_COMMIT))
       {
           printf("Not enough memory. Request was %i bytes. Rc: %i",sizeof(PRINTFILE),rc);
           exit(0);
       }
  
       if (rc = DosAllocMem((VOID *) &(pPrtfDat), (ULONG) sizeof(PRTFDAT),PAG_READ|PAG_WRITE|PAG_COMMIT))
       {
           printf("Not enough memory. Request was %i bytes. Rc: %i",sizeof(PRTFDAT),rc);
           exit(0);
       }
       memcpy(pPrtf,&Prtf,sizeof(PRINTFILE));
       pPrtf->pPrtfDat = pPrtfDat;
       memcpy(pPrtf->pPrtfDat,Prtf.pPrtfDat,sizeof(PRTFDAT));
   }

   if (iInteractive == FALSE)
   {
       if (Prtf.lThreaded == NOT_THREADED) 
       {
          Prtf.lAction = FILE;
          rc = printfiles(&Prtf);
          if (rc != 0) 
          {
             printf("\n\nError: (Content of pszErrMsg)\n%s\n",Prtf.pszErrMsg);
             exit(rc);
          }
       }
       else
       {
          pPrtf->lAction = FILE;
          rc = printfiles(pPrtf);
          if (rc != 0)
          {
             printf("\n\nError occured. No info available since PRINTFILE structure is freed.\n");
             exit(rc);
          }
       }
   }
   else
   {
          if (Prtf.lThreaded == NOT_THREADED)
          {
             printf("Everything you type will be printed\n"
                    "with the above specified parameters.\n"
                    "Enter exit to finish.\n\n");
             fflush(stdout);
             printf("Doctitle --> ");
             fflush(stdout);
             gets(pBuffer);
             fflush(stdin);
             strcpy(Prtf.pszDocName,pBuffer);

             Prtf.lAction = OPEN;
             rc = printfiles(&Prtf);
             if (rc != 0)
             {
                 printf("\n\nError: (Content of pszErrMsg)\n %s\n",Prtf.pszErrMsg);
                 exit(rc);
             }
             Prtf.lAction = PRINT_QUEUED;
             while (TRUE)
             {
                 printf("Enter text -->  ");
                 fflush(stdout);
                 gets(pBuffer);
                 fflush(stdin);
                 if (0 == strcmp(pBuffer,"exit"))
                   break;
                 strcpy(Prtf.pszTextToPrint,pBuffer);
                 strcat(Prtf.pszTextToPrint,"\x0d\x0a");
                 rc = printfiles(&Prtf);
                 if (rc != 0)
                 {
                    printf("\n\nError: (Content of pszErrMsg)\n %s\n",Prtf.pszErrMsg);
                    exit(rc);
                 }
             }
             Prtf.lAction = CLOSE;
             rc = printfiles(&Prtf);
             if (rc != 0)
             {
                 printf("\n\nError: (Content of pszErrMsg)\n %s\n",Prtf.pszErrMsg);
                 exit(rc);
             }
         }
         else
         {
             printf("Everything you type will be printed\n"
                    "with the above specified parameters.\n"
                    "Enter exit to finish.\n\n");
             fflush(stdout);
             printf("Doctitle --> ");
             fflush(stdout);
             gets(pBuffer);
             fflush(stdin);
             strcpy(pPrtf->pszDocName,pBuffer);
             pPrtf->lDone = 0;
             pPrtf->lAction = OPEN;
             rc = printfiles(pPrtf);
             while (TRUE)
             {
                 DosSleep(1000L);
                 if (pPrtf->lDone == OPEND)
                   break;
             }
             if (rc != 0)
             {
                    printf("\n\nError: (Content of pszErrMsg)\n %s\n",pPrtf->pszErrMsg);
                    exit(rc);
             }
             pPrtf->lAction = PRINT_QUEUED;
             while (TRUE)
             {
                 printf("Enter text -->  ");
                 fflush(stdout);
                 gets(pBuffer);
                 fflush(stdin);
                 if (0 == strcmp(pBuffer,"exit"))
                   break;
                 strcpy(pPrtf->pszTextToPrint,pBuffer);
                 strcat(pPrtf->pszTextToPrint,"\x0d\x0a");
                 printfiles(pPrtf);
             }
             pPrtf->lAction = CLOSE;
             printfiles(pPrtf);
         }
   }
   if (Prtf.lThreaded == THREADED) 
   {
      printf("\n\n\nPlease open your printer object on the desktop!\n");
      printf("W A I T  until the green arrow for the document object disappears.\n");
      printf("Then the thread closing the spooler is done. Exit with any keystroke.\n\n\n");
      fflush(stdout);
      fflush(stdin);
      while (_kbhit() == 0);
   }
   printf("Main (prtf) is exiting now!");
   exit(0);
}


SHORT printfiles(PPRINTFILE pPrtf)
{
   int iRc;
   if (pPrtf->lThreaded == THREADED) 
   {
      iRc = _beginthread(printfiles2,NULL,(unsigned) STACK_SIZE_PRTF,(PVOID) pPrtf);
      if (iRc == -1)
      {
        sprintf (pPrtf->pszErrMsg,"Can't start thread RC = %i.",iRc);
        return(TRUE);
      }
      return(FALSE);
   }
   else
   {
      printfiles2((PVOID) pPrtf);
      return(pPrtf->pPrtfDat->lrc);
   }
}

void printfiles2(PVOID pPrtfVoid)
{
   PPRINTFILE pPrtf;

   pPrtf = (PPRINTFILE) pPrtfVoid;
   pPrtf->pPrtfDat->lrc = 0;
   pPrtf->pszErrMsg[0] = '\0';
   pPrtf->pPrtfDat->iCleanupDone = FALSE;
   pPrtf->pPrtfDat->prq3 = NULL;
   pPrtf->pPrtfDat->iForceLineFeedCtlLf   = -1;
   pPrtf->pPrtfDat->iForceLineFeedNoCtlLf = -1;
   pPrtf->pPrtfDat->iErrFlag = NOT_SET;
 

   /* Entry points: */

   switch(pPrtf->lAction)
   {
      case OPEN:
      {
         pPrtf->pPrtfDat->lFirst = TRUE;
         pPrtf->pPrtfDat->lCloseFileHandle = FALSE;
         pPrtf->pPrtfDat->lrc = SplOpenDevice(pPrtf);
      }
      break;
      case PRINT_QUEUED:
         pPrtf->pPrtfDat->lrc = SplSpool(pPrtf);
      break;
      case CLOSE:
         pPrtf->pPrtfDat->lrc = SplCloseDevice(pPrtf);
      break;
      case FILE:
      {
           pPrtf->pPrtfDat->lFirst = TRUE;
           pPrtf->pPrtfDat->lCloseFileHandle = TRUE;
           pPrtf->pPrtfDat->lrc = SplOpenDevice(pPrtf);
           if (pPrtf->pPrtfDat->lrc != 0)
             break;
           newpage(pPrtf);
           pPrtf->pPrtfDat->pt.x =  pPrtf->pPrtfDat->lxLeft;
           pPrtf->pPrtfDat->pt.y = pPrtf->pPrtfDat->sizel.cy-pPrtf->pPrtfDat->lLinefeed;
           pPrtf->pPrtfDat->lrc = SplSpool(pPrtf);
           if (pPrtf->pPrtfDat->lrc != 0)
             break;
           pPrtf->pPrtfDat->lrc = SplCloseDevice(pPrtf);
      }
      break;
      default:
      break;
   }
   if ((pPrtf->lThreaded == THREADED &&  pPrtf->lAction == CLOSE) ||
       (pPrtf->lThreaded == THREADED && pPrtf->pPrtfDat->iCleanupDone))
   {
      if (pPrtf->pPrtfDat  != (PPRTFDAT) NULL)
          DosFreeMem(pPrtf->pPrtfDat);
      if (pPrtf->pFat != (PFATTRS) NULL)
          DosFreeMem(pPrtf->pFat);
      if (pPrtf  != (PPRINTFILE) NULL)
          DosFreeMem(pPrtf);
   }
}

SHORT SplCloseDevice(PPRINTFILE pPrtf)
{
  LONG rc = 0;
  LONG l;

  if (pPrtf->pPrtfDat->lCloseFileHandle == TRUE)
     DosClose(pPrtf->pPrtfDat->FHandle);

   if (pPrtf->pPrtfDat->lOutputBufferLength)
   {
       rc=GpiCharString(pPrtf->pPrtfDat->hps, 
                        pPrtf->pPrtfDat->lOutputBufferLength, pPrtf->pPrtfDat->pOutputBuffer);
       if (rc!=GPI_OK)
       {
           if(pPrtf->pPrtfDat->iCleanupDone == FALSE)
              sprintf(pPrtf->pszErrMsg,"%sCharString failed:\n",pPrtf->pszErrMsg);
           cleanup(pPrtf,rc);
       }
   }

#ifdef never
   SplQmWrite(hspl, n, FileBuffer);

   rc = SplQmEndDoc(hspl);

   sprintf(pPrtf->pszErrMsg,"Job id is %d\n",rc);
#endif

    /* close the document */
    l = 2;
    rc=  DevEscape(pPrtf->pPrtfDat->hdc,
                   DEVESC_ENDDOC,
                    0L, (PBYTE)NULL, &l, (PBYTE)&pPrtf->pPrtfDat->usJob);
    if (DEV_OK!=rc)
    {
       if(pPrtf->pPrtfDat->iCleanupDone == FALSE)
          sprintf(pPrtf->pszErrMsg,"%sDevEscape ENDDOC failed: %d\n",pPrtf->pszErrMsg,l);
       cleanup(pPrtf,rc);
    }
    /* discard presentation space */
/*     GpiAssociate(pPrtf->pPrtfDat->hps, (pPrtf->pPrtfDat->hdc)NULLHANDLE ); */
         GpiDestroyPS(pPrtf->pPrtfDat->hps);

    /* discard device context */
    DevCloseDC(pPrtf->pPrtfDat->hdc);
#ifdef never
    SplQmClose(hspl);
#endif
   copyright();
   WinTerminate(pPrtf->pPrtfDat->hab); /*                   Terminate the application    */
   pPrtf->lDone = CLOSED;
   if (pPrtf->pPrtfDat->prq3 != NULL)
      DosFreeMem(pPrtf->pPrtfDat->prq3);
   if(pPrtf->pPrtfDat->iCleanupDone == TRUE)
      return(TRUE);
   else
      return(FALSE);
}

SHORT SplSpool(PPRINTFILE pPrtf)
{
    LONG rc = 0;

    if (pPrtf->lAction == PRINT_QUEUED)
    {
       pPrtf->pPrtfDat->lInputBufferPrinted = 0;
       pPrtf->pPrtfDat->lOutputBufferLength = 0;
       pPrtf->pPrtfDat->lInputBufferPrintedPrevious = 0;
       memcpy(pPrtf->pPrtfDat->pInputBuffer,pPrtf->pszTextToPrint,strlen(pPrtf->pszTextToPrint));
       if (pPrtf->pPrtfDat->lFirst == TRUE)
       {
           newpage(pPrtf);
           pPrtf->pPrtfDat->pt.x =  pPrtf->pPrtfDat->lxLeft;
           pPrtf->pPrtfDat->pt.y = pPrtf->pPrtfDat->sizel.cy-pPrtf->pPrtfDat->lLinefeed;
           pPrtf->pPrtfDat->lFirst = FALSE;
           pPrtf->pPrtfDat->lInputBufferRead += strlen(pPrtf->pszTextToPrint);
       }
       else
          pPrtf->pPrtfDat->lInputBufferRead = strlen(pPrtf->pszTextToPrint);

    }
    while (TRUE)
    {
        top:
        if (pPrtf->pPrtfDat->iErrFlag == SET)
           break;

        if (pPrtf->lAction == FILE)
        {
           if (pPrtf->pPrtfDat->lInputBufferPrinted > pPrtf->pPrtfDat->lInputBufferRead ||
               pPrtf->pPrtfDat->lFirst == TRUE)
              if (!readfile(pPrtf))
                   break;
        }
        printhead(pPrtf);

        if (pPrtf->lAction == PRINT_QUEUED &&
            pPrtf->pPrtfDat->lInputBufferPrinted ==
            pPrtf->pPrtfDat->lInputBufferRead)
            break;
       
        pPrtf->pPrtfDat->ucThis_char    = pPrtf->pPrtfDat->pInputBuffer[pPrtf->pPrtfDat->lInputBufferPrinted];
        if (pPrtf->pPrtfDat->lInputBufferPrinted + 1 <= pPrtf->pPrtfDat->lInputBufferRead)
            pPrtf->pPrtfDat->ucNext_char = pPrtf->pPrtfDat->pInputBuffer[pPrtf->pPrtfDat->lInputBufferPrinted + 1];
        else
            pPrtf->pPrtfDat->ucNext_char = 0;

  /* Look one character ahead and determine whether a new page will be printed. This is needed because the
           insertion of the title in newpage() has to take place before the character position (pos) is ucThis_char.
           Example:
           This is a text here the new page starts.
                                         IL newpage
                                         L  look ahead, when newpage, call newpage

           This is a text here the new pag !this is the title!e starts.

           Since the charcter taking the place of the last e in "page" might be small (e.g. an i) still the
           newpage wouldn't be executed, so we have to force it with the iForceLineFeed flag.
           This has to be done here since after the switch code section the newline and carriage returns branch
           back to top.
        */

        if (pPrtf->pPrtfDat->pt.y - pPrtf->pPrtfDat->lLinefeed < 0)
        {
             
            if (pPrtf->pPrtfDat->ucThis_char     == '\x0d' &&  pPrtf->pPrtfDat->ucNext_char == '\x0a')
            {
               pPrtf->pPrtfDat->iForceLineFeedCtlLf = 1;
            }
            if (pPrtf->pPrtfDat->iForceLineFeedCtlLf == 0)
                   newpage(pPrtf);
        } 

        if (pPrtf->pPrtfDat->iForceLineFeedCtlLf >= 0)
              pPrtf->pPrtfDat->iForceLineFeedCtlLf--;


        /* see if it's a control char */
        switch (pPrtf->pPrtfDat->ucThis_char)
        {
           case  13:
           {
              pPrtf->pPrtfDat->pt.x =  pPrtf->pPrtfDat->lxLeft;
              pPrtf->pPrtfDat->lInputBufferPrinted++;
              goto top;
           }
           case  10:
           {
              pPrtf->pPrtfDat->pt.y-=pPrtf->pPrtfDat->lLinefeed;
              pPrtf->pPrtfDat->lInputBufferPrinted++;
              goto top;
           }
           case  9:
           {
              if (pPrtf->lTabExpand != 0)  /* avoid a zero divide */
                  pPrtf->pPrtfDat->pt.x=(pPrtf->pPrtfDat->pt.x/pPrtf->pPrtfDat->lTab+1) * pPrtf->pPrtfDat->lTab;
              pPrtf->pPrtfDat->lInputBufferPrinted++;
              goto top;
           }
           case  8:
           {
              pPrtf->pPrtfDat->pt.x-=pPrtf->pPrtfDat->alWidths[' '];
              if  (pPrtf->pPrtfDat->pt.x < pPrtf->pPrtfDat->lxLeft)
                  pPrtf->pPrtfDat->pt.x =  pPrtf->pPrtfDat->lxLeft;
              pPrtf->pPrtfDat->lInputBufferPrinted++;
              goto top;
           }
           case 12:
           {
              if (pPrtf->pPrtfDat->ucThis_char ==12)
              pPrtf->pPrtfDat->pt.y=-1; /* force form feed */
              pPrtf->pPrtfDat->lInputBufferPrinted++;
              goto top;
           }
        }
        /* get its width */
        if (pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucThis_char]>=pPrtf->pPrtfDat->sizel.cx)
        {
           sprintf(pPrtf->pszErrMsg,"Character %c does not fit on the page\n",pPrtf->pPrtfDat->ucThis_char);
           cleanup(pPrtf,rc);
           return(TRUE);
        }

        /* This has to be done here because this code doesn't look for newlines and carriage 
           returns. It looks whether there is enough space  on the current line. If not, a new
           line will be simulated in the next if section. We look one byte ahead to prepare the
           insertion of the title. If we wouldn't, ucThis_char would always be printed on the
           new page before the title.                                                             */

        if (pPrtf->pPrtfDat->pt.x + pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucThis_char] +
                                    pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucNext_char] >=
                                    pPrtf->pPrtfDat->sizel.cx &&
                                    pPrtf->pPrtfDat->pt.y - pPrtf->pPrtfDat->lLinefeed < 0)
        {
            if (pPrtf->pPrtfDat->iForceLineFeedNoCtlLf < 0)
            {
                newpage(pPrtf);
                pPrtf->pPrtfDat->iForceLineFeedNoCtlLf =  1;
            }
        }


        if (pPrtf->pPrtfDat->pt.x + pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucThis_char] >=
                                           pPrtf->pPrtfDat->sizel.cx || 
                                           pPrtf->pPrtfDat->iForceLineFeedNoCtlLf  ==  0)
        {
           pPrtf->pPrtfDat->pt.x= pPrtf->pPrtfDat->lxLeft;  /* on wraparound simulate cr and lf */
           pPrtf->pPrtfDat->pt.y-=pPrtf->pPrtfDat->lLinefeed;
           pPrtf->pPrtfDat->iMarginFlag=1;
        }
        else
        {
           pPrtf->pPrtfDat->iMarginFlag=0;
           pPrtf->pPrtfDat->pOutputBuffer[pPrtf->pPrtfDat->lOutputBufferLength] = 
                  pPrtf->pPrtfDat->ucThis_char;
           pPrtf->pPrtfDat->lOutputBufferLength++;
           pPrtf->pPrtfDat->pt.x+=pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucThis_char];
        }
        pPrtf->pPrtfDat->lInputBufferPrinted++;
        if (pPrtf->pPrtfDat->iForceLineFeedNoCtlLf >= 0)
           pPrtf->pPrtfDat->iForceLineFeedNoCtlLf--;
   }
   if(pPrtf->pPrtfDat->iCleanupDone == TRUE)
      return(TRUE);
   else
      return(FALSE);
}

SHORT SplOpenDevice(PPRINTFILE pPrtf)
{
   DEVOPENSTRUC dos;     /* description of the queue */
   UCHAR pWorkBuffer[MAX_OUTPUT_BUFFER];
   FATTRS fat;
   LONG rc = 0;
   ULONG ulAction;
   ULONG ulBytesNeeded;
   ULONG  cbBuf;
   ULONG  cbNeeded ;
   PVOID  pBuf;
   CHAR * pszDeviceName;
   CHAR * pszToken = "*";

   /* GRADIENTL gradlAngle={0L,0L}; */
   SIZEF sizfCharBox; /* Character-box size in world coordinates.     */

   pPrtf->pPrtfDat->lInputBufferRead = 0;
   pPrtf->pPrtfDat->lInputBufferPrinted = 0;
   pPrtf->pPrtfDat->lOutputBufferLength = 0;

   pPrtf->pPrtfDat->sizel.cx = 0;
   pPrtf->pPrtfDat->sizel.cy = 0;
   pPrtf->pPrtfDat->iMarginFlag = 0;
   pPrtf->pPrtfDat->lPage = 0;
   setdefaults(pPrtf);

   if (0==(pPrtf->pPrtfDat->hab = WinInitialize( 0 )))
       {
       sprintf(pPrtf->pszErrMsg,"WinInitialize failed:\n");
       cleanup(pPrtf,rc);
       return(TRUE);
       }

   if (pPrtf->lAction == FILE) 
   {
      /* open the input file */
      if (0!=(rc=DosQueryPathInfo(pPrtf->pszDocName, 5, &pPrtf->pszDocName,
                                        sizeof(pPrtf->pszDocName))))
      {
          sprintf(pPrtf->pszErrMsg,"DosQueryPathInfo failed:\n");
          cleanup(pPrtf,rc);
          return(TRUE);
      }
      if (0!=(rc=DosOpen(pPrtf->pszDocName, &pPrtf->pPrtfDat->FHandle,
                          &ulAction, 0, FILE_NORMAL,
                          FILE_OPEN,
                          OPEN_FLAGS_SEQUENTIAL+
                          OPEN_SHARE_DENYWRITE+
                          OPEN_ACCESS_READONLY,
                          NULL)))
      {
         sprintf(pPrtf->pszErrMsg,"DosOpen failed:\n");
         cleanup(pPrtf,rc);
         return(TRUE);
      }
    }

      if (pPrtf->pszQueueName[0] == '\0')
      {
        /* get default queue from os2.ini */
         if (0==PrfQueryProfileString(HINI_PROFILE,
                                     (PSZ)"PM_SPOOLER",
                                     (PSZ)"QUEUE",
                                      NULL,
                                     (PSZ)pPrtf->pszQueueName,
                                     (LONG)sizeof(pPrtf->pszQueueName)
                                     ))
         {
            sprintf(pPrtf->pszErrMsg,"No default queue defined: please define one or use the -q option\n");
            cleanup(pPrtf,rc);
            return(TRUE);
         }
         if (NULL!=(pszDeviceName=strchr(pPrtf->pszQueueName,';')))
                   *pszDeviceName='\0';
     }
     rc = SplQueryQueue((PVOID)NULL, pPrtf->pszQueueName, 3L,
                                 (PVOID)NULL, 0L, &cbNeeded );
     if (rc != NERR_BufTooSmall && rc != ERROR_MORE_DATA )
     {
        sprintf(pPrtf->pszErrMsg,"SplQueryQueue Error\n") ;
        cleanup(pPrtf,rc);
        return(TRUE);
     }
     if (!DosAllocMem( &pBuf, cbNeeded,PAG_READ|PAG_WRITE|PAG_COMMIT))
     {
        cbBuf = cbNeeded ;
        rc = SplQueryQueue((PVOID)NULL, pPrtf->pszQueueName, 3L,
                                        pBuf, cbBuf, &cbNeeded) ;
        pPrtf->pPrtfDat->prq3=(PPRQINFO3)pBuf;
     }

/* this is not needed for spl calls, but needed for dev */
#ifndef never
     if (NULL!=(pszDeviceName=strchr(pPrtf->pPrtfDat->prq3->pszPrinters,',')))
        *pszDeviceName='\0';
     if (NULL!=(pszDeviceName=strchr(pPrtf->pPrtfDat->prq3->pszDriverName,'.')))
     {
         *pszDeviceName='\0';
          pszDeviceName++;
     }
     else
         pszDeviceName="";

     if (DEV_OK!=(rc = DevPostDeviceModes( pPrtf->pPrtfDat->hab,
                             pPrtf->pPrtfDat->prq3->pDriverData,
                             pPrtf->pPrtfDat->prq3->pszDriverName,
                             pszDeviceName,
                             pPrtf->pPrtfDat->prq3->pszPrinters,
                             DPDM_QUERYJOBPROP )))
     {
        sprintf(pPrtf->pszErrMsg,"DevPostDeviceModes failed");
        cleanup(pPrtf,rc);
        return(TRUE);
     }

#endif

    memset( &dos,   0, sizeof(dos));
    dos.pszLogAddress      = pPrtf->pPrtfDat->prq3->pszName;           /* queue name */
    dos.pszDriverName      = pPrtf->pPrtfDat->prq3->pszDriverName;
    dos.pdriv              = pPrtf->pPrtfDat->prq3->pDriverData;
    dos.pszDataType        = "PM_Q_STD";  /* RAW if using Spl calls */
    dos.pszComment         = pPrtf->pszDocName;
    dos.pszQueueProcName   = pPrtf->pPrtfDat->prq3->pszPrProc;
    sprintf(pWorkBuffer,"MAP=N COP=%d",pPrtf->lCopies);
    dos.pszQueueProcParams=pWorkBuffer;
    dos.pszSpoolerParams   = pPrtf->pszSpoolerParms;
/*  dos.pszNetworkParams   = NULL; */

#ifdef never
       /* corresponds to DevOpenDc */
    hspl = SplQmOpen(pszToken,8,(PQMOPENDATA)&dos);

    if ( hspl != SPL_ERROR )        /* Good SplSpooler handle */
       sprintf(pPrtf->pszErrMsg,"SplQmOpen handle is %d\n",hspl);
    else
    {
        sprintf(pPrtf->pszErrMsg,"SplQmOpen failed.\n");
        cleanup(pPrtf,rc);
        return(TRUE);
    }
#endif

 /* create device context for the queue */
    if (DEV_ERROR==( pPrtf->pPrtfDat->hdc = 
                     DevOpenDC(pPrtf->pPrtfDat->hab, OD_QUEUED, pszToken, 8, (void*)&dos, NULLHANDLE)))
    {
       sprintf(pPrtf->pszErrMsg,"DevOpenDC failed:\n");
       cleanup(pPrtf,rc);
       return(TRUE);
    }

#ifdef never
    SplQmStartDoc(hspl,pPrtf->pszDocName);
#endif

         /* Issue STARTDOC to begin printing */
    if (DEV_OK!=(rc=DevEscape( pPrtf->pPrtfDat->hdc,
                    DEVESC_STARTDOC,
                    (int)strlen(pPrtf->pszDocName),
                    pPrtf->pszDocName,
                    (PLONG)NULL, (PBYTE)NULL)))
    {
       sprintf(pPrtf->pszErrMsg,"DevEscape STARTDOC failed:\n");
       cleanup(pPrtf,rc);
       return(TRUE);
    }

 /* create a presentation space associated with the context */

    if (0==(pPrtf->pPrtfDat->hps = GpiCreatePS(pPrtf->pPrtfDat->hab, pPrtf->pPrtfDat->hdc,
 &pPrtf->pPrtfDat->sizel, /* use same page size as device */
        PU_TWIPS+GPIF_LONG+GPIT_NORMAL+GPIA_ASSOC)))
    {
       sprintf(pPrtf->pszErrMsg,"GpiCreatePS failed:\n");
       cleanup(pPrtf,rc);
       return(TRUE);
    }

    GpiSetAttrMode(pPrtf->pPrtfDat->hps,AM_NOPRESERVE);
    GpiQueryPS(pPrtf->pPrtfDat->hps, &pPrtf->pPrtfDat->sizel);

    GpiSetCharMode(pPrtf->pPrtfDat->hps,CM_MODE2);
    GpiSetColor(pPrtf->pPrtfDat->hps,CLR_DEFAULT );


    if (pPrtf->lUseFattrs == FALSE)
    {
       sizfCharBox.cx = (LONG)(pPrtf->fPointSize*20*65536); /* translate to twips and fix */
       sizfCharBox.cy = (LONG)(pPrtf->fPointSize*20*65536);
    }
    else
    {
       sizfCharBox.cx = (LONG) TwipsToFixed(pPrtf->fPointSize)*20*65536;
       sizfCharBox.cy = (LONG) TwipsToFixed(pPrtf->fPointSize)*20*65536;
    }

    /* select a font */
    if (pPrtf->lUseFattrs == FALSE)
    {
        memset(&fat,0,sizeof(fat)); /* defaults */
        strcpy(fat.szFacename,pPrtf->pszFaceName);
        fat.usRecordLength=sizeof(fat);
        fat.fsSelection=pPrtf->lFattrs;
        fat.fsFontUse = FATTR_FONTUSE_NOMIX;/*+FATTR_FONTUSE_OUTLINE+FATTR_FONTUSE_TRANSFORMABLE*/;
        /* 0 for scalable font */
        fat.lMaxBaselineExt=fat.lAveCharWidth=pPrtf->fPointSize;
    }
    else
    {
        memcpy(&fat,pPrtf->pFat,sizeof(FATTRS));
        if (fat.fsFontUse == FATTR_FONTUSE_OUTLINE ) 
            fat.lMaxBaselineExt=fat.lAveCharWidth=0;
        else 
        {
            fat.lMaxBaselineExt=fat.lAveCharWidth=pPrtf->fPointSize;
            fat.fsFontUse = FATTR_FONTUSE_NOMIX+FATTR_FONTUSE_OUTLINE+FATTR_FONTUSE_TRANSFORMABLE;
        }
    }
    if (FONT_MATCH!=(rc=GpiCreateLogFont(pPrtf->pPrtfDat->hps,
                                  (PSTR8)NULL,
                                  1L,
                                  &fat)))
    {
       if (rc==FONT_DEFAULT)
           sprintf(pPrtf->pszErrMsg,"Unable to match the typeface\n");
       else
          sprintf(pPrtf->pszErrMsg,"GpiCreateLogFont failed:\n");
       cleanup(pPrtf,rc);
       return(TRUE);
    }

    GpiSetCharSet(pPrtf->pPrtfDat->hps, 1L );

    /*GpiSetCharAngle(pPrtf->pPrtfDat->hps,&gradlAngle); */

   
    GpiSetCharBox(pPrtf->pPrtfDat->hps,&sizfCharBox);
    GpiQueryFontMetrics(pPrtf->pPrtfDat->hps, sizeof(FONTMETRICS), &pPrtf->pPrtfDat->fm);

    /* figure out how much tab and line feed are in this font */
    pPrtf->pPrtfDat->lTab=pPrtf->pPrtfDat->fm.lAveCharWidth*pPrtf->lTabExpand;
    pPrtf->pPrtfDat->lLinefeed = (pPrtf->pPrtfDat->fm.lExternalLeading+pPrtf->pPrtfDat->fm.lMaxBaselineExt);

    /* retrieve character width table */
    rc=GpiQueryWidthTable(pPrtf->pPrtfDat->hps,
                    0,
                    255,
                    pPrtf->pPrtfDat->alWidths);
    pPrtf->pPrtfDat->lxLeft = pPrtf->lLeftMargin * pPrtf->pPrtfDat->alWidths[' '];
    if (pPrtf->pPrtfDat->lxLeft > pPrtf->pPrtfDat->sizel.cx) 
    {
       sprintf(pPrtf->pszErrMsg,"Left margin greater than text width.");
       cleanup(pPrtf,rc);
    } /* endif */
    pPrtf->lDone = OPEND;
    return(FALSE);
}

SHORT readfile(PPRINTFILE pPrtf)
{
      SHORT rc;
      ULONG lInputBufferReadSave;

      lInputBufferReadSave = pPrtf->pPrtfDat->lInputBufferRead;
      if ((rc=DosRead(pPrtf->pPrtfDat->FHandle, 
           pPrtf->pPrtfDat->lFirst == TRUE ?
           &(pPrtf->pPrtfDat->pInputBuffer + pPrtf->pPrtfDat->lInputBufferRead) : &pPrtf->pPrtfDat->pInputBuffer,
              MAX_INPUT_BUFFER, (ULONG *) &pPrtf->pPrtfDat->lInputBufferRead)) != 0)
      {
              sprintf(pPrtf->pszErrMsg,"DosRead failed:\n");
              cleanup(pPrtf,rc);
              return(TRUE);
      }
      if (pPrtf->pPrtfDat->lFirst == TRUE)
          pPrtf->pPrtfDat->lInputBufferRead += lInputBufferReadSave;
      pPrtf->pPrtfDat->lInputBufferPrinted = 0;
      pPrtf->pPrtfDat->lInputBufferPrintedPrevious = 0;
      pPrtf->pPrtfDat->lFirst = FALSE;
      if (pPrtf->pPrtfDat->lInputBufferRead == 0)
          return(FALSE);
      return(TRUE);
}

SHORT printhead(PPRINTFILE pPrtf)
{
     APIRET rc = 0;

     if (pPrtf->pPrtfDat->lOutputBufferLength)
     {
        rc=GpiCharString(pPrtf->pPrtfDat->hps, pPrtf->pPrtfDat->lOutputBufferLength,
                         pPrtf->pPrtfDat->pOutputBuffer);
        if (rc != GPI_OK)
        {
            sprintf(pPrtf->pszErrMsg,"CharString failed:\n");
            cleanup(pPrtf,rc);
            return(TRUE);
        }
        pPrtf->pPrtfDat->lOutputBufferLength=0;
     }
     if (pPrtf->pPrtfDat->pt.y < 0)
     {
         DevEscape( pPrtf->pPrtfDat->hdc,
                    DEVESC_NEWFRAME,
                    0L,
                    (PBYTE)NULL,
                    (PLONG)NULL,
                    (PBYTE)NULL );
        pPrtf->pPrtfDat->pt.x =  pPrtf->pPrtfDat->lxLeft;
        pPrtf->pPrtfDat->pt.y = pPrtf->pPrtfDat->sizel.cy-pPrtf->pPrtfDat->lLinefeed;
     }
     rc = GpiMove(pPrtf->pPrtfDat->hps, &(pPrtf->pPrtfDat->pt));
     if (rc != GPI_OK)
     {
         sprintf(pPrtf->pszErrMsg,"Move failed:\n");
         cleanup(pPrtf,rc);
         return(TRUE);
     }
     if (pPrtf->pPrtfDat->iMarginFlag)
     {
            pPrtf->pPrtfDat->iMarginFlag = 0;
            pPrtf->pPrtfDat->pOutputBuffer[pPrtf->pPrtfDat->lOutputBufferLength] = 
                   pPrtf->pPrtfDat->ucThis_char;
            pPrtf->pPrtfDat->lOutputBufferLength++;
            pPrtf->pPrtfDat->pt.x+=pPrtf->pPrtfDat->alWidths[pPrtf->pPrtfDat->ucThis_char];
            if (pPrtf->pPrtfDat->lOutputBufferLength >= MAX_OUTPUT_BUFFER) 
                 printhead(pPrtf);
     }
     return(0);
}

SHORT  newpage(PPRINTFILE pPrtf)
{
    CHAR     pszWork[CCHMAXPATH * 4];
    USHORT   usMemCopyLen = 0;
 
    pszWork[0] = '\0';
    pPrtf->pPrtfDat->lPage++;
    if (pPrtf->pszPrtTitle[0] != '\0')
        sprintf(pszWork,"%s  Page: %4i\x0d\x0a%s%s",
                pPrtf->pszPrtTitle,pPrtf->pPrtfDat->lPage,pPrtf->pszDocName,
                pPrtf->pszDocName[0] == '\0' ? " " : "\x0d\x0a\x0d\x0a");
    pPrtf->pPrtfDat->lTitleLen = strlen(pszWork);
    if (pPrtf->pPrtfDat->lTitleLen != 0) 
    {
       if (((pPrtf->pPrtfDat->lInputBufferPrinted - pPrtf->pPrtfDat->lInputBufferPrintedPrevious) <= pPrtf->pPrtfDat->lTitleLen) &&
             pPrtf->pPrtfDat->lInputBufferPrintedPrevious != 0)
       {
           sprintf(pPrtf->pszErrMsg,"Title doesn't fit in page. ");
           return(cleanup(pPrtf,1));
       }
       pPrtf->pPrtfDat->lInputBufferPrintedPrevious = pPrtf->pPrtfDat->lInputBufferPrinted;
       if ((pPrtf->pPrtfDat->lInputBufferRead + pPrtf->pPrtfDat->lTitleLen +
            pPrtf->pPrtfDat->lInputBufferRead -  pPrtf->pPrtfDat->lInputBufferPrinted) > (MAX_INPUT_BUFFER * 4))
       {
           sprintf(pPrtf->pszErrMsg,"Font or title is to large. Input buffer overflow. ");
           return(cleanup(pPrtf,1));
       }
       if (pPrtf->pPrtfDat->lInputBufferPrinted != 0)
       {
           memmove(&(pPrtf->pPrtfDat->pInputBuffer[pPrtf->pPrtfDat->lInputBufferPrinted  + pPrtf->pPrtfDat->lTitleLen]),
                   &(pPrtf->pPrtfDat->pInputBuffer[pPrtf->pPrtfDat->lInputBufferPrinted]),
                     pPrtf->pPrtfDat->lInputBufferRead -  pPrtf->pPrtfDat->lInputBufferPrinted <= 0 ?
                     0 : pPrtf->pPrtfDat->lInputBufferRead -  pPrtf->pPrtfDat->lInputBufferPrinted);
           memcpy(&(pPrtf->pPrtfDat->pInputBuffer
                   [pPrtf->pPrtfDat->lInputBufferPrinted   + (pPrtf->pPrtfDat->lInputBufferPrinted == 0 ? 0 : 1)]),
                    pszWork,pPrtf->pPrtfDat->lTitleLen);
       }
       else
       {
           memmove(&(pPrtf->pPrtfDat->pInputBuffer[pPrtf->pPrtfDat->lTitleLen]),
                     pPrtf->pPrtfDat->pInputBuffer,pPrtf->pPrtfDat->lTitleLen);
           memcpy(pPrtf->pPrtfDat->pInputBuffer,pszWork,pPrtf->pPrtfDat->lTitleLen);
       }
       pPrtf->pPrtfDat->lInputBufferRead = pPrtf->pPrtfDat->lInputBufferRead + pPrtf->pPrtfDat->lTitleLen;
    }
    return(0);
}

SHORT cleanup(PPRINTFILE pPrtf, APIRET rc)
{
   CHAR pWork[CCHMAXPATH * 2];
   ERRORID erridErrorCode;
 
   pPrtf->pPrtfDat->iErrFlag = SET;
   if (pPrtf->pPrtfDat->iCleanupDone == TRUE)
       return(FALSE);

   if (0 != (erridErrorCode = WinGetLastError(pPrtf->pPrtfDat->hab)))
   {
      sprintf(pWork,"WinGetLastError = %x\n",erridErrorCode);
      strcat(pPrtf->pszErrMsg,pWork);
      pWork[0] = '\0';
      switch (erridErrorCode&0xFFFF)
      {
         case PMERR_DC_IS_ASSOCIATED: sprintf(pWork,"Device context is associated.\n"); break;
         case PMERR_ESC_CODE_NOT_SUPPORTED: sprintf(pWork,"Escape code is not supported on DevEscape\n"); break;
         case PMERR_FONT_AND_MODE_MISMATCH: sprintf(pWork,"Font and mode mismatch.\n"); break;
         case PMERR_INV_DC_DATA: sprintf(pWork,"Invalid data on DevOpenDC.\n"); break;
         case PMERR_INV_DC_TYPE: sprintf(pWork,"Invalid type parameter on DevOpenDC.\n"); break;
         case PMERR_INV_DRIVER_NAME: sprintf(pWork,"Invalid driver name.\n"); break;
         case PMERR_INV_ESCAPE_DATA: sprintf(pWork,"Invalid data on DevEscape.\n"); break;
         case PMERR_DEV_FUNC_NOT_INSTALLED: sprintf(pWork,"Device function not supported by presentation driver.\n"); break;
         case PMERR_INV_HDC: sprintf(pWork,"Invalid device-context handle.\n"); break;
         case PMERR_INV_HPS: sprintf(pWork,"Invalid presentation-space handle.\n"); break;
         case PMERR_NOT_IN_IDX: sprintf(pWork,"Name not found, possible forms mismatch.\n"); break;
         case PMERR_INV_LENGTH_OR_COUNT: sprintf(pWork,"Invalid length or count.\n"); break;
         case PMERR_INV_LOGICAL_ADDRESS: sprintf(pWork,"An invalid device logical address.\n"); break;
         case PMERR_INV_OR_INCOMPAT_OPTIONS: sprintf(pWork,"An invalid or incompatible presentation space options.\n"); break;
         case PMERR_INV_PS_SIZE: sprintf(pWork,"Invalid size for presentation space.\n"); break;
         case PMERR_PS_BUSY: sprintf(pWork,"Presentation space is busy.\n"); break;
         case PMERR_SPL_PRINT_ABORT: sprintf(pWork,"Spooler print job has been aborted or redirected to a file.\n"); break;
      }
      strcat(pPrtf->pszErrMsg,pWork);
  }
  if (rc!=0)
  {
      sprintf(pWork,"RetCode = %d\n",rc);
      strcat(pPrtf->pszErrMsg,pWork);
      pWork[0] = '\0';
      switch(rc)
      {
         case ERROR_ACCESS_DENIED: sprintf(pWork,"Access is denied.\n"); break;
         case ERROR_BAD_NETPATH: sprintf(pWork,"The network path cannot be located.\n"); break;
         case ERROR_BUFFER_OVERFLOW: sprintf(pWork,"Buffer overflow.\n"); break;
         case ERROR_DEVICE_IN_USE: sprintf(pWork,"Device in use.\n"); break;
         case ERROR_DRIVE_LOCKED: printf("Drive locked.\n"); break;
         case ERROR_FILE_NOT_FOUND: sprintf(pWork,"File not found.\n"); break;
         case ERROR_FILENAME_EXCED_RANGE: sprintf(pWork,"Filename exceedes range.\n"); break;
         case ERROR_INVALID_ACCESS: sprintf(pWork,"Invalid access.\n"); break;
         case ERROR_INVALID_PARAMETER: sprintf(pWork,"Invalid parameter.\n"); break;
         case ERROR_MORE_DATA: sprintf(pWork,"Additional data is available, buffer to small.\n"); break;
         case ERROR_NOT_DOS_DISK: sprintf(pWork,"Not a DOS disk.\n"); break;
         case ERROR_NOT_SUPPORTED: sprintf(pWork,"Request not supported by the network.\n"); break;
         case ERROR_OPEN_FAILED: sprintf(pWork,"Open failed / file not found.\n"); break;
         case ERROR_PATH_NOT_FOUND: sprintf(pWork,"Path not found.\n"); break;
         case ERROR_SHARING_BUFFER_EXCEEDED: sprintf(pWork,"Sharing buffer exceeded.\n"); break;
         case ERROR_SHARING_VIOLATION: sprintf(pWork,"Sharing violation.\n"); break;
         case ERROR_TOO_MANY_OPEN_FILES: sprintf(pWork,"Too many open files.\n"); break;
         case 2150: sprintf(pWork,"The printer queue does not exist.\n"); break;
         case 2161: sprintf(pWork,"The spooler is not running.\n"); break;
      }
      strcat(pPrtf->pszErrMsg,pWork);
   }
   pPrtf->pPrtfDat->iCleanupDone = TRUE;
   SplCloseDevice(pPrtf);
   return(rc);
}

SHORT setdefaults(PPRINTFILE pPrtf)
{
   if (pPrtf->pszFaceName[0] == '\0')
      strcpy(pPrtf->pszFaceName,"Courier");
   if (pPrtf->fPointSize == 0)
       pPrtf->fPointSize = 12.0;
   if (pPrtf->lCopies == 0)
       pPrtf->lCopies = 1;
   return(0);
}  

SHORT copyright(void)
{
   return(0);
} 

double TwipsToFixed(FIXED fTwips)
{
   double dIntegerPart = 0 ;
   double dFractionalPart = 0;
   double dDivisor;
   double dRet;
   dIntegerPart = fTwips / 65536;
   dDivisor = fTwips - (dIntegerPart * 65536);
   if (dDivisor != 0)
      dFractionalPart =  (double) 100 / (double) 65536 * dDivisor;
   dRet =  dIntegerPart + (dFractionalPart / (double) 100);
   return(dRet);
}

double FixedToTwips(double dPointSize)
{
   int iIntegerPart = 0;
   double dFractionalPart = 0;
   double dRet;
 
   iIntegerPart = dPointSize;
   dFractionalPart = dPointSize - (double) iIntegerPart;
   dFractionalPart = ((double) 65536 / (double) 1) * dFractionalPart;
   dRet = iIntegerPart * 65536;
   dRet += dFractionalPart;
   return(dRet);
}

