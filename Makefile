# Compiler and flags
CXX := g++
CXXFLAGS := -Iimgui -Iimgui/backends -Wall -O2

# Linker flags
LDFLAGS := -lglfw -lGL

# ImGui source files
IMGUI_SRC := lib/imgui/imgui.cpp \
             lib/imgui/imgui_draw.cpp \
						 lib/imgui/imgui_tables.cpp \
             lib/imgui/imgui_widgets.cpp \
						 lib/imgui/imgui_demo.cpp	\
             lib/imgui/backends/imgui_impl_glfw.cpp \
             lib/imgui/backends/imgui_impl_opengl3.cpp

# Your project's source files
SRC := src/main.cpp src/glad.c $(IMGUI_SRC)

# Output binary
TARGET := texview

# Default target
all: $(TARGET)

# Link the program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean target to remove compiled files
clean:
	rm -f $(TARGET) imgui.ini

# Phony targets
.PHONY: all clean

