/* SSSBBACC.h - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
     Copyright (c) 2001 Takayuki 'January June' Suwa */

/* TRUE if locking for Ring0 is done  */
extern volatile BOOL G_bRing0Initialized;

/* GRADD ID of itself */
extern GID G_gidSelf;

/* previous HWEntry in GRADD chain */
extern PFNHWENTRY G_pfnPreviousChainedHWEntry;

/* TransferBlock entry */
extern VOID (* _System G_pfTransferBlock)(PBYTE, PBYTE, ULONG, ULONG, LONG);

/* pointer to TransferBlock buffer */
extern PVOID G_pvTransferBuffer;

/* global flags */
extern ULONG G_ulFlags;
#define FLAG_UNACCELERATED_GHI 0x00000001

/* GHI commands table */
extern const HWCMD G_axGHICommandTable[GHI_CMD_MAX];

/* GHI service routines & entry */
extern FNHWCMD HWQueryCaps;
extern FNHWCMD HWQueryModes;
extern FNHWENTRY HWEntry;

/* get CPUID feature value */
extern ULONG _Optlink IdentifyCpuFeature(VOID);
#define CPUFEATURE_FPU 0x00000001L  /* FPU on-chip */
#define CPUFEATURE_MMX 0x00800000L  /* MMX */
#define CPUFEATURE_SSE 0x02000000L  /* Streaming SIMD Extensions */

/* assembler TransferBlock subfunctions */
extern VOID _System TransferBlock_Generic(PBYTE pbDst,  /* 64bit FPU integer transfer */
                                          PBYTE pbSrc,
                                          ULONG ulXBytes,
                                          ULONG ulExtY,
                                          LONG lAdditiveY);
extern VOID _System TransferBlock_MMX(PBYTE pbDst,      /* 64bit MMX transfer */
                                      PBYTE pbSrc,
                                      ULONG ulXBytes,
                                      ULONG ulExtY,
                                      LONG lAdditiveY);
extern VOID _System TransferBlock_MMX2(PBYTE pbDst,      /* 64bit MMX transfer w/SSEint */
                                       PBYTE pbSrc,
                                       ULONG ulXBytes,
                                       ULONG ulExtY,
                                       LONG lAdditiveY);

