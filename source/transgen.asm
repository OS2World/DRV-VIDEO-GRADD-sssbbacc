; transgen.asm - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
;   Copyright (c) 2001 Takayuki 'January June' Suwa

  .386
  .387

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

; extern VOID _System TransferBlock_Generic(PBYTE pbDst,
;                                           PBYTE pbSrc,
;                                           ULONG ulXBytes,
;                                           ULONG ulExtY,
;                                           LONG lAdditiveY);
  public  TransferBlock_Generic
  org  0
TransferBlock_Generic  proc  near
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
  call  near ptr FLAT:TransferScanline_Generic
  add  ebx, ARG.lAdditiveY
  add  esi, ARG.lAdditiveY
  dec  edi
  jne  short BLBL0

  pop  edi
  pop  esi
  pop  ebx
  leave
  ret
TransferBlock_Generic  endp

; static VOID _Optlink TransferScanline_Generic(PBYTE pbDst,   /* eax */
;                                               PBYTE pbSrc,   /* edx */
;                                               ULONG ulBytes  /* ecx */);
  org  37h
TransferScanline_Generic  proc  near
  push  ebx
  push  esi
  push  edi

  lea  ebx, [ecx+edx+7]
  mov  esi, not 7
  and  esi, edx
  sub  ebx, esi

  shr  ebx, 4
  mov  edi, FLAT:G_pvTransferBuffer
  jnc  short BLBL0
  fild  qword ptr [esi]
  add  esi, 8
  fistp  qword ptr [edi]
  add  edi, 8
  test  ebx, ebx
  jz  short BLBL1
BLBL0:
  cmp  al, [edi+32]
  add  edi, 16
  fild  qword ptr [esi+8]
  fild  qword ptr [esi]
  add  esi, 16
  dec  ebx
  fistp  qword ptr [edi-16]
  fistp  qword ptr [edi-8]
  jnz  short BLBL0
BLBL1:

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
  shr  ecx, 1
  jnc  short BLBL5
  fild  qword ptr [esi]
  add  esi, 8
  fistp  qword ptr [eax]
  add  eax, 8
BLBL5:
  test  ecx, ecx
  jz  short BLBL7
BLBL6:
  cmp  al, [esi+32]
  add  eax, 16
  fild  qword ptr [esi+8]
  fild  qword ptr [esi]
  add  esi, 16
  dec  ecx
  fistp  qword ptr [eax-16]
  fistp  qword ptr [eax-8]
  jnz  short BLBL6
BLBL7:

  pop  edi
  pop  esi
  pop  ebx
  ret
TransferScanline_Generic  endp

CODE32  ends

  end

