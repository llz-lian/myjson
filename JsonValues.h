#pragma once
#include<string>
#include<set>
#include<unordered_set>
#include<variant>
#include<iostream>
enum class JSONTYPE:int { NULL_TYPE, BOOL_TYPE, NUMBER_TYPE,STRING_TYPE, ARRAY_TYPE, JSON_TYPE ,NO_TYPE = -1};
class JsonNode;
using String = std::string;
using Null = std::monostate;
using JsonArray = std::multiset <JsonNode>;
using Value = std::variant<Null, bool, double, String, JsonArray>;

struct JsonValue
{
	JsonValue();
	JsonValue(Value &&val);
	JsonValue(Value &&val, bool array_type);
	JsonValue(const JsonValue & v);
	JsonValue(JsonValue && v);

	template<class T,
		class U = typename std::enable_if<std::is_integral_v<T>, void>::type>
	JsonValue(T t)
	{
		if constexpr (std::is_same_v<bool, T>)
		{
			__value = T(t);
		}
		else 
		{
			double v;
			v = t;
			__value = v;
		}
	}

	template<class T, 
		class U = typename std::enable_if<std::is_integral_v<T>, void>::type>
	operator T() const
	{
		if (!canGetValue())
			throw std::runtime_error("value wrong convert.");
		if(std::is_same_v<T,bool>)
			return std::get<bool>(__value);
		return std::get<double>(__value);
	}
	operator String()const
	{
		if (!canGetValue()||type!=JSONTYPE::STRING_TYPE)
			throw std::runtime_error("value wrong convert.");
		return std::get<String>(__value);
	}

	bool canGetValue()const
	{
		return !(type == JSONTYPE::NO_TYPE || type == JSONTYPE::ARRAY_TYPE || type == JSONTYPE::JSON_TYPE);
	}


	Value __value = std::monostate{};
	bool array_type = false;
	JSONTYPE type = JSONTYPE::NO_TYPE;

	JSONTYPE getType();
	JSONTYPE getType() const { return type; };
	std::string toString()const;
	bool eq(const JsonValue & v)const ;
};