#pragma once
#include <glfw/glfw3.h>
#include <vma/vk_mem_alloc.h>
#include <glm/glm.hpp>
#include <vector>
#include <array>
/*
* !@brief Generate given number of mip levels for given VkImage
* 
* @param[in] device - device the image belongs to
* @param[in] cmd - command buffer to populate with commands. YOU HAVE TO ALLOCATE, SUBMIT AND FREE PROVIDED COMMAND BUFFER YOURSELF!
* @param[in] image - target image. IMAGE SHOULD BE TRANSITIONED TO TRANSFER_SRC LAYOUT BEFOREHAND!
* @param[in] dimensions - image size
* @param[in] subRange - specify number of mipLevels and arrayLayers here
*/
VkBool32 GenerateMipMaps(const VkDevice& device, const VkCommandBuffer& cmd, const VkImage& image, const VkExtent2D& dimensions, const VkImageSubresourceRange& subRange);
/*
* !@brief Creates synchronization fence
*
* @param[in] device - logical device to create object on
* @param[out] outFence - pointer to store fence at
*
* @return VK_TRUE if creation was successful, VK_FALSE otherwise
*/
VkBool32 CreateFence(const VkDevice& device, VkFence* outFence);
/*
* !@brief Creates synchronization semaphore
*
* @param[in] device - logical device to create object on
* @param[out] outSemaphore - pointer to store semaphore at
*
* @return VK_TRUE if creation was successful, VK_FALSE otherwise
*/
VkBool32 CreateSemaphore(const VkDevice& device, VkSemaphore* outSemaphore);
/*
* !@brief Allocates specified number of command buffers from the pool
*
* @param[in] device - logical device to allocate on
* @param[in] pool - command pool to allocate from
* @param[in] count - number of buffers to allocate
* @param[out] outBuffers - pointer to the VkCommandBuffer object/array
*
* @return VK_TRUE if allocation was successful, VK_FALSE otherwise
*/
VkBool32 AllocateBuffers(const VkDevice& device, const VkCommandPool& pool, const uint32_t count, VkCommandBuffer* outBuffers);
/*
* !@brief Initialize descriptor pool object
*
* @param[in] device - logical device to create object on
* @param[in] poolSizes - pointer to an array of VkDescriptorPoolSize structs
* @param[in] poolSizesCount - size of the given array
* @param[in] setsCount - maximum number of descriptor sets, that the pool can allocate
* @param[out] outPool - pointer to store resulting object at
*
* @return VK_TRUE if creation was successful, VK_FALSE otherwise
*/
VkBool32 CreateDescriptorPool(const VkDevice& device, const VkDescriptorPoolSize* poolSizes, const size_t poolSizesCount, const uint32_t setsCount, VkDescriptorPool* outPool);
/*
* !@brief Finds suitable queue indices for specified queue flag bits
*
* @param[in] physicalDevice - physical device to find queues on
* @param[in] flags - collection of queue flags to find
*
* @return collection of family indices ordered respectively to the flags collection
*/
std::vector<uint32_t> FindDeviceQueues(const VkPhysicalDevice& physicalDevice, const std::vector<VkQueueFlagBits>& flags);
/*
* !@brief Checks if GPU supports specified extensions
*
* @param[in] physicalDevuce - GPU device to test
* @param[in] desired_extensions - collection of extensions to find
*
* @return VK_TRUE if GPU supports specified extensions, VK_FALSE otherwise
*/
VkBool32 EnumerateDeviceExtensions(const VkPhysicalDevice& physicalDevice, const std::vector<const char*>& desired_extensions);
/*
* !@brief Initializes VkSwapchainKHR object based on the surface capabilities
*
* @param[in] physicalDevuce - GPU device
* @param[in] device - logical device to create object on
* @param[in] surface - target surface
* @param[in/out] extent - framebuffer size, may be changed to account for device capabilities
* @param[out] outSwapchain - pointer to store resulting VkSwapchain at
* 
* @return VK_TRUE if initialization was successful, VK_FALSE if window size is zero or initialization failed
*/
VkBool32 CreateSwapchain(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkSurfaceFormatKHR& desired_format, VkExtent2D& extent, VkSwapchainKHR* outSwapchain);
/*
* !@brief Initializes VkSwapchainKHR object based on the surface capabilities
*
* @param[in] physicalDevuce - GPU device
* @param[in] surface - target surface
*
* @return Supported and/or desired surface format for swapchain imaging
*/
VkSurfaceFormatKHR GetSurfaceFormat(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkSurfaceFormatKHR& desired_format);
/*
* !@brief Creates command pool for specified queue family
*
* @param[in] device - logical device to create object on
* @param[in] targetQueueIndex - index of the queue
* @param[out] outPool - where to store command pool
*
* @return VK_TRUE if creation was successful, VK_FALSE otherwise
*/
VkBool32 CreateCommandPool(const VkDevice& device, const uint32_t targetQueueIndex, VkCommandPool* outPool);

VkBool32 CreateFramebuffer(const VkDevice& device, const VkRenderPass& renderPass, const VkExtent2D extents, const std::vector<VkImageView>& attachments, VkFramebuffer* outFramebuffer);
/*
* !@brief Initialize Vulkan Memmory Allocator to handle allocations
*
* @return VK_TRUE if creation is successful, VK_FALSE otherwise
*/
VkBool32 CreateAllocator(const VkInstance& instance, const VkPhysicalDevice& physicalDevice, const VkDevice& device, VmaAllocator* outAllocator);
/*
* !@brief Find suitable physical device
* compatable with required extensions
*
* @return VK_TRUE if suitable device was found, VK_FALSE otherwise
*/
VkBool32 EnumeratePhysicalDevices(const VkInstance& instance, const std::vector<const char*>& device_extensions, VkPhysicalDevice* outPhysicalDevice);
/*
* !@brief Initilizes logical device based on suitable physical device
*
* @return VK_TRUE if device was created successfuly, VK_FALSE otherwise
*/
VkBool32 CreateLogicalDevice(const VkPhysicalDevice& physicalDevice, const VkPhysicalDeviceFeatures& device_features, const std::vector<const char*>& device_extensions, const std::vector<uint32_t>& queues, VkDevice* outDevice);

VkBool32 CreateRenderPass(const VkDevice& device, const VkRenderPassCreateInfo& info, VkRenderPass* outRenderPass);