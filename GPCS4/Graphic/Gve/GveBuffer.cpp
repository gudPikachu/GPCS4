#include "GveBuffer.h"
#include "GveDevice.h"


namespace gve
{;

GveBuffer::GveBuffer(const RcPtr<GveDevice>& device, 
	const GveBufferCreateInfo& createInfo, 
	GveMemoryAllocator& memAlloc, 
	VkMemoryPropertyFlags memFlags):
	m_device(device),
	m_info(createInfo),
	m_memAlloc(&memAlloc),
	m_memFlags(memFlags)
{
	do
	{
		VkBufferCreateInfo info;
		convertCreateInfo(m_info, info);

		if (vkCreateBuffer(*m_device, &info, nullptr, &m_buffer) != VK_SUCCESS) 
		{
			LOG_ERR("Failed to create buffer");
			break;
		}

		VkMemoryDedicatedRequirementsKHR dedicatedRequirements;
		dedicatedRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS_KHR;
		dedicatedRequirements.pNext = VK_NULL_HANDLE;
		dedicatedRequirements.prefersDedicatedAllocation = VK_FALSE;
		dedicatedRequirements.requiresDedicatedAllocation = VK_FALSE;

		VkMemoryRequirements2 memReq;
		memReq.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2_KHR;
		memReq.pNext = &dedicatedRequirements;

		VkBufferMemoryRequirementsInfo2 memReqInfo;
		memReqInfo.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2_KHR;
		memReqInfo.buffer = m_buffer;
		memReqInfo.pNext = VK_NULL_HANDLE;

		VkMemoryDedicatedAllocateInfoKHR dedMemoryAllocInfo;
		dedMemoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO_KHR;
		dedMemoryAllocInfo.pNext = VK_NULL_HANDLE;
		dedMemoryAllocInfo.buffer = m_buffer;
		dedMemoryAllocInfo.image = VK_NULL_HANDLE;

		vkGetBufferMemoryRequirements2(*m_device, &memReqInfo, &memReq);

		// Use high memory priority for GPU-writable resources
		bool isGpuWritable = (info.usage & (
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
			VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT)) != 0;

		float priority = isGpuWritable ? 1.0f : 0.5f;

		// Ask driver whether we should be using a dedicated allocation
		m_memory = m_memAlloc->alloc(&memReq.memoryRequirements,
			dedicatedRequirements, dedMemoryAllocInfo, m_memFlags, priority);

		if (vkBindBufferMemory(*m_device, m_buffer, m_memory.memory(), m_memory.offset()) != VK_SUCCESS)
		{
			LOG_ERR("Failed to bind device memory");
			break;
		}
			
	} while (false);
}

GveBuffer::~GveBuffer()
{
	vkDestroyBuffer(*m_device, m_buffer, nullptr);
}


VkBuffer GveBuffer::handle() const
{
	return m_buffer;
}

const GnmBuffer* GveBuffer::getGnmBuffer() const
{
	return &m_info.buffer;
}

void GveBuffer::convertCreateInfo(const GveBufferCreateInfo& gveInfo, VkBufferCreateInfo& vkInfo)
{
	memset(&vkInfo, 0, sizeof(vkInfo));
	// TODO:
	vkInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkInfo.size = gveInfo.buffer.getSize();
	vkInfo.usage = m_info.usage;
	vkInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

///

GveBufferView::GveBufferView()
{
}

GveBufferView::~GveBufferView()
{
}



}  // namespace gve