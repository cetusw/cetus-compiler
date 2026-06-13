#include "ObjAssertionMetadata.h"

#include <utility>

ObjAssertionMetadata::ObjAssertionMetadata(AssertionDescriptor descriptor)
	: Obj(ObjType::ASSERTION_METADATA)
	, m_descriptor(std::move(descriptor))
{
}

const AssertionDescriptor& ObjAssertionMetadata::GetDescriptor() const
{
	return m_descriptor;
}
