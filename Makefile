.PHONY: all clean

all: engine.exe

clean:
	rm bin\*.exe
	rm obj\*.o
	rm obj\Physics\*.o

PARAMS = -m64 -fexceptions -Wall -Wno-unused-function -std=c++11
INCLUDE = -Iinclude\glew -Iinclude\stbi -Iinclude\glm -Iinclude\glew -Iinclude\glfw

main.o: src\Main.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Main.cpp -o obj\Main.o

opengl.o: src\OpenGL.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\OpenGL.cpp -o obj\OpenGL.o

windows.o: src\Windows.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Windows.cpp -o obj\Windows.o


algorithms.o: src\Physics\Algorithms.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Physics\Algorithms.cpp -o obj\Physics\Algorithms.o

body.o: src\Physics\Body.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Physics\Body.cpp -o obj\Physics\Body.o

collider.o: src\Physics\Collider.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Physics\Collider.cpp -o obj\Physics\Collider.o

collision.o: src\Physics\Collision.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Physics\Collision.cpp -o obj\Physics\Collision.o

shape.o: src\Physics\Shape.cpp
	g++ $(PARAMS) $(INCLUDE) -c src\Physics\Shape.cpp -o obj\Physics\Shape.o

Physics:
	mkdir obj\Physics


LIBS = -Llibs\glew -Llibs\glfw
OBJS = obj\Main.o obj\OpenGL.o obj\Physics\Algorithms.o obj\Physics\Body.o obj\Physics\Collider.o obj\Physics\Collision.o obj\Physics\Shape.o obj\Windows.o
# -mwindows -O2 -s
LINKING = -m64 -std=c++11 -lglew32s -lopengl32 -lglfw3 -lgdi32

engine.exe: main.o opengl.o windows.o Physics algorithms.o body.o collider.o collision.o shape.o
	g++ $(LIBS) -o bin\Engine.exe $(OBJS) $(LINKING)
	echo "All done!"