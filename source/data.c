/* data.c - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
     Copyright (c) 2001 Takayuki 'January June' Suwa */

#pragma strings(readonly)

#define INCL_BASE
#define INCL_PM
#define INCL_ERRORS
#include <os2.h>
#include <gradd.h>
#include "SSSBBACC.h"

/* TRUE if locking for Ring0 is done  */
volatile BOOL G_bRing0Initialized;

/* GRADD ID of itself */
GID G_gidSelf;

/* previous HWEntry in GRADD chain */
PFNHWENTRY G_pfnPreviousChainedHWEntry;

/* TransferBlock entry */
VOID (* _System G_pfTransferBlock)(PBYTE, PBYTE, ULONG, ULONG, LONG);

/* pointer to TransferBlock buffer */
PVOID G_pvTransferBuffer;

/* global flags */
ULONG G_ulFlags;

/* GHI commands table */
const HWCMD G_axGHICommandTable[GHI_CMD_MAX] =
 {{NULL, RC_SUCCESS},        /* GHI_CMD_INIT */
  {NULL, RC_SUCCESS},        /* GHI_CMD_INITPROC */
  {NULL, RC_SUCCESS},        /* GHI_CMD_TERM */
  {NULL, RC_SUCCESS},        /* GHI_CMD_TERMPROC */
  {HWQueryCaps, DONTCARE},   /* GHI_CMD_QUERYCAPS */
  {HWQueryModes, DONTCARE},  /* GHI_CMD_QUERYMODES */
  {NULL, RC_ERROR},          /* GHI_CMD_SETMODE */
  {NULL, RC_ERROR},          /* GHI_CMD_PALETTE */
  {NULL, RC_ERROR},          /* GHI_CMD_BITBLT */
  {NULL, RC_ERROR},          /* GHI_CMD_LINE */
  {NULL, RC_ERROR},          /* GHI_CMD_MOVEPTR */
  {NULL, RC_ERROR},          /* GHI_CMD_SETPTR */
  {NULL, RC_ERROR},          /* GHI_CMD_SHOWPTR */
  {NULL, RC_ERROR},          /* GHI_CMD_VRAM */
  {NULL, RC_ERROR},          /* GHI_CMD_REQUESTHW */
  {NULL, RC_ERROR},          /* GHI_CMD_EVENT */
  {NULL, RC_ERROR},          /* GHI_CMD_EXTENSION */
  {NULL, RC_ERROR},          /* GHI_CMD_BANK */
  {NULL, RC_ERROR},          /* GHI_CMD_TEXT */
  {NULL, RC_ERROR}           /* GHI_CMD_USERCAPS */};

