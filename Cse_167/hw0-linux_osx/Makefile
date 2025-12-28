CC = g++
ifeq ($(shell uname 2>/dev/null), Darwin)
CFLAGS = -g -DGL_GLEXT_PROTOTYPES -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -DOSX -Wno-deprecated-register -Wno-deprecated-declarations -Wno-shift-op-parentheses -Wno-parentheses-equality
INCFLAGS = -I./glm-0.9.7.1 -I/usr/X11/include -I./include/
LDFLAGS = -framework GLUT -framework OpenGL -L./lib/mac/ \
		-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
		-lm -lstdc++ -lfreeimage 
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES 
INCFLAGS = -I./glm-0.9.7.1 -I/opt/local/include -I./include/

LDFLAGS = -framework GLUT -framework OpenGL -L/opt/local/lib \
  -L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
  -lGL -lGLU -lm -lstdc++ -lfreeimage
endif
RM = /bin/rm -f
all: mytest3
mytest3: mytest3.o shaders.o shaders.h geometry3.h
	$(CC) $(CFLAGS) -o mytest3 shaders.o mytest3.o $(INCFLAGS) $(LDFLAGS) 
mytest3.o: mytest3.cpp shaders.h geometry3.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c mytest3.cpp 
shaders.o: shaders.cpp
	$(CC) $(CFLAGS) $(INCFLAGS) -c shaders.cpp 
clean: 
	$(RM) *.o mytest3
 
