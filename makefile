CC=cl
# -Od for no optimizations, -Ox for max
CFLAGS=-nologo -FC -Zi -EHsc -Oi -Ox -WX -W4 -wd4201 -wd4189 -wd4100 -wd4127 -wd4996 -fp:fast
LDFLAGS=
SRCF= src
SOURCES= $(SRCF)\win_platform.cpp $(SRCF)\render.cpp $(SRCF)\mem_arena.cpp $(SRCF)\ks_glutils.cpp
DEFINES= -DGLEW_STATIC -DBUILD_INTERNAL -DENABLE_CONSOLE -DLOGLEVEL=1 -DVSYNC=0
INCLUDES= /I C:\dxsdk\Include
LIBS=user32.lib gdi32.lib opengl32.lib glu32.lib glew32s.lib ole32.lib winmm.lib

all:
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFINES) $(SOURCES) $(LIBS) -Fekoomgl

clean:
	del *.obj *.exe
