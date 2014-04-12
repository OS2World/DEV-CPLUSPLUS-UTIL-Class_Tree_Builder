##
## Module   :MAKEFILE
##
## Copyright (C) Sergey I. Yevtushenko
##


INCLUDE   = ..\include
OUTPUT    = ..\exe
.path.h   = $(INCLUDE)
.path.hpp = $(INCLUDE)
.path.exe = $(OUTPUT)

OBJS = ctb.obj stream.obj tstream.obj table.obj
CC   = hc -O7 -I..\include -Hpfnoflt -486

.cpp.obj :
        $(CC) -c {$< }

ctb.exe : $(OBJS)
        -$(CC) -o $(OUTPUT)\ctb.exe $(OBJS)

ctb.obj    : dlc.h     ctb.cpp
stream.obj : stream.hpp stream.cpp
tstream.obj: dlc.h      tstream.cpp
table.obj  : table.h    table.cpp

