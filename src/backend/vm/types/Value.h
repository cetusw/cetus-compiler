#pragma once

#include "src/backend/vm/objects/Obj.h"
#include "src/backend/vm/types/RuntimeTypes.h"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <variant>

class ObjFunction;
class ObjNative;
class ObjRef;
class ObjPointer;
class ObjArray;
class ObjSlice;
class ObjStruct;
class ObjAssertionMetadata;

using HeapCloneCache = std::unordered_map<const Obj*, HeapObject>;

class Value
{
public:
	Value();
	explicit Value(RuntimeInt value);
	explicit Value(RuntimeFloat value);
	explicit Value(RuntimeBool value);
	explicit Value(HeapObject obj);

	[[nodiscard]] bool IsNull() const;
	[[nodiscard]] bool IsInt() const;
	[[nodiscard]] bool IsFloat() const;
	[[nodiscard]] bool IsNumber() const;
	[[nodiscard]] bool IsBool() const;
	[[nodiscard]] bool IsHeapObject() const;
	[[nodiscard]] bool IsString() const;
	[[nodiscard]] bool IsFalsey() const;
	[[nodiscard]] bool IsFunction() const;
	[[nodiscard]] bool IsNative() const;
	[[nodiscard]] bool IsRef() const;
	[[nodiscard]] bool IsPointer() const;
	[[nodiscard]] bool IsArray() const;
	[[nodiscard]] bool IsSlice() const;
	[[nodiscard]] bool IsSequence() const;
	[[nodiscard]] bool IsIndexable() const;
	[[nodiscard]] bool IsStruct() const;
	[[nodiscard]] bool IsAssertionMetadata() const;

	[[nodiscard]] RuntimeInt AsInt() const;
	[[nodiscard]] RuntimeFloat AsFloat() const;
	[[nodiscard]] double AsNumber() const;
	[[nodiscard]] RuntimeBool AsBool() const;
	[[nodiscard]] const std::string& AsString() const;
	[[nodiscard]] std::shared_ptr<ObjFunction> AsFunction() const;
	[[nodiscard]] std::shared_ptr<ObjNative> AsNative() const;
	[[nodiscard]] std::shared_ptr<ObjRef> AsRef() const;
	[[nodiscard]] std::shared_ptr<ObjPointer> AsPointer() const;
	[[nodiscard]] std::shared_ptr<ObjArray> AsArray() const;
	[[nodiscard]] std::shared_ptr<ObjSlice> AsSlice() const;
	[[nodiscard]] std::shared_ptr<ObjStruct> AsStruct() const;
	[[nodiscard]] std::shared_ptr<ObjAssertionMetadata> AsAssertionMetadata() const;
	[[nodiscard]] HeapObject AsHeapObject() const;
	[[nodiscard]] Value Dereference() const;
	[[nodiscard]] Value Clone() const;
	[[nodiscard]] Value Clone(HeapCloneCache& cache) const;

	void Print() const;

	Value operator-() const;
	[[nodiscard]] Value LogicalNot() const;
	Value operator+(const Value& other) const;
	Value operator-(const Value& other) const;
	Value operator*(const Value& other) const;
	Value operator/(const Value& other) const;
	Value operator%(const Value& other) const;
	Value operator==(const Value& other) const;
	Value operator!=(const Value& other) const;
	Value operator>(const Value& other) const;
	Value operator<(const Value& other) const;
	Value operator>=(const Value& other) const;
	Value operator<=(const Value& other) const;

private:
	std::variant<std::nullptr_t, RuntimeInt, RuntimeFloat, RuntimeBool, HeapObject> m_data;
};
