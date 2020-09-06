; transmm2.asm - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
;   Copyright (c) 2001 Takayuki 'January June' Suwa

  .586
  .mmx

CODE32  segment  para use32 public 'CODE'
CODE32  ends
DATA32  segment  para use32 public 'DATA'
DATA32  ends
CONST32_RO  segment  para use32 public 'CONST'
CONST32_RO  ends
BSS32  segment  para use32 public 'BSS'
BSS32  ends
DGROUP  group  BSS32, DATA32
  assume  cs:FLAT, ds:FLAT, ss:FLAT, es:FLAT, fs:nothing, gs:nothing

CONST32_RO  segment  para use32 public 'CONST'

ByteMask  db  7 dup(80h), 8 dup(0)

CONST32_RO  ends

  extrn  G_pvTransferBuffer:dword

CODE32  segment

; extern VOID _System TransferBlock_MMX2(PBYTE pbDst,
;                                        PBYTE pbSrc,
;                                        ULONG ulXBytes,
;                                        ULONG ulExtY,
;                                        LONG lAdditiveY);
  public  TransferBlock_MMX2
  org  0
TransferBlock_MMX2  proc  near
ARGLIST  struc
pbDst       dd  ?  ; (PBYTE)
pbSrc       dd  ?  ; (PBYTE)
ulXBytes    dd  ?  ; (ULONG)
ulExtY      dd  ?  ; (ULONG)
lAdditiveY  dd  ?  ; (LONG)
ARGLIST  ends
VARLIST  struc
VARLIST  ends
  enter size VARLIST, 0
ARG  equ  ARGLIST[ebp+8]
VAR  equ  VARLIST[ebp-size VARLIST]
  push  ebx
  push  esi
  push  edi

  mov  ebx, ARG.pbDst
  mov  esi, ARG.pbSrc
  mov  edi, ARG.ulExtY
BLBL0:
  mov  eax, ebx
  mov  edx, esi
  mov  ecx, ARG.ulXBytes
  call  near ptr FLAT:TransferScanline_MMX2
  add  ebx, ARG.lAdditiveY
  add  esi, ARG.lAdditiveY
  dec  edi
  jne  short BLBL0
  emms

  pop  edi
  pop  esi
  pop  ebx
  leave
  ret
TransferBlock_MMX2  endp

; static VOID _Optlink TransferScanline_MMX2(PBYTE pbDst,   /* eax */
;                                            PBYTE pbSrc,   /* edx */
;                                            ULONG ulBytes  /* ecx */);
  org  3ah
TransferScanline_MMX2  proc  near
  push  ebx
  push  esi
  push  edi

  lea  ebx, [ecx+edx+7]
  mov  esi, not 7
  and  esi, edx
  sub  ebx, esi

  shr  ebx, 3
  mov  edi, FLAT:G_pvTransferBuffer
BLBL0:
  db  0fh, 18h, 4fh, 20h  ; prefetcht0  [edi+32]
  add  edi, 8
  movq  mm0, [esi]
  add  esi, 8
  dec  ebx
  movq  [edi-8], mm0
  jnz  short BLBL0

  mov  esi, 7
  and  esi, edx
  mov  edx, 7
  add  esi, FLAT:G_pvTransferBuffer
  and  edx, ecx
  shr  ecx, 3
  jz  short BLBL2
BLBL1:
  db  0fh, 18h, 04eh, 20h  ; prefetcht0  [esi+32]
  add  eax, 8
  movq  mm0, [esi]
  add  esi, 8
  dec  ecx
  db  0fh, 0e7h, 40h, 0f8h  ; movntq  [eax-8], mm0
  jnz  short BLBL1
BLBL2:
  mov  edi, eax
  mov  eax, offset FLAT:ByteMask+7
  sub  eax, edx
  movq  mm0, [esi]
  movq  mm1, [eax]
  db  0fh, 0f7h, 0c1h  ; maskmovq  mm0, mm1

  pop  edi
  pop  esi
  pop  ebx
  ret
TransferScanline_MMX2  endp

CODE32  ends

  end

