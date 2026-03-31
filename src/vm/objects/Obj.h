#pragma once
#include <memory>

enum class ObjType
{
	STRING,
};

class Obj
{
public:
	explicit Obj(ObjType type);
	virtual ~Obj() = default;

	[[nodiscard]] ObjType GetType() const;

	void SetType(ObjType type);

private:
	ObjType m_type = ObjType::STRING;
};

using HeapObject = std::shared_ptr<Obj>;
