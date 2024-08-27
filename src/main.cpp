#include "../lib/imgui/imgui.h"
#include "../lib/imgui/backends/imgui_impl_glfw.h"
#include "../lib/imgui/backends/imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "shader_m.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <map>

// Forward declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void RenderGui(void);
GLuint loadTexture(const std::string& textureName);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Texture bank to store loaded textures
std::map<std::string, GLuint> textureBank;

int main() {
  // Initialize and configure GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  // GLFW window creation
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Texture Viewer", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Load all OpenGL function pointers with GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // Configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // Build and compile shaders
  Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
  
  // Set vertex data/buffers and configure vertex attributes
  float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
  };  
   
  unsigned int VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // Bind vertex array object first, then bind and set vertex buffer
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); 

  // Texture coordinate attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  unsigned int texture1;
  
  // Load and create texture 1
  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  // Set the texture wrapping params
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // Load image, create texture, and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char *data = stbi_load("assets/oak_planks.png", &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data); 

  // Tell opengl which texture unit belongs to what sampler (only has to be done once)
  ourShader.use();
  ourShader.setInt("texture1", 0);

  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  // Render loop
  while(!glfwWindowShouldClose(window)) {
    // Input
    processInput(window);

    // Rendering commands
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1); 
 
    // Activate shader
    ourShader.use();

    // Create transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Retrieve matrix uniform locations and pass to shaders
    unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    ourShader.setMat4("projection", projection);

    // Render cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Render GUI
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    
    RenderGui();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
 
    // Swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();

  // Free all resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();
  return 0;
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void RenderGui(void) {
  // Check context and verify ABI compatibility between caller code and compiled ver of ImGui
  IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context. Refer to examples app!");
  IMGUI_CHECKVERSION();

  // Window flags
  static bool unlock_window = false;
  static bool enable_darkmode = false;
  static bool disable_background = false;

  ImGuiWindowFlags window_flags = 0;

  if (unlock_window)        window_flags |= ImGuiWindowFlags_NoMove;
  if (disable_background) window_flags |= ImGuiWindowFlags_NoBackground;
  //if (enable_darkmode)    window_flags |= ImGuiWindowFlags_EnableDarkmode;

  // Main body of window starts here
  if (!ImGui::Begin("Texview Menu", nullptr, window_flags | ImGuiWindowFlags_AlwaysAutoResize)) {
    // Early out if window is collapsed, as an optimization
    ImGui::End();
    return;
  }

  if (ImGui::CollapsingHeader("Window Options")) {
    //ImGuiIO& io = ImGui::GetIO();
    
    if (ImGui::BeginTable("split", 2)) {
      ImGui::TableNextColumn(); ImGui::Checkbox("Unlock window", &unlock_window);
      ImGui::TableNextColumn(); ImGui::Checkbox("Enable darkmode", &enable_darkmode);
      ImGui::TableNextColumn(); ImGui::Checkbox("Disable background", &disable_background);
      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Asset Browser")) {
    const char* items[] = {"Oak Planks", "Dirt", "Stone", "Glass"};
    static int item_current = 1;
    ImGui::ListBox("Textures", &item_current, items, IM_ARRAYSIZE(items), 4);
  }

  ImGui::CollapsingHeader("Lighting Options");

  ImGui::End();
}

GLuint loadTexture(const std::string& textureName) {
  // Check if texture already loaded
  if (textureBank.find(textureName) != textureBank.end()) {
    return textureBank[textureName];
  }

  // Load texture data from file
  std::string texturePath = "assets/" + textureName;
  int width, height, nrChannels;
  unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
  if (!data) {
    std::cerr << "Failed to load texture: " << textureName << std::endl;
    return 0;
  }

  // Generate OpenGL texture ID
  GLuint textureID;
  glGenTexture(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  // Set texture parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Determine the format (RGB or RGBA) based on the number of channels
  GLenum format = GL_RGB;
  if (nrChannels == 4)
      format = GL_RGBA;

  // Upload the texture data to the GPU
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Free the image memory
  stbi_image_free(data);

  // Store the texture ID in the bank
  textureBank[textureName] = textureID;

  return textureID;
}
