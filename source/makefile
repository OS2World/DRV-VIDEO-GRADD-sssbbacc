# makefile - Software Screen to Screen BitBlt Acceleration GRADD filter, release 0.0.6
#   Copyright (c) 2001 Takayuki 'January June' Suwa

SSSBBACC.sym : SSSBBACC.map
    mapsym -a SSSBBACC.map

..\SSSBBACC.dll SSSBBACC.map : SSSBBACC.obj trcommon.obj transgen.obj transmmx.obj transmm2.obj SSSBBACC.def
    link386 /nodefaultlibrarysearch /noignorecase /packcode /packdata /map /nologo trcommon.obj+transgen.obj+transmmx.obj+transmm2.obj+SSSBBACC.obj, ..\SSSBBACC.dll, SSSBBACC.map, os2386.lib, SSSBBACC.def
    eautil ..\SSSBBACC.dll nul /s
    lxlite ..\SSSBBACC.dll /b- /c+ /i- /l- /mr3 /ml1 /mf1 /s+ /t: /v- /yur /ydd /yxs /ynl /zs:1024

SSSBBACC.obj : data.w entry.w queries.w SSSBBACC.def
    icc /C+ /Ss+ /G5 /Gs+ /Gi+ /Gf+ /Gu+ /O+ /Oc+ /Oi+ /Ol+ /Rn /Q+ /FoSSSBBACC.obj data.w entry.w queries.w SSSBBACC.def

data.w : data.c SSSBBACC.h
    icc /Fw+ /Ss+ /IfromIBM /G5 /Gs+ /Gi+ /Gf+ /Gu- /O+ /Oc+ /Oi+ /Ol- /Rn /Q+ data.c

entry.w : entry.c SSSBBACC.h
    icc /Fw+ /Ss+ /IfromIBM /G5 /Gs+ /Gi+ /Gf+ /Gu+ /O+ /Oc+ /Oi+ /Ol- /Rn /Q+ entry.c

queries.w : queries.c SSSBBACC.h
    icc /Fw+ /Ss+ /IfromIBM /G5 /Gs+ /Gi+ /Gf+ /Gu+ /O+ /Oc+ /Oi+ /Ol- /Rn /Q+ queries.c

trcommon.obj : trcommon.asm
    alp -Mb trcommon.asm

transgen.obj : transgen.asm
    alp -Mb transgen.asm

transmmx.obj : transmmx.asm
    alp -Mb transmmx.asm

transmm2.obj : transmm2.asm
    alp -Mb transmm2.asm

