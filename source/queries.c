/* queries.c - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
     Copyright (c) 2001 Takayuki 'January June' Suwa */

#pragma strings(readonly)

#define INCL_BASE
#define INCL_PM
#define INCL_ERRORS
#include <os2.h>
#include <gradd.h>
#include "SSSBBACC.h"

/*  */
ULONG HWQueryCaps(PVOID pIn,
                  PVOID pOut)
{
#define pxCapsInfo ((PCAPSINFO)pOut)
    pxCapsInfo->pszFunctionClassID = BASE_FUNCTION_CLASS;
    pxCapsInfo->ulFCFlags = (ULONG)0;
    return RC_SUCCESS;
#undef pxCapsInfo
}

/*  */
ULONG HWQueryModes(PVOID pIn,
                   PVOID pOut)
{
    switch((ULONG)pIn)
    {
        case QUERYMODE_NUM_MODES:
#define pulSupportedModes ((PULONG)pOut)
            *pulSupportedModes = (ULONG)0;
            break;
#undef pulSupportedModes
        case QUERYMODE_MODE_DATA:
#define pxModeInfo ((PGDDMODEINFO)pOut)
            break;
#undef pxModeInfo
    }
    return RC_SUCCESS;
}

