#ifndef _CEXCEPTION_H
#define _CEXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

//#include <avr/pgmspace.h>
#include <setjmp.h>
#include <stdint.h>
#include "ExceptionValues.h"
#ifdef __cplusplus
}
#endif

//To Use CException, you have a number of options:
//1. Just include it and run with the defaults
//2. Define any of the following symbols at the command line to override them
//3. Include a header file before CException.h everywhere which defines any of these
//4. Create an Exception.h in your path, and just define EXCEPTION_USE_CONFIG_FILE first

#ifdef CEXCEPTION_USE_CONFIG_FILE
#include "CExceptionConfig.h"
#endif

//This is the value to assign when there isn't an exception
#ifndef CEXCEPTION_NONE
#define CEXCEPTION_NONE      (0x5A5A)
#endif

//This is number of exception stacks to keep track of (one per task)
#ifndef CEXCEPTION_NUM_ID
#define CEXCEPTION_NUM_ID    (1) //there is only the one stack by default
#endif

//This is the method of getting the current exception stack index (0 if only one stack)
#ifndef CEXCEPTION_GET_ID
#define CEXCEPTION_GET_ID    (0) //use the first index always because there is only one anyway
#endif

//The type to use to store the exception values.
#ifndef CEXCEPTION_T
#define CEXCEPTION_T         uint16_t
#endif

#define CEXCEPTION_FILELINEDEFS     volatile char* ex_file; volatile uint16_t ex_line

// Declare below to get variables in your catch for the file and line number where
// the exception was thrown
//#define SAVE_FILE_AND_LINE

#ifdef __cplusplus
extern "C" {
#endif
//exception frame structures
typedef struct {
    jmp_buf *pFrame;
    volatile CEXCEPTION_T Exception;
#ifdef SAVE_FILE_AND_LINE
    volatile uint16_t Line;
    volatile char File[128];
#endif
} CEXCEPTION_FRAME_T;
#ifdef __cplusplus
}
#endif

//actual root frame storage (only one if single-tasking)
extern volatile CEXCEPTION_FRAME_T CExceptionFrames[];


#ifdef SAVE_FILE_AND_LINE
// Debug version of Try and Catch macros with file and line number
//Try (see C file for explanation)
#define Try                                                         \
    {                                                               \
        jmp_buf *PrevFrame, NewFrame;                               \
        unsigned int MY_ID = CEXCEPTION_GET_ID;                     \
        PrevFrame = CExceptionFrames[CEXCEPTION_GET_ID].pFrame;     \
        CExceptionFrames[MY_ID].pFrame = (jmp_buf*)(&NewFrame);     \
        CExceptionFrames[MY_ID].Exception = CEXCEPTION_NONE;        \
        CExceptionFrames[MY_ID].File[0] = 0;                        \
        CExceptionFrames[MY_ID].Line = 0;                           \
        if (setjmp(NewFrame) == 0) {                                \
            if (&PrevFrame)

//Use ex_file and ex_line to access file and line variables.
#define Catch(e)                                                    \
            else { }                                                \
            CExceptionFrames[MY_ID].Exception = CEXCEPTION_NONE;    \
            CExceptionFrames[MY_ID].File[0] = 0;                    \
            CExceptionFrames[MY_ID].Line = 0;                       \
        }                                                           \
        else                                                        \
        { e = CExceptionFrames[MY_ID].Exception; e=e;}              \
        CExceptionFrames[MY_ID].pFrame = PrevFrame;                 \
    }                                                               \
    if (CExceptionFrames[CEXCEPTION_GET_ID].Exception != CEXCEPTION_NONE)

//Catch (see C file for explanation)
//Use ex_file and ex_line to access file and line variables.
#define CatchFL(e)                                                  \
            else { }                                                \
            CExceptionFrames[MY_ID].Exception = CEXCEPTION_NONE;    \
            CExceptionFrames[MY_ID].File[0] = 0;                    \
            CExceptionFrames[MY_ID].Line = 0;                       \
        }                                                           \
        else                                                        \
        { e = CExceptionFrames[MY_ID].Exception; e=e;               \
          ex_file = CExceptionFrames[MY_ID].File; ex_file=ex_file;  \
          ex_line = CExceptionFrames[MY_ID].Line; ex_line=ex_line; }\
        CExceptionFrames[MY_ID].pFrame = PrevFrame;                 \
    }                                                               \
    if (CExceptionFrames[CEXCEPTION_GET_ID].Exception != CEXCEPTION_NONE)

#else
//Standard version without file and line number
//Try (see C file for explanation)
#define Try                                                         \
    {                                                               \
        jmp_buf *PrevFrame, NewFrame;                               \
        unsigned int MY_ID = CEXCEPTION_GET_ID;                     \
        PrevFrame = CExceptionFrames[CEXCEPTION_GET_ID].pFrame;     \
        CExceptionFrames[MY_ID].pFrame = (jmp_buf*)(&NewFrame);     \
        CExceptionFrames[MY_ID].Exception = CEXCEPTION_NONE;        \
        if (setjmp(NewFrame) == 0) {                                \
            if (&PrevFrame)


//Catch (see C file for explanation)
#define Catch(e)                                                    \
            else { }                                                \
            CExceptionFrames[MY_ID].Exception = CEXCEPTION_NONE;    \
        }                                                           \
        else                                                        \
        { e = CExceptionFrames[MY_ID].Exception; e=e; }             \
        CExceptionFrames[MY_ID].pFrame = PrevFrame;                 \
    }                                                               \
    if (CExceptionFrames[CEXCEPTION_GET_ID].Exception != CEXCEPTION_NONE)

#endif

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

//Throw an Error
#ifdef SAVE_FILE_AND_LINE
EXTERNC void ThrowFunc(CEXCEPTION_T ExceptionID, const char* File, uint16_t Line );
#define Throw(e)        ThrowFunc(e, __BASE_FILE__, __LINE__)
#else
EXTERNC void ThrowFunc(CEXCEPTION_T ExceptionID);
#define Throw(e)        ThrowFunc(e)
#endif

#undef EXTERNC
#endif // _CEXCEPTION_H
