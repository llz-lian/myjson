#pragma once
#include<string>
#include<set>
#include<unordered_set>
#include<variant>
#include<iostream>
enum JSONTYPE:int { NULL_TYPE, BOOL_TYPE, NUMBER_TYPE,STRING_TYPE, ARRAY_TYPE, JSON_TYPE ,NO_TYPE = -1};
class JsonNode;
using String = std::string;
using Null = std::monostate;
using JsonArray = std::multiset <JsonNode>;
using Value = std::variant<Null, bool, double, String, JsonArray>;

struct JsonValue
{
	JsonValue();
	JsonValue(const Value &&val);
	JsonValue(const Value &&val, bool array_type);
	JsonValue(const JsonValue & v);
	JsonValue(JsonValue && v);

	Value __value = std::monostate{};
	bool array_type = false;
	JSONTYPE type = JSONTYPE::NO_TYPE;

	JSONTYPE getType();
	std::string toString()const;
	bool eq(const JsonValue & v)const ;
};