TARGET = main
SRCS = main.cpp func.cpp
OPS = -L/usr/include -lglut -lGLU -lGL -lm
CP=g++
$(TARGET):$(SRCS)
	$(CP) $(SRCS) $(OPS) -o $(TARGET)
