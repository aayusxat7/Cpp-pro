CXX = g++
CXXFLAGS = -I. -I./src -I./vendor/imgui -I./vendor/imgui/backends -g -Wall -Wformat
LIBS = -lGL -ldl -lglfw -lpthread -lX11

SOURCES = main.cpp \
          src/App.cpp \
          vendor/imgui/imgui.cpp \
          vendor/imgui/imgui_demo.cpp \
          vendor/imgui/imgui_draw.cpp \
          vendor/imgui/imgui_tables.cpp \
          vendor/imgui/imgui_widgets.cpp \
          vendor/imgui/backends/imgui_impl_glfw.cpp \
          vendor/imgui/backends/imgui_impl_opengl3.cpp

OBJS = $(SOURCES:.cpp=.o)
TARGET = EduSavant

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
