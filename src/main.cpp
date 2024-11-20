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
unsigned int loadCubemap(vector<std::string> faces);

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

  Shader voxelShader("shaders/voxel.vs", "shaders/voxel.fs");
  Shader screenShader("shaders/aa_post.vs", "shaders/aa_post.fs");
  
  // set vertex data/buffers and configure vertex attributes
  float voxelVertices[] = {
    // positions       
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
  };

  float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
  };

  // set up voxel VAO
  unsigned int voxelVBO, voxelVAO;
  glGenVertexArrays(1, &voxelVAO);
  glGenBuffers(1, &voxelVBO);

  glBindVertexArray(voxelVAO);
  glBindBuffer(GL_ARRAY_BUFFER, voxelVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(voxelVertices), &voxelVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

  // set screen VAO
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(, &quadVBO);

  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // configure MSAA framebuffer
  unsigned int framebuffer;
  glGenFramebuffers(1, &framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

  // create multisampled color attachement texture
  unsigned int textureColorBufferMultiSampled;
  glGenTexture(1, &textureColorBufferMultiSampled);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

  // create multisampled renderbuffer object for depth and stencil attachments
  unsigned int rbo;
  glGenRenderbuffer(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "ERROR:FRAMEBUFFER:: Framebuffer is not complete!" << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // configure second post-processing framebuffer
  unsigned int intermediateFBO;
  glGenFramebuffers(1, &intermediateFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

  // create color attachment texture
  unsigned int screenTexture;
  glGenTextures(1, &screenTexture);
  glBindTexture(GL_TEXTURE_2D, screenTexture);
  glBindTexture(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SC_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0); 

  // Load default texture on start
  unsigned int texture_default = loadTexture("oak_planks.png");
  glBindTexture(GL_TEXTURE_2D, texture_default);

  // shader configuration
  screenShader.use();
  screenShader.setInt("screenTexture", 0);

  // Create imgui context
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.IniFilename = NULL;
  
  ImGui::StyleColorsLight();
  
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");

  render(window, voxelShader, , voxelVAO, lightsourceVAO);

  // Destroy imgui context
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();

  // Free all resources
  glDeleteVertexArrays(1, &voxelVAO);
  glDeleteVertexArrays(1, &lightsourceVAO);
  glDeleteBuffers(1, &voxelVBO);

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
  while(!glfwWindowShouldClose(window)) {
    processInput(window);

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


