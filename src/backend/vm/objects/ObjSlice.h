#pragma once

#include "Obj.h"
#include "src/backend/vm/types/Value.h"

#include <memory>
#include <vector>

class ObjArray;

class ObjSlice final : public Obj
{
public:
	ObjSlice(std::shared_ptr<ObjArray> storage, int offset, int length, int capacity);

	[[nodiscard]] int Length() const;
	[[nodiscard]] int Capacity() const;
	[[nodiscard]] const Value& Get(int index) const;
	void Set(int index, const Value& value) const;
	[[nodiscard]] std::shared_ptr<ObjSlice> Append(const std::vector<Value>& values) const;

private:
	[[nodiscard]] int NewLength(const std::vector<Value>& values) const;
	[[nodiscard]] int NewCapacity(int newLength) const;
	[[nodiscard]] std::shared_ptr<ObjSlice> AppendInPlace(const std::vector<Value>& values, int newLength) const;
	[[nodiscard]] std::shared_ptr<ObjSlice> AppendWithGrowth(const std::vector<Value>& values, int newLength) const;
	void EnsureStorageLength(int length) const;
	void CopyValuesToStorage(const std::vector<Value>& values, int startIndex) const;
	[[nodiscard]] std::shared_ptr<ObjSlice> CopyToNewStorage(const std::vector<Value>& values, int newLength, int newCapacity) const;
	[[nodiscard]] int StorageIndex(int index) const;

	std::shared_ptr<ObjArray> m_storage;
	int m_offset = 0;
	int m_length = 0;
	int m_capacity = 0;
};
