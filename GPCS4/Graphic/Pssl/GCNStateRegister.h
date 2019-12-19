#pragma once

#include "PsslCommon.h"
#include "GCNSpirvTypes.h"

#include <string>
#include <optional>

namespace pssl
{;

class GCNCompiler;
class SpirvModule;

class SpirvRegisterU64
{
	enum class RegType
	{
		REG64_LOW,
		REG64_HIGH,
		REG64_FULL,
	};

public:
	SpirvRegisterU64(
		GCNCompiler* compiler, 
		const std::string& name = nullptr, 
		std::optional<uint32_t> initValue = std::nullopt);
	~SpirvRegisterU64();

	// lower 32bit uint32
	SpirvRegisterPointer low();

	// higher 32bit uint32
	SpirvRegisterPointer high();

	// whole 64bit uint64
	SpirvRegisterPointer value();

private:
	SpirvRegisterPointer createU64Value(const std::string& name, std::optional<uint32_t> initValue);
	SpirvRegisterPointer castToVec2(SpirvRegisterPointer u64Val);
	SpirvRegisterPointer mapAccessPtr(RegType type);

private:
	GCNCompiler* m_compiler;
	SpirvModule* m_module;
	std::string m_name;
	std::optional<uint32_t> m_initValue;

	SpirvRegisterPointer m_value;
	SpirvRegisterPointer m_vec2Ptr;

	// We cast a uint64 value to a vec2 (two uint32),
	// so that we can access both low and high part.
	SpirvRegisterPointer m_low;
	SpirvRegisterPointer m_high;
};


// For all status registers related to thread,
// we assume we are on 0's thread,
// ie. exec will be initialized with 1 .
struct GcnStateRegister
{
	GcnStateRegister(GCNCompiler* compiler):
		vcc(compiler, "vcc"),
		exec(compiler, "exec", 1)
	{}

	SpirvRegisterU64 vcc;
	SpirvRegisterU64 exec;

	//SpirvRegisterPointer scc;
	SpirvRegisterPointer m0;

	// spirv condition id
	uint32_t sccz;  // There's no hardware sccz, branch instruction detect if scc is 0 or 1
	uint32_t vccz;
	uint32_t execz;
};


}  // namespace pssl

