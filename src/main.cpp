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
#include <vector>
#include <algorithm>

// Forward declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

void render(GLFWwindow *window, Shader voxelShader, Shader lightsourceShader, unsigned int voxelVAO, unsigned int lightsourceVAO);
void helpMarker(const char* desc);
unsigned int loadTexture(const std::string& textureName);

struct Texture {
  std::string name;
  unsigned int id;
};

// Vector to store texture data sequentially
std::vector<Texture> textures;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Custom path for imugi ini to be stored
const char* custom_ini_path = "bin/imgui.ini";

// Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  
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

  Shader voxelShader("shaders/voxelshader.vs", "shaders/voxelshader.fs");
  Shader lightsourceShader("shaders/lightsource.vs", "shaders/lightsource.fs");
  
  // Set vertex data/buffers and configure vertex attributes
  float vertices[] = {
     // positions         // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
  }; 

  unsigned int VBO, voxelVAO;
  glGenVertexArrays(1, &voxelVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(voxelVAO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0); 
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  unsigned int lightsourceVAO;
  glGenVertexArrays(1, &lightsourceVAO);
  glBindVertexArray(lightsourceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // Load default texture on start
  unsigned int texture_default = loadTexture("oak_planks.png");
  glBindTexture(GL_TEXTURE_2D, texture_default);

  // Tell opengl which texture unit belongs to what sampler (only has to be done once)
  voxelShader.use();
  voxelShader.setInt("material.diffuse", 0);

  // Create imgui context
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = NULL;
  
  ImGui::StyleColorsLight();
  
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  render(window, voxelShader, lightsourceShader, voxelVAO, lightsourceVAO);

  // Destroy imgui context
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();

  // Free all resources
  glDeleteVertexArrays(1, &voxelVAO);
  glDeleteVertexArrays(1, &lightsourceVAO);
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

void helpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void render(GLFWwindow *window, Shader voxelShader, Shader lightsourceShader, unsigned int voxelVAO, unsigned int lightsourceVAO) {
  // Render loop
  while(!glfwWindowShouldClose(window)) {
    // Input
    processInput(window);

    // Rendering commands
    glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    // Activate voxel shader
    voxelShader.use();
    voxelShader.setVec3("light.position", lightPos);

    // Light properties
    static float lightAmbReflect = 0.5f;
    static float lightDiffReflect = 0.7f;
    static float lightSpecReflect = 1.0f;
    voxelShader.setVec3("light.ambient", lightAmbReflect, lightAmbReflect, lightAmbReflect); 
    voxelShader.setVec3("light.diffuse", lightDiffReflect, lightDiffReflect, lightDiffReflect);
    voxelShader.setVec3("light.specular", lightSpecReflect, lightSpecReflect, lightSpecReflect);

    // material properties 
    static float matSpecReflect = 0.3f;
    static float matSpecExp = 2.0f;
    voxelShader.setVec3("material.specular", matSpecReflect, matSpecReflect, matSpecReflect);
    voxelShader.setFloat("material.shininess", matSpecExp); // [2, 256]

    // Create transformations
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    // Retrieve matrix uniform locations and pass to shaders
    unsigned int modelLoc = glGetUniformLocation(voxelShader.ID, "model");
    unsigned int viewLoc = glGetUniformLocation(voxelShader.ID, "view");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    voxelShader.setMat4("projection", projection);
    voxelShader.setMat4("view", view);
    voxelShader.setMat4("model", model);

    // Render voxel
    glBindVertexArray(voxelVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw light source object
    lightsourceShader.use();
    lightsourceShader.setMat4("projection", projection);
    lightsourceShader.setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lightsourceShader.setMat4("model", model);

    glBindVertexArray(lightsourceVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Init ImGui window
    ImGui::LoadIniSettingsFromDisk(custom_ini_path);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame(); 

    IM_ASSERT(ImGui::GetCurrentContext() != NULL && "Missing Dear ImGui context");
    IMGUI_CHECKVERSION();
   
    // Default window flags
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize; 
    
    static bool unlock_window = false;
    static bool send_to_back = false;

    if (!unlock_window) {
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      window_flags |= ImGuiWindowFlags_NoMove;
    }

    /*
    if (send_to_back) {
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
      ImGui::SetNextWindowFocus();
    }
    */

    if (!ImGui::Begin("Texview Menu", nullptr, window_flags)) {
      // Early out if window is collapsed
      ImGui::End();
      return;
    }

    if (ImGui::CollapsingHeader("Window Options")) {
      // TODO: Fix send to background option
      if (ImGui::BeginTable("split", 2)) {
        ImGui::TableNextColumn(); ImGui::Checkbox("Unlock window", &unlock_window);
        ImGui::TableNextColumn(); ImGui::Checkbox("Send to background", &send_to_back);
        ImGui::EndTable();
      }
    }

    std::map<std::string, std::string> items = {
      {"Oak Planks", "oak_planks.png"}, 
      {"Acacia Planks", "acacia_planks.png"}, 
      {"Dark Oak Planks", "dark_oak_planks.png"},
      {"Jungle Planks", "jungle_planks.png"},
      {"Bamboo Planks", "bamboo_planks.png"}, 
      {"Mangrove Planks", "mangrove_planks.png"}, 
      {"Spruce Planks", "spruce_planks.png"}, 
      {"Birch Planks", "birch_planks.png"}
    }; 
    
    std::vector<const char*> keys;
    for (const auto& pair : items) {
      keys.push_back(pair.first.c_str());
    }

    static int item_selected = -1;

    if (ImGui::CollapsingHeader("Asset Browser")) { 
      if (ImGui::ListBox("Textures", &item_selected, keys.data(), keys.size())) {
        std::string key_selected = keys[item_selected];
        unsigned int textureID = loadTexture(items[key_selected]);
        glBindTexture(GL_TEXTURE_2D, textureID);
      }
      ImGui::SameLine(); helpMarker("Use the listbox to control which texture is bound.");
    }

    if (ImGui::CollapsingHeader("Lighting Options")) {
      ImGui::SeparatorText("Light Properties");
      ImGui::SliderFloat("Ambient reflection", &lightAmbReflect, 0.0f, 1.0f, "%.3f");
      ImGui::SliderFloat("Diffuse reflection", &lightDiffReflect, 0.0f, 1.0f, "%.3f");
      ImGui::SliderFloat("Specular reflection##A", &lightSpecReflect, 0.0f, 1.0f, "%.3f");

      ImGui::SeparatorText("Material Properties");
      ImGui::SliderFloat("Specular reflection##B", &matSpecReflect, 0.0f, 1.0f, "%.3f");
      ImGui::SliderFloat("Specular exponent (shininess)", &matSpecExp, 2, 256);  // these bounds for shininess are semiarbitrary
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGui::SaveIniSettingsToDisk(custom_ini_path);

    // Swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

unsigned int loadTexture(const std::string& textureName) {
  // Check if texture has already been loaded
  auto it = std::find_if(textures.begin(), textures.end(), [textureName](const Texture& texture) {
    return texture.name == textureName;
  });
  if (it != textures.end()) {
    //std::cout << "Found texture: " << it->name << ", ID: " << it->id << "\n";
    return it->id;
  } else {
    // std::cout << "Loading new texture with name " << textureName << "\n";
  }

  unsigned int textureID;
  glGenTextures(1, &textureID);

  // Load texture data and generate mipmaps
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  std::string texturePath = "assets/textures/" + textureName;
  unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format = GL_NONE;
    switch (nrChannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  } else {
    std::cout << "Failed to load texture: " << textureName << std::endl;
  } 
 
  stbi_image_free(data);
  textures.push_back({textureName, textureID}); // store new texture info

  return textureID;
}


