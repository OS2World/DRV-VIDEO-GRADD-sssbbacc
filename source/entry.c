/* entry.c - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
     Copyright (c) 2001 Takayuki 'January June' Suwa */

#pragma strings(readonly)

#define INCL_BASE
#define INCL_PM
#define INCL_ERRORS
#include <os2.h>
#include <gradd.h>
#include "SSSBBACC.h"

/*  */
static VOID LockMemoryBlock(PVOID pvAddress,
                            BOOL bWritable)
{
    ULONG ulSize;
    ULONG ulFlags;
    pvAddress = (PVOID)((ULONG)pvAddress & ~(ULONG)4095);
    for(;;)
    {
        ulSize = (ULONG)4096;
        (VOID)DosQueryMem(pvAddress,
                          &ulSize,
                          &ulFlags);
        if((ulFlags & PAG_FREE) != (ULONG)0)
            return;
        if((ulFlags & PAG_BASE) != (ULONG)0)
            break;
        pvAddress = (PVOID)((ULONG)pvAddress - (ULONG)4096);
    }
    ulSize = (ULONG)-1;
    (VOID)DosQueryMem(pvAddress,
                      &ulSize,
                      &ulFlags);
    (VOID)VHLockMem(pvAddress,
                    ulSize,
                    bWritable);
}

/*  */
static ULONG FilteredBitBlt(PVOID pIn,
                            PVOID pOut)
{
#define pxBITBLTINFO ((PBITBLTINFO)pIn)
    ULONG ulIndex;
    PBMAPINFO pxBitmap;
    LONG lAdditiveY;
    ULONG ulSrcX, ulSrcY;
    ULONG ulDstX, ulDstY;
    ULONG ulExtX, ulExtY;
    if((pxBITBLTINFO->pDstBmapInfo->ulType & BMAP_MEMORY) == BMAP_VRAM &&                           /* destination is VRAM */
       (pxBITBLTINFO->ulBltFlags & ~(BF_DIR_X_NEGATIVE | BF_DIR_Y_NEGATIVE)) == BF_ROP_INCL_SRC &&  /* operation has source and no special */
       (pxBITBLTINFO->pSrcBmapInfo->ulType & BMAP_MEMORY) == BMAP_VRAM &&                           /* source is VRAM */
       pxBITBLTINFO->ulROP == ROP_SRCCOPY &&                                                        /* operation is simple copy */
       pxBITBLTINFO->pDstBmapInfo->ulBpp >= (ULONG)8)                                               /* destination BPP >= 8 */
    {
        pxBitmap = pxBITBLTINFO->pDstBmapInfo;
        lAdditiveY = (LONG)pxBitmap->ulBytesPerLine;
        if((pxBITBLTINFO->ulBltFlags & BF_DIR_Y_NEGATIVE) != (ULONG)0)
            lAdditiveY = -lAdditiveY;
        for(ulIndex = (ULONG)0;
            ulIndex < pxBITBLTINFO->cBlits;
            ulIndex++)
        {
            ulSrcX = (ULONG)pxBITBLTINFO->aptlSrcOrg[ulIndex].x; ulSrcY = (ULONG)pxBITBLTINFO->aptlSrcOrg[ulIndex].y;
            ulDstX = pxBITBLTINFO->abrDst[ulIndex].ulXOrg; ulDstY = pxBITBLTINFO->abrDst[ulIndex].ulYOrg;
            ulExtX = pxBITBLTINFO->abrDst[ulIndex].ulXExt; ulExtY = pxBITBLTINFO->abrDst[ulIndex].ulYExt;
            if((pxBITBLTINFO->ulBltFlags & BF_DIR_X_NEGATIVE) != (ULONG)0)
            {
                ulSrcX -= ulExtX - (ULONG)1;
                ulDstX -= ulExtX - (ULONG)1;
            }
            G_pfTransferBlock(&pxBitmap->pBits[pxBitmap->ulBpp / (ULONG)8 * ulDstX + pxBitmap->ulBytesPerLine * ulDstY],
                              &pxBitmap->pBits[pxBitmap->ulBpp / (ULONG)8 * ulSrcX + pxBitmap->ulBytesPerLine * ulSrcY],
                              pxBitmap->ulBpp / (ULONG)8 * ulExtX,
                              ulExtY,
                              lAdditiveY);
        }
        return RC_SUCCESS;
    }
    return RC_SIMULATE;
#undef pxBITBLTINFO
}

/* GHI command entry */
ULONG HWEntry(GID gid,
              ULONG ulFunction,
              PVOID pIn,
              PVOID pOut)
{
    ULONG ulReturn = RC_ERROR;
    ULONG ulCpuFeature;
    if(ulFunction == GHI_CMD_INIT)
    {
#define pxGDDInitIn ((PGDDINITIN)pIn)
#define pxGDDInitOut ((PGDDINITOUT)pOut)
        if(pxGDDInitIn->pfnChainedHWEntry != (PFNHWENTRY)NULL)
        {
            ulCpuFeature = IdentifyCpuFeature();
            if((ulCpuFeature & CPUFEATURE_FPU) != (ULONG)0)
            {
                if(G_bRing0Initialized == (BOOL)FALSE)
                {
                    LockMemoryBlock((PVOID)&G_bRing0Initialized,
                                    (BOOL)TRUE);
                    LockMemoryBlock((PVOID)HWEntry,
                                    (BOOL)FALSE);
                    G_bRing0Initialized = (BOOL)TRUE;
                }
                if(DosScanEnv((PSZ)"SSSBBACC_NOSSEINT",
                              (PSZ*)&ulReturn) == NO_ERROR)
                    ulCpuFeature &= ~CPUFEATURE_SSE;
                if(DosScanEnv((PSZ)"SSSBBACC_NOMMX",
                              (PSZ*)&ulReturn) == NO_ERROR)
                    ulCpuFeature &= ~CPUFEATURE_MMX;
                G_pfTransferBlock = TransferBlock_Generic;
                if((ulCpuFeature & CPUFEATURE_SSE) != (ULONG)0)
                    G_pfTransferBlock = TransferBlock_MMX2;
                else if((ulCpuFeature & CPUFEATURE_MMX) != (ULONG)0)
                    G_pfTransferBlock = TransferBlock_MMX;
                G_pvTransferBuffer = VHAllocMem((ULONG)16384);
                if(DosScanEnv((PSZ)"SSSBBACC_UNACCELERATED_GHI",
                              (PSZ*)&ulReturn) == NO_ERROR)
                    G_ulFlags |= FLAG_UNACCELERATED_GHI;
                G_gidSelf = gid;
                G_pfnPreviousChainedHWEntry = pxGDDInitIn->pfnChainedHWEntry;
                ulReturn = RC_SUCCESS;
            }
        }
#undef pxGDDInitOut
#undef pxGDDInitIn
    }
    else
        if(gid == G_gidSelf)
        {
            if(ulFunction < GHI_CMD_MAX)
                ulReturn = G_axGHICommandTable[ulFunction].pfnHWCmd != (FNHWCMD*)NULL ? G_axGHICommandTable[ulFunction].pfnHWCmd(pIn,
                                                                                                                                 pOut)
                                                                                      : G_axGHICommandTable[ulFunction].ulRc;
        }
        else
        {
            if((G_ulFlags & FLAG_UNACCELERATED_GHI) != (ULONG)0)
            {
                switch(ulFunction)
                {
                    case GHI_CMD_BITBLT:
                        ulReturn = FilteredBitBlt(pIn,
                                                  pOut);
                        break;
                }
                if(ulReturn != RC_SUCCESS)
                    ulReturn = G_pfnPreviousChainedHWEntry(gid,
                                                           ulFunction,
                                                           pIn,
                                                           pOut);
            }
            else
            {
                ulReturn = G_pfnPreviousChainedHWEntry(gid,
                                                       ulFunction,
                                                       pIn,
                                                       pOut);
                if(ulReturn == RC_SIMULATE)
                    switch(ulFunction)
                    {
                        case GHI_CMD_BITBLT:
                            ulReturn = FilteredBitBlt(pIn,
                                                      pOut);
                            break;
                    }
            }
        }
    return ulReturn;
}

