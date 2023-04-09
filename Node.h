#pragma once
#include<iostream>
#include"JsonValues.h"
class JsonNode
{
public:
	friend inline std::ostream & operator <<(std::ostream & o, const JsonNode & jn);
	explicit JsonNode(const std::string & key);
	explicit JsonNode(const std::string & key, const JsonValue&val);

	JsonNode(JsonNode && node);
	JsonNode(const JsonNode & node);
	std::string toString()const;
	bool operator<(const JsonNode & b)const;

	bool valueEq(const JsonNode & b)const;

	const JsonNode & operator [](const std::string & s);
	std::string & getKey();
	JSONTYPE valueType() {
		return __value.getType();
	}
private:
	std::string __key;
	JsonValue __value;
};