/* Orginal scriptheader

Trivial script to load/save current contents of the kernel clock
from/to a file. Helpful as a *bootstrap* clock on machines where
there isn't a useful RTC driver (e.g. on development boards). Using
NTP is still recommended on these machines to get to real time sync
once more of the system is up and running.

Copyright 2012-2016 Steve McIntyre <93sam@debian.org>

License: GPLv2

#-[ export Readme ]------------------------------------------------------------

does the same as the bash script of the same name, but faster

this is a complete rewrite in c. absolutly near by the orginal script

this is written 10'2022 by Heiko Stoevesandt <hstools "ta" t-online.de>
orginal this was written as a bash-script by Steve McIntyre

SYNOPSIS
 fake-hwclock [ command ] [ force ]
command : save or load (or empty to set command to save)

more can be found at man(8) fake-hwclock

The Orginal have not really a usable return value
fake-hwclock has now following return codes

RT | Description
---|----------------
 0 | no error (save or load)
 1 | save error
 2 | don't save fake-hwclock with time before HWCLOCK_EPOCH_SEC ; use force to overwrite
 3 | load error ( file not found )
 4 | load error ( file can't read )
 5 | load error ( size error )
 6 | don't load - clock is in the past
 7 | --
 8 | --
 9 | command not found
10 | settimeofday error

#-[ EOF Readme ]-----------[ Revision ]-----------------------------------------

 26.11.22 HS local_strptime aendert den uebergebenen string -> jetzt eine Kopie davon
             Datum verkehrt herum abgefragt
			 
------------------------------------------------------------------------------*/

#ifdef __ARM_FP
#ifdef __linux__
    #define OS_RASPBERRY
    #define OS_VERSION "Raspberry"
    #define DFLT_ETC "/etc/"
#endif
#endif

#ifndef OS_RASBERRY
#ifdef HS_DEBUG
#ifdef __WIN32
    #define OS_WINDOWS
    #define OS_VERSION "Windows"
    #define DFLT_ETC "C:\\hs\\etc\\"
#endif
#endif
#endif // raspberry

#if !defined(OS_RASPBERRY) && !defined(OS_WINDOWS)
    #error "NO valid OS found. eg. Windows if NoDebug"
#endif

#include <stdio.h>
#ifndef _MSC_VER
//include/stdlib.h included wird im Präerkennungsmode für Codeblocks durchlaufen wird
#include <../include/stdlib.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

//nice to have
#include "VERSION.h"

char     tBuf[22];
char     *tFormat="%Y-%m-%d %H:%M:%S";

// write t [time_t] to tBuf
char *useTime(const time_t t)
{
    struct tm *tnow;
    tnow = gmtime(&t);
    strftime(tBuf, sizeof(tBuf)-1, tFormat, tnow);
    return tBuf;
}

// 2022-11-22 23:12:43
// 0123456789012345678
// convert the time ti time_t; this is not the best method, but why not
time_t local_strptime(char *src, const char *dummy, struct tm *savedtime)
{
    memset (savedtime, 0, sizeof(struct tm));
    src[4]=0;
    src[7]=0;
    src[10]=0;
    src[13]=0;
    src[16]=0;
    savedtime->tm_year  = atoi(src)-1900;
    savedtime->tm_mon   = atoi(&src[5])-1;
    savedtime->tm_mday  = atoi(&src[8]);
    savedtime->tm_hour  = atoi(&src[11]);
    savedtime->tm_min   = atoi(&src[14]);
    savedtime->tm_sec   = atoi(&src[17]);
#ifdef OS_RASPBERRY
    return timegm(savedtime);
#endif
#ifdef OS_WINDOWS
    return _mkgmtime(savedtime);
#endif
}

// show if a file exists if true, then return true (1) else false (0)
int fexists(const char *name)
{
    struct stat buf;
    if (stat (name ,&buf )==-1) return 0;
    if (!S_ISREG( buf.st_mode )) return 0;
    return 1;
}

// FILE /etc/fake-hwclock.data write
int fout(const char *fname, const char *txt)
{
    FILE *OUTPTR;
    if ((OUTPTR=fopen(fname, "wt"))==NULL) return EXIT_FAILURE;
    fprintf (OUTPTR,"%s\n", txt);
    fclose(OUTPTR);
    return EXIT_SUCCESS;
}

// FILE /etc/fake-hwclock.data read to txt
int fin(const char *fname, char *txt)
{
    FILE *INPTR;
    int sz;
    if ((INPTR=fopen(fname, "rt"))==NULL) return 4;
    sz=fread(txt,1,19,INPTR);
    fclose(INPTR);
    if (sz!=19) return 5;
    return EXIT_SUCCESS;
}

//-------- MAIN --------
signed int main(int argc, char *argv[])
{
    char      *FILE;                        // filename for like /etc/fake-hwclock.data
    time_t     HWCLOCK_EPOCH_SEC;           // date hardcoded secs for date 2016-04-15 00:00:00
    time_t     NOW_SEC;                     // secs for $NOW; then time readed from system ( this can be 1.1.1970 )
    char       SAVED[32];                   // date read from FILE
    char       cSAVED[32];                  // copy of SAVED
    struct tm  savedtime;                   // date read from FILE converted in this struct to create SAVED_SEC
    time_t     SAVED_SEC;                   // seconds from FILE
    struct timespec tsp;                    // struct to write wit clock_settime
    char      *COMMAND;                     // from the user (script) input
    int        FORCE;                       // some times out of range, FORCE will overwrite this an demand the COMMAND
    int        rc;                          // internal use for result-codes

    FILE = getenv("FILE");                  // FILE defined? maybe for testing
    if (FILE==NULL) FILE=DFLT_ETC "fake-hwclock.data"; // else we go it from this location
    HWCLOCK_EPOCH_SEC = 1460678400ll;       // this is apr.15 2016 0000
    NOW_SEC = time(NULL);                   // UTC for now

    if (argc>=2) COMMAND = argv[1];
    else         COMMAND = "save";          // default command is save

    FORCE = 0;                              // normal we don't need this
    if (argc>=3)
    {
        if (!strcmp(argv[2],"force"))       // if force then FORCE=1
        {
            FORCE = 1;
        }
    }

    // we will save this time ?
    if (!strcmp(COMMAND,"save"))
    {
        if (fexists(FILE))
        {
            if (!FORCE)
            {
                if (NOW_SEC < HWCLOCK_EPOCH_SEC )
                {
                    printf ("Time travel detected!\n");
                    printf ("fake-hwclock release date is in the future: %s\n", useTime(HWCLOCK_EPOCH_SEC));
                    printf ("Current system time: %s\n", useTime(NOW_SEC));
                    printf ("To force the saved system clock backwards in time anyway, use \"force\"");
                    return 2;
                }
            }
        }
        // date -u '+%Y-%m-%d %H:%M:%S' > $FILE
        return fout (FILE, useTime(NOW_SEC));
    }
    // we need to get time for this system
    if (!strcmp(COMMAND,"load"))
    {
        if (!fexists(FILE))
        {
            printf("Unable to read saved clock information: %s does not exist",FILE );
            return 3;
        }
        if ((rc = fin(FILE, SAVED)))
        {
            if (rc==4) printf ("open error at %s\n", FILE);
            if (rc==5) printf ("size error in %s\n", FILE);
            return rc;
        }
        strcpy (cSAVED, SAVED);
        SAVED_SEC = local_strptime(cSAVED, tFormat, &savedtime);
        if (!FORCE)
        {
            if (NOW_SEC > SAVED_SEC)
            {
                printf ("Current system time: %s\n", useTime(NOW_SEC));
                printf ("fake-hwclock saved clock information is in the past: %s\n", SAVED);
                printf ("To set system time to this saved clock anyway, use \"force\"\n");
                return 6;
            }
        }
        // setclock
        tsp.tv_sec = SAVED_SEC;
        tsp.tv_nsec= 0;
#ifdef OS_WINDOWS
        rc = tsp.tv_sec % 256;
        printf ("(windows) fake set time to UTC \"%s\"\n",useTime(SAVED_SEC));
        rc = 0;
#endif
#ifdef OS_RASPBERRY
        // this works!! but if the systemd-timesyncd running, this will be overwritten
        rc = clock_settime(CLOCK_REALTIME, &tsp);
#endif // OS_RASPBERRY
        if (rc) printf ("error while settime rc=%i errno=%i\n", rc, errno);
        return rc;
    }
    // thats for me to determinate the Version
    if (!strcmp(COMMAND,"--version"))
    {
        printf ("fake-hwclock V%u.%u.%u %s %s (w) by Hesti  ", I_MAJOR, I_MINOR, I_BUILD,
#if ( (defined _M_X64) || (defined _WIN64) )
        "X64"
#else
        "X86"
#endif
        ,I_BETA);
#ifdef __GNUC__
#define COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define COMPILER "GnuCC"
#else
#define COMPILER_VERSION 0
#define COMPILER "unknown "
#endif // __GNUC__
        printf ("Compiled %s V%i.%02i.%02i (%s) at  : %s %s\n", COMPILER, COMPILER_VERSION / 10000, (COMPILER_VERSION / 100) % 100,COMPILER_VERSION % 100, OS_VERSION, __DATE__, __TIME__);
        return 0;
    }
    printf("%s: Unknown command %s\n",argv[0],COMMAND);
    return 9;
}
