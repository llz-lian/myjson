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
	const std::string & getKey() const;
	JSONTYPE valueType() 
	{
		return __value.getType();
	}
	JSONTYPE valueType()const
	{
		return __value.getType();
	}

	template<class T,
		class U = typename std::enable_if<std::is_integral_v<T>, void>::type>
	operator T() const
	{
		if (!__value.canGetValue())
			throw std::runtime_error("value wrong convert.");
		return __value;
	}

	operator String()const
	{
		if (!__value.canGetValue())
			throw std::runtime_error("value wrong convert.");
		return String(__value);
	}
	auto iterArrayValue()const
	{
		if (!(__value.getType() == JSONTYPE::JSON_TYPE || __value.getType() == JSONTYPE::ARRAY_TYPE))
			throw std::runtime_error("not a array");
		return std::pair{ std::get<JsonArray>(__value.__value).begin(),std::get<JsonArray>(__value.__value).end()};
	}
private:
	std::string __key;
	JsonValue __value;
};
