#include "pch.hpp"
#include "noise.hpp"
#include "vulkan_objects/pipeline.hpp"
#include "vulkan_objects/descriptor_set.hpp"

extern TAuto<VulkanImage> create_image(const RenderScope& Scope, void* pixels, int count, int w, int h, const VkFormat& format, const VkImageCreateFlags& flags);

TAuto<VulkanImage> generate(const char* shader, const RenderScope& Scope, VkFormat format, VkExtent3D imageSize, TVector<uint32_t> constants)
{
	TVector<uint32_t> queueFamilyIndices;
	queueFamilyIndices.push_back(Scope.GetQueue(VK_QUEUE_GRAPHICS_BIT).GetFamilyIndex());
	queueFamilyIndices.push_back(Scope.GetQueue(VK_QUEUE_COMPUTE_BIT).GetFamilyIndex());
	VkImageCreateInfo noiseInfo{};
	noiseInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	noiseInfo.arrayLayers = 1u;
	noiseInfo.extent = imageSize;
	noiseInfo.format = format;
	noiseInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	noiseInfo.mipLevels = 1u;
	noiseInfo.flags = 0u;
	noiseInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	noiseInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	noiseInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	noiseInfo.queueFamilyIndexCount = queueFamilyIndices.size();
	noiseInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	noiseInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	noiseInfo.imageType = imageSize.depth == 1u ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
	VmaAllocationCreateInfo noiseAllocCreateInfo{};
	noiseAllocCreateInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
	VkImageViewCreateInfo noiseImageView{};
	noiseImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	noiseImageView.viewType = imageSize.depth == 1u ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_3D;
	noiseImageView.format = format;
	noiseImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	noiseImageView.subresourceRange.baseArrayLayer = 0u;
	noiseImageView.subresourceRange.baseMipLevel = 0u;
	noiseImageView.subresourceRange.layerCount = 1u;
	noiseImageView.subresourceRange.levelCount = 1u;

	TAuto<VulkanImage> noise = std::make_unique<VulkanImage>(Scope);
	noise->CreateImage(noiseInfo, noiseAllocCreateInfo)
		.CreateImageView(noiseImageView)
		.CreateSampler(ESamplerType::LinearRepeat)
		.TransitionLayout(VK_IMAGE_LAYOUT_GENERAL);

	ComputePipelineDescriptor noise_pipeline{};
	TAuto<DescriptorSet> noise_set = DescriptorSetDescriptor().AddStorageImage(0, VK_SHADER_STAGE_COMPUTE_BIT, *noise)
		.Allocate(Scope);
	noise_pipeline.SetShaderName(shader) 
		.AddDescriptorLayout(noise_set->GetLayout());

	for (size_t i = 0; i < constants.size(); i++) {
		noise_pipeline.AddSpecializationConstant(i, constants[i]);
	}

	TAuto<Pipeline> pipeline = noise_pipeline.Construct(Scope);

	VkCommandBuffer cmd;
	Scope.GetQueue(VK_QUEUE_COMPUTE_BIT)
		.AllocateCommandBuffers(1, &cmd);
	::BeginOneTimeSubmitCmd(cmd);
	pipeline->BindPipeline(cmd);
	noise_set->BindSet(0, cmd, *pipeline);
	vkCmdDispatch(cmd, noise->GetExtent().width, noise->GetExtent().height, noise->GetExtent().depth);
	::EndCommandBuffer(cmd);
	Scope.GetQueue(VK_QUEUE_COMPUTE_BIT)
		.Submit(cmd)
		.Wait()
		.FreeCommandBuffers(1, &cmd);

	noise->GenerateMipMaps();

	return noise;
}

TAuto<VulkanImage> GRNoise::GenerateSolidColor(const RenderScope& Scope, VkExtent2D imageSize, VkFormat Format, std::byte r, std::byte g, std::byte b, std::byte a)
{
	std::byte* pixels = new std::byte[imageSize.width * imageSize.height * 4];

	assert(pixels != nullptr);

	for (uint32_t i = 0u; i < imageSize.width * imageSize.height * 4; i += 4)
	{
		pixels[i] = r;
		pixels[i + 1] = g;
		pixels[i + 2] = b;
		pixels[i + 3] = a;
	}

	TAuto<VulkanImage> target = create_image(Scope, pixels, 1, imageSize.width, imageSize.height, Format, 0);
	
	delete[] pixels;

	return target;
}

TAuto<VulkanImage> GRNoise::GeneratePerlin(const RenderScope& Scope, VkExtent3D imageSize, uint32_t frequency, uint32_t octaves)
{
	return generate("perlin_comp", Scope, VK_FORMAT_R8_UNORM, imageSize, { frequency, octaves });
}

TAuto<VulkanImage> GRNoise::GenerateWorley(const RenderScope& Scope, VkExtent3D imageSize, uint32_t frequency, uint32_t octaves)
{
	return generate("worley_comp", Scope, VK_FORMAT_R8_UNORM, imageSize, { frequency, octaves });
}

TAuto<VulkanImage> GRNoise::GenerateWorleyPerlin(const RenderScope& Scope, VkExtent3D imageSize, uint32_t frequency, uint32_t worley_octaves, uint32_t perlin_octaves)
{
	return generate("worley_perlin_comp", Scope, VK_FORMAT_R8_UNORM, imageSize, { frequency, worley_octaves, perlin_octaves });
}

TAuto<VulkanImage> GRNoise::GenerateCloudShapeNoise(const RenderScope& Scope, VkExtent3D imageSize, uint32_t worley_frequency, uint32_t perlin_frequency)
{
	uint32_t seed = 0;
	seed = (uint32_t)&seed;
	return generate("cloud_shape_comp", Scope, VK_FORMAT_R8_UNORM, imageSize, { worley_frequency, perlin_frequency, seed });
}

TAuto<VulkanImage> GRNoise::GenerateCloudDetailNoise(const RenderScope& Scope, VkExtent3D imageSize, uint32_t frequency, uint32_t octaves)
{
	uint32_t seed = 0;
	seed = (uint32_t)&seed;
	return generate("cloud_detail_comp", Scope, VK_FORMAT_B10G11R11_UFLOAT_PACK32, imageSize, { frequency, octaves, seed });
}

TAuto<VulkanImage> GRNoise::GenerateCheckerBoard(const RenderScope& Scope, VkExtent2D imageSize, uint32_t frequency)
{
	return generate("checkerboard", Scope, VK_FORMAT_R8_UNORM, { imageSize.width, imageSize.height, 1u }, { frequency });
}