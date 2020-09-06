; transmmx.asm - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
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

  extrn  G_pvTransferBuffer:dword

CODE32  segment

; extern VOID _System TransferBlock_MMX(PBYTE pbDst,
;                                       PBYTE pbSrc,
;                                       ULONG ulXBytes,
;                                       ULONG ulExtY,
;                                       LONG lAdditiveY);
  public  TransferBlock_MMX
  org  0
TransferBlock_MMX  proc  near
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
  call  near ptr FLAT:TransferScanline_MMX
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
TransferBlock_MMX  endp

; static VOID _Optlink TransferScanline_MMX(PBYTE pbDst,   /* eax */
;                                           PBYTE pbSrc,   /* edx */
;                                           ULONG ulBytes  /* ecx */);
  org  37h
TransferScanline_MMX  proc  near
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
  cmp  al, [edi+32]
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

  shr  ecx, 1
  jnc  short BLBL2
  mov  bl, [esi]
  inc  esi
  mov  [eax], bl
  inc  eax
BLBL2:
  shr  ecx, 1
  jnc  short BLBL3
  mov  bx, [esi]
  add  esi, 2
  mov  [eax], bx
  add  eax, 2
BLBL3:
  shr  ecx, 1
  jnc  short BLBL4
  mov  ebx, [esi]
  add  esi, 4
  mov  [eax], ebx
  add  eax, 4
BLBL4:
  test  ecx, ecx
  jz  short BLBL6
BLBL5:
  cmp  al, [esi+32]
  add  eax, 8
  movq  mm0, [esi]
  add  esi, 8
  dec  ecx
  movq  [eax-8], mm0
  jnz  short BLBL5
BLBL6:

  pop  edi
  pop  esi
  pop  ebx
  ret
TransferScanline_MMX  endp

CODE32  ends

  end

