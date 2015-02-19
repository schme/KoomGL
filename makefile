CC=cl
CFLAGS=-nologo -FC -Zi -EHsc -Oi -Od -WX -W4 -wd4201 -wd4189 -wd4100 -wd4127 -wd4996
LDFLAGS=
SRCF= src
SOURCES= $(SRCF)\win_platform.cpp $(SRCF)\game.cpp $(SRCF)\memorystack.cpp $(SRCF)\ks_glutils.cpp $(SRCF)\ks_objprs.cpp
DEFINES= -DGLEW_STATIC -DBUILD_INTERNAL -DENABLE_CONSOLE -DLOGLEVEL=1 -DVSYNC=0
INCLUDES= /I C:\dxsdk\Include
LIBS=user32.lib gdi32.lib opengl32.lib glu32.lib glew32s.lib ole32.lib winmm.lib

all: 
	$(CC) $(CFLAGS) $(LDFLAGS) $(INCLUDES) $(DEFINES) $(SOURCES) $(LIBS) -Fekoomgl

clean:
	del *.obj *.exe
