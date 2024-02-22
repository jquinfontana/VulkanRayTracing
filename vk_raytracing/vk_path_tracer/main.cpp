#include <array>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include "imgui/imgui_helper.h"

#include "hello_vulkan.h"
#include "imgui/imgui_camera_widget.h"
#include "nvh/cameramanipulator.hpp"
#include "nvh/fileoperations.hpp"
#include "nvpsystem.hpp"
#include "nvvk/commands_vk.hpp"
#include "nvvk/context_vk.hpp"


//////////////////////////////////////////////////////////////////////////
#define UNUSED(x) (void)(x)
//////////////////////////////////////////////////////////////////////////

// Default search path for shaders
std::vector<std::string> defaultSearchPaths;


// GLFW Callback functions
static void onErrorCallback(int error, const char* description)
{
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Extra UI
void renderUI(HelloVulkan& helloVk)
{
  if(ImGui::CollapsingHeader("Extra widget"))
  {
    ImGuiH::CameraWidget();
    ImGui::RadioButton("Point", &helloVk.m_pcRaster.lightType, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Infinite", &helloVk.m_pcRaster.lightType, 1);

    ImGui::SliderFloat3("Position", &helloVk.m_pcRaster.lightPosition.x, -20.f, 20.f);
    ImGui::SliderFloat("Intensity", &helloVk.m_pcRaster.lightIntensity, 0.f, 150.f);
  }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static int const SAMPLE_WIDTH  = 1280;
static int const SAMPLE_HEIGHT = 720;


//--------------------------------------------------------------------------------------------------
// Application Entry
//
int main(int argc, char** argv)
{
  UNUSED(argc);

  // Setup GLFW window
  glfwSetErrorCallback(onErrorCallback);
  if(!glfwInit())
  {
    return 1;
  }
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(SAMPLE_WIDTH, SAMPLE_HEIGHT, PROJECT_NAME, nullptr, nullptr);


  // Setup camera
  CameraManip.setWindowSize(SAMPLE_WIDTH, SAMPLE_HEIGHT);
  CameraManip.setLookat(glm::vec3(5, 4, -4), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

  // Setup Vulkan
  if(!glfwVulkanSupported())
  {
    printf("GLFW: Vulkan Not Supported\n");
    return 1;
  }

  // setup some basic things for the sample, logging file for example
  NVPSystem system(PROJECT_NAME);

  // Search path for shaders and other media
  defaultSearchPaths = {
      NVPSystem::exePath() + PROJECT_RELDIRECTORY,
      NVPSystem::exePath() + PROJECT_RELDIRECTORY "..",
      std::string(PROJECT_NAME),
  };

  // Vulkan required extensions
  assert(glfwVulkanSupported() == 1);
  uint32_t count{0};
  auto     reqExtensions = glfwGetRequiredInstanceExtensions(&count);

  // Requesting Vulkan extensions and layers
  nvvk::ContextCreateInfo contextInfo;
  contextInfo.setVersion(1, 2);                       // Using Vulkan 1.2
  for(uint32_t ext_id = 0; ext_id < count; ext_id++)  // Adding required extensions (surface, win32, linux, ..)
    contextInfo.addInstanceExtension(reqExtensions[ext_id]);
  contextInfo.addInstanceLayer("VK_LAYER_LUNARG_monitor", true);              // FPS in titlebar
  contextInfo.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, true);  // Allow debug names
  contextInfo.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);            // Enabling ability to present rendering

  // #VKRay: Activate the ray tracing extension
  VkPhysicalDeviceAccelerationStructureFeaturesKHR accelFeature{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR};
  contextInfo.addDeviceExtension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, false, &accelFeature);  // To build acceleration structures
  VkPhysicalDeviceRayTracingPipelineFeaturesKHR rtPipelineFeature{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR};
  contextInfo.addDeviceExtension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, false, &rtPipelineFeature);  // To use vkCmdTraceRaysKHR
  contextInfo.addDeviceExtension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);  // Required by ray tracing pipeline

  // Creating Vulkan base application
  nvvk::Context vkctx{};
  vkctx.initInstance(contextInfo);
  // Find all compatible devices
  auto compatibleDevices = vkctx.getCompatibleDevices(contextInfo);
  assert(!compatibleDevices.empty());
  // Use a compatible device
  vkctx.initDevice(compatibleDevices[0], contextInfo);

  // Create example
  HelloVulkan helloVk;

  // Window need to be opened to get the surface on which to draw
  const VkSurfaceKHR surface = helloVk.getVkSurface(vkctx.m_instance, window);
  vkctx.setGCTQueueWithPresent(surface);

  helloVk.setup(vkctx.m_instance, vkctx.m_device, vkctx.m_physicalDevice, vkctx.m_queueGCT.familyIndex);
  helloVk.createSwapchain(surface, SAMPLE_WIDTH, SAMPLE_HEIGHT);
  helloVk.createDepthBuffer();
  helloVk.createRenderPass();
  helloVk.createFrameBuffers();

  // Setup Imgui
  helloVk.initGUI(0);  // Using sub-pass 0

  // Creation of the example-----------------------------------------------------------------------------------------------------------------
  
  //CornellBox Spheres
  //helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Sphere.obj", defaultSearchPaths, true));

  {  //Minecraft floor
      /*helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Sphere.obj", defaultSearchPaths, true));
      helloVk.loadModel(nvh::findFile("media/scenes/vokselia_spawn.obj", defaultSearchPaths, true),
                       glm::scale(glm::translate(glm::mat4(1.0f), vec3(0, 0.1, 0.1)), vec3(0.5)));*/
  }

  {  //cornell dragon
      /*helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Empty-CO.obj", defaultSearchPaths, true));
      helloVk.loadModel(nvh::findFile("media/scenes/dragon.obj", defaultSearchPaths, true),
                        glm::translate(
                        glm::rotate(
                        glm::scale(glm::mat4(1.0f), vec3(1.5,1.5,1.5)), (float)1.5, vec3(0, 1, 0)),vec3(0, 0.5, 0)));*/
  }

  helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Water.obj", defaultSearchPaths, true));

  //Lego
  {
      //helloVk.loadModel(nvh::findFile("media/scenes/lego.obj", defaultSearchPaths, true));
  }

  { //cornell bunny
      /*helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Mirror.obj", defaultSearchPaths, true));
      helloVk.loadModel(nvh::findFile("media/scenes/bunny.obj", defaultSearchPaths, true));*/
  }

  {  //cornell lucy
      /*helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Empty-CO.obj", defaultSearchPaths, true));
    helloVk.loadModel(nvh::findFile("media/scenes/lucy.obj", defaultSearchPaths, true),
                      glm::scale(glm::rotate(glm::mat4(1.0f), (float)1.5, vec3(0, 1, 0)), vec3(0.0023)));*/
  }

  //Sponza
  //helloVk.loadModel(nvh::findFile("media/scenes/sponza.obj", defaultSearchPaths, true));

  {  //cornell bubble
     /*helloVk.loadModel(nvh::findFile("media/scenes/CornellBox-Empty-CO.obj", defaultSearchPaths, true));

     helloVk.loadModel(nvh::findFile("media/scenes/sphere.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.5, 1.2, -0.5)), vec3(0.4)));
     helloVk.loadModel(nvh::findFile("media/scenes/SphereInv.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.5, 1.2, -0.5)), vec3(0.39)));

     helloVk.loadModel(nvh::findFile("media/scenes/sphere.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(-0.5, 1.2, 0)), vec3(0.2)));
     helloVk.loadModel(nvh::findFile("media/scenes/SphereInv.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(-0.5, 1.2, 0)), vec3(0.19)));

     helloVk.loadModel(nvh::findFile("media/scenes/sphere.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(-0.45, 0.8, -0.1)), vec3(0.1)));
     helloVk.loadModel(nvh::findFile("media/scenes/SphereInv.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(-0.45, 0.8, -0.1)), vec3(0.095)));

     helloVk.loadModel(nvh::findFile("media/scenes/sphere.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.3, 0.4, 0.2)), vec3(0.3)));
     helloVk.loadModel(nvh::findFile("media/scenes/SphereInv.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.3, 0.4, 0.2)), vec3(0.29)));

     helloVk.loadModel(nvh::findFile("media/scenes/sphere.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.1, 0.55, 0.4)), vec3(0.2)));
     helloVk.loadModel(nvh::findFile("media/scenes/SphereInv.obj", defaultSearchPaths, true),
         glm::scale(glm::translate(glm::mat4(1.0f), vec3(0.1, 0.55, 0.4)), vec3(0.19)));*/
  }
  
  //-----------------------------------------------------------------------------------------------------------------------------------------

  helloVk.createOffscreenRender();
  helloVk.createDescriptorSetLayout();
  helloVk.createGraphicsPipeline();
  helloVk.createUniformBuffer();
  helloVk.createObjDescriptionBuffer();
  helloVk.updateDescriptorSet();

  // #VKRay
  helloVk.initRayTracing();
  helloVk.createBottomLevelAS();
  helloVk.createTopLevelAS();
  helloVk.createRtDescriptorSet();
  helloVk.createRtPipeline();
  helloVk.createRtShaderBindingTable();

  helloVk.createPostDescriptor();
  helloVk.createPostPipeline();
  helloVk.updatePostDescriptorSet();


  glm::vec4 clearColor   = glm::vec4(1, 1, 1, 1.00f);
  bool      useRaytracer = true;

  helloVk.m_pcRay.camAperture = 0.f;
  helloVk.m_pcRay.focusDist = 3.f;
  helloVk.m_pcRay.shininess = 0.f;
  helloVk.m_pcRay.fuzziness = 0.f;


  helloVk.setupGlfwCallbacks(window);
  ImGui_ImplGlfw_InitForVulkan(window, true);

 


  // Main loop
  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    if(helloVk.isMinimized())
      continue;

    // Start the Dear ImGui frame
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // Show UI window.
    if(helloVk.showGui()) {
      ImGuiH::Panel::Begin();
      renderUI(helloVk);

      ImGui::Checkbox("Ray Tracer mode", &useRaytracer);  // Switch between raster and ray tracing
      ImGui::Checkbox("Ambient Ligth", &helloVk.m_pcRay.ambientLigth); //enable ambient ligth
      ImGui::SliderFloat("Aperture", &helloVk.m_pcRay.camAperture, 0.001f, 0.5f); //camera parameters
      ImGui::SliderFloat("Focus distance", &helloVk.m_pcRay.focusDist, 0.1f, 20.f);
      ImGui::SliderFloat("Shininess", &helloVk.m_pcRay.shininess, 0.f, 700.f);
      ImGui::SliderFloat("Fuzziness", &helloVk.m_pcRay.fuzziness, 0.f, 1.f);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGuiH::Panel::End();
    }

    // Start rendering the scene
    helloVk.prepareFrame();

    // Start command buffer of this frame
    auto                   curFrame = helloVk.getCurFrame();
    const VkCommandBuffer& cmdBuf   = helloVk.getCommandBuffers()[curFrame];

    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    // Updating camera buffer
    helloVk.updateUniformBuffer(cmdBuf);

    // Clearing screen
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color        = {{clearColor[0], clearColor[1], clearColor[2], clearColor[3]}};
    clearValues[1].depthStencil = {1.0f, 0};

    // Offscreen render pass
    {
      VkRenderPassBeginInfo offscreenRenderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
      offscreenRenderPassBeginInfo.clearValueCount = 2;
      offscreenRenderPassBeginInfo.pClearValues    = clearValues.data();
      offscreenRenderPassBeginInfo.renderPass      = helloVk.m_offscreenRenderPass;
      offscreenRenderPassBeginInfo.framebuffer     = helloVk.m_offscreenFramebuffer;
      offscreenRenderPassBeginInfo.renderArea      = {{0, 0}, helloVk.getSize()};

      // Rendering Scene
      if(useRaytracer)
      {
        helloVk.raytrace(cmdBuf, clearColor);
      }
      else
      {
        vkCmdBeginRenderPass(cmdBuf, &offscreenRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        helloVk.rasterize(cmdBuf);
        vkCmdEndRenderPass(cmdBuf);
      }
    }

    // 2nd rendering pass: tone mapper, UI
    {
      VkRenderPassBeginInfo postRenderPassBeginInfo{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
      postRenderPassBeginInfo.clearValueCount = 2;
      postRenderPassBeginInfo.pClearValues    = clearValues.data();
      postRenderPassBeginInfo.renderPass      = helloVk.getRenderPass();
      postRenderPassBeginInfo.framebuffer     = helloVk.getFramebuffers()[curFrame];
      postRenderPassBeginInfo.renderArea      = {{0, 0}, helloVk.getSize()};

      // Rendering tonemapper
      vkCmdBeginRenderPass(cmdBuf, &postRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
      helloVk.drawPost(cmdBuf);
      // Rendering UI
      ImGui::Render();
      ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);
      vkCmdEndRenderPass(cmdBuf);
    }

    // Submit for display
    vkEndCommandBuffer(cmdBuf);
    helloVk.submitFrame();
  }

  // Cleanup
  vkDeviceWaitIdle(helloVk.getDevice());

  helloVk.destroyResources();
  helloVk.destroy();
  vkctx.deinit();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
