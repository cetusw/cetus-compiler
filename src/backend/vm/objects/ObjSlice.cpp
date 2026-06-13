#include "ObjSlice.h"

#include "ObjArray.h"

ObjSlice::ObjSlice(std::shared_ptr<ObjArray> storage, const int offset, const int length, const int capacity)
	: Obj(ObjType::SLICE)
	, m_storage(std::move(storage))
	, m_offset(offset)
	, m_length(length)
	, m_capacity(capacity)
{
}

int ObjSlice::Length() const
{
	return m_length;
}

int ObjSlice::Capacity() const
{
	return m_capacity;
}

int ObjSlice::Offset() const
{
	return m_offset;
}

const std::shared_ptr<ObjArray>& ObjSlice::GetStorage() const
{
	return m_storage;
}

int ObjSlice::NewLength(const std::vector<Value>& values) const
{
	return m_length + static_cast<int>(values.size());
}

int ObjSlice::NewCapacity(const int newLength) const
{
	int newCapacity = m_capacity > 0 ? m_capacity : 1;
	while (newCapacity < newLength)
	{
		newCapacity *= 2;
	}
	return newCapacity;
}

void ObjSlice::EnsureStorageLength(const int length) const
{
	if (m_storage->Length() < length)
	{
		m_storage->Resize(length);
	}
}

void ObjSlice::CopyValuesToStorage(const std::vector<Value>& values, const int startIndex) const
{
	for (int index = 0; index < static_cast<int>(values.size()); ++index)
	{
		m_storage->Set(startIndex + index, values[static_cast<std::size_t>(index)]);
	}
}

std::shared_ptr<ObjSlice> ObjSlice::AppendInPlace(const std::vector<Value>& values, const int newLength) const
{
	EnsureStorageLength(m_offset + newLength);
	CopyValuesToStorage(values, m_offset + m_length);
	return std::make_shared<ObjSlice>(m_storage, m_offset, newLength, m_capacity);
}

std::shared_ptr<ObjSlice> ObjSlice::CopyToNewStorage(const std::vector<Value>& values, const int newLength, const int newCapacity) const
{
	auto storage = std::make_shared<ObjArray>(std::vector<Value>{});
	storage->Reserve(newCapacity);
	storage->Resize(newLength);
	for (int index = 0; index < m_length; ++index)
	{
		storage->Set(index, m_storage->Get(m_offset + index));
	}
	for (int index = 0; index < static_cast<int>(values.size()); ++index)
	{
		storage->Set(m_length + index, values[static_cast<std::size_t>(index)]);
	}
	return std::make_shared<ObjSlice>(storage, 0, newLength, newCapacity);
}

std::shared_ptr<ObjSlice> ObjSlice::AppendWithGrowth(const std::vector<Value>& values, const int newLength) const
{
	return CopyToNewStorage(values, newLength, NewCapacity(newLength));
}

const Value& ObjSlice::Get(const int index) const
{
	return m_storage->Get(StorageIndex(index));
}

Value* ObjSlice::GetAddress(const int index) const
{
	return m_storage->GetAddress(StorageIndex(index));
}

void ObjSlice::Set(const int index, const Value& value) const
{
	m_storage->Set(StorageIndex(index), value);
}

std::shared_ptr<ObjSlice> ObjSlice::SubSlice(const int start, const int end) const
{
	return std::make_shared<ObjSlice>(m_storage, m_offset + start, end - start, m_capacity - start);
}

std::shared_ptr<ObjSlice> ObjSlice::Append(const std::vector<Value>& values) const
{
	const int newLength = NewLength(values);
	if (newLength <= m_capacity)
	{
		return AppendInPlace(values, newLength);
	}

	return AppendWithGrowth(values, newLength);
}

int ObjSlice::StorageIndex(const int index) const
{
	return m_offset + index;
}
