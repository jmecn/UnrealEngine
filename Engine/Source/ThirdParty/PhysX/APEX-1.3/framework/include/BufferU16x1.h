/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */


// This file was generated by NxParameterized/scripts/GenParameterized.pl
// Created: 2015.06.02 04:11:41

#ifndef HEADER_BufferU16x1_h
#define HEADER_BufferU16x1_h

#include "NxParametersTypes.h"

#ifndef NX_PARAMETERIZED_ONLY_LAYOUTS
#include "NxParameterized.h"
#include "NxParameters.h"
#include "NxParameterizedTraits.h"
#include "NxTraitsInternal.h"
#endif

namespace physx
{
namespace apex
{

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to __declspec(align())

namespace BufferU16x1NS
{


struct U16_DynamicArray1D_Type
{
	physx::PxU16* buf;
	bool isAllocated;
	physx::PxI32 elementSize;
	physx::PxI32 arraySizes[1];
};


struct ParametersStruct
{

	U16_DynamicArray1D_Type data;

};

static const physx::PxU32 checksum[] = { 0x3c20aee2, 0x4e6abe8c, 0x8c1e5625, 0x7cabf8fc, };

} // namespace BufferU16x1NS

#ifndef NX_PARAMETERIZED_ONLY_LAYOUTS
class BufferU16x1 : public NxParameterized::NxParameters, public BufferU16x1NS::ParametersStruct
{
public:
	BufferU16x1(NxParameterized::Traits* traits, void* buf = 0, PxI32* refCount = 0);

	virtual ~BufferU16x1();

	virtual void destroy();

	static const char* staticClassName(void)
	{
		return("BufferU16x1");
	}

	const char* className(void) const
	{
		return(staticClassName());
	}

	static const physx::PxU32 ClassVersion = ((physx::PxU32)0 << 16) + (physx::PxU32)0;

	static physx::PxU32 staticVersion(void)
	{
		return ClassVersion;
	}

	physx::PxU32 version(void) const
	{
		return(staticVersion());
	}

	static const physx::PxU32 ClassAlignment = 8;

	static const physx::PxU32* staticChecksum(physx::PxU32& bits)
	{
		bits = 8 * sizeof(BufferU16x1NS::checksum);
		return BufferU16x1NS::checksum;
	}

	static void freeParameterDefinitionTable(NxParameterized::Traits* traits);

	const physx::PxU32* checksum(physx::PxU32& bits) const
	{
		return staticChecksum(bits);
	}

	const BufferU16x1NS::ParametersStruct& parameters(void) const
	{
		BufferU16x1* tmpThis = const_cast<BufferU16x1*>(this);
		return *(static_cast<BufferU16x1NS::ParametersStruct*>(tmpThis));
	}

	BufferU16x1NS::ParametersStruct& parameters(void)
	{
		return *(static_cast<BufferU16x1NS::ParametersStruct*>(this));
	}

	virtual NxParameterized::ErrorType getParameterHandle(const char* long_name, NxParameterized::Handle& handle) const;
	virtual NxParameterized::ErrorType getParameterHandle(const char* long_name, NxParameterized::Handle& handle);

	void initDefaults(void);

protected:

	virtual const NxParameterized::DefinitionImpl* getParameterDefinitionTree(void);
	virtual const NxParameterized::DefinitionImpl* getParameterDefinitionTree(void) const;


	virtual void getVarPtr(const NxParameterized::Handle& handle, void*& ptr, size_t& offset) const;

private:

	void buildTree(void);
	void initDynamicArrays(void);
	void initStrings(void);
	void initReferences(void);
	void freeDynamicArrays(void);
	void freeStrings(void);
	void freeReferences(void);

	static bool mBuiltFlag;
	static NxParameterized::MutexType mBuiltFlagMutex;
};

class BufferU16x1Factory : public NxParameterized::Factory
{
	static const char* const vptr;

public:
	virtual NxParameterized::Interface* create(NxParameterized::Traits* paramTraits)
	{
		// placement new on this class using mParameterizedTraits

		void* newPtr = paramTraits->alloc(sizeof(BufferU16x1), BufferU16x1::ClassAlignment);
		if (!NxParameterized::IsAligned(newPtr, BufferU16x1::ClassAlignment))
		{
			NX_PARAM_TRAITS_WARNING(paramTraits, "Unaligned memory allocation for class BufferU16x1");
			paramTraits->free(newPtr);
			return 0;
		}

		memset(newPtr, 0, sizeof(BufferU16x1)); // always initialize memory allocated to zero for default values
		return NX_PARAM_PLACEMENT_NEW(newPtr, BufferU16x1)(paramTraits);
	}

	virtual NxParameterized::Interface* finish(NxParameterized::Traits* paramTraits, void* bufObj, void* bufStart, physx::PxI32* refCount)
	{
		if (!NxParameterized::IsAligned(bufObj, BufferU16x1::ClassAlignment)
		        || !NxParameterized::IsAligned(bufStart, BufferU16x1::ClassAlignment))
		{
			NX_PARAM_TRAITS_WARNING(paramTraits, "Unaligned memory allocation for class BufferU16x1");
			return 0;
		}

		// Init NxParameters-part
		// We used to call empty constructor of BufferU16x1 here
		// but it may call default constructors of members and spoil the data
		NX_PARAM_PLACEMENT_NEW(bufObj, NxParameterized::NxParameters)(paramTraits, bufStart, refCount);

		// Init vtable (everything else is already initialized)
		*(const char**)bufObj = vptr;

		return (BufferU16x1*)bufObj;
	}

	virtual const char* getClassName()
	{
		return (BufferU16x1::staticClassName());
	}

	virtual physx::PxU32 getVersion()
	{
		return (BufferU16x1::staticVersion());
	}

	virtual physx::PxU32 getAlignment()
	{
		return (BufferU16x1::ClassAlignment);
	}

	virtual const physx::PxU32* getChecksum(physx::PxU32& bits)
	{
		return (BufferU16x1::staticChecksum(bits));
	}
};
#endif // NX_PARAMETERIZED_ONLY_LAYOUTS

} // namespace apex
} // namespace physx

#pragma warning(pop)

#endif
