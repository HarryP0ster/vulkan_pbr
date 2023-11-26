#pragma once
#include "vulkan_objects/queue.hpp"
#include "vulkan_api.hpp"

class RenderScope
{
public:
	RenderScope() {};
	~RenderScope() { Destroy(); };

	RenderScope& CreatePhysicalDevice(const VkInstance& instance, const std::vector<const char*>& device_extensions);

	RenderScope& CreateLogicalDevice(const VkPhysicalDeviceFeatures& features, const std::vector<const char*>& device_extensions, const std::vector<VkQueueFlagBits>& queues);

	RenderScope& CreateMemoryAllocator(const VkInstance& instance);

	RenderScope& CreateSwapchain(const VkSurfaceKHR& surface);

	RenderScope& CreateDefaultRenderPass();

	RenderScope& CreateDescriptorPool(uint32_t setsCount, const std::vector<VkDescriptorPoolSize>& poolSizes);

	void RecreateSwapchain(const VkSurfaceKHR& surface);

	void Destroy();

	VkBool32 IsReadyToUse() const;

	inline const VkDevice& GetDevice() const { return logicalDevice; };

	inline const VmaAllocator& GetAllocator() const { return allocator; };

	inline const VkPhysicalDevice& GetPhysicalDevice() const { return physicalDevice; };

	inline const VkRenderPass& GetRenderPass() const { return renderPass; };

	inline const VkSwapchainKHR& GetSwapchain() const { return swapchain; };

	inline const VkExtent2D& GetSwapchainExtent() const { return swapchainExtent; };

	inline const VkDescriptorPool& GetDescriptorPool() const { return descriptorPool; };

	inline const VkFormat& GetColorFormat() const { return swapchainFormat; };

	inline const VkFormat& GetDepthFormat() const { return depthFormat; };

	inline const uint32_t& GetMaxFramesInFlight() const { return framesInFlight; };

	inline const Queue& GetQueue(VkQueueFlagBits Type) const {
		assert(available_queues.contains(Type));
		return available_queues.at(Type);
	}

private:
	std::unordered_map<VkQueueFlagBits, Queue> available_queues;
	uint32_t framesInFlight = 1u;

	VkDevice logicalDevice = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	const VkFormat depthFormat = VK_FORMAT_D16_UNORM;
	VkFormat swapchainFormat = VK_FORMAT_R8G8B8A8_SRGB;
	VkExtent2D swapchainExtent = { 0, 0 };
};