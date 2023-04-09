#pragma once
#include<sstream>
#ifdef DEUBG
#include<cassert>
#endif
#include"Node.h"
#include"Func.h"
enum PARSE_STATE { PARSE_INVALID_VALUE, PARSE_OK, PARSE_NEED_VALUE, PARSE_NEED_SINGULAR,PARSE_INVALID_STRING ,PARSE_INVALID_ARRAY};
class Parse
{
protected:
	void __nopBlank(const std::string_view s, size_t & index)
	{
		while (index < s.size() && s[index] == ' ')
			index++;
	}
	bool __checkStr(const std::string_view s, size_t & index, const std::string_view & target)
	{
		auto check = [&](char c) {return index < s.size() && s[index++] == c; };
		for (int i = 0; i < target.size(); i++)
		{
			if (!check(target[i]))
				return false;
		}
		return true;
	}
	PARSE_STATE __parseTrue(const std::string_view s, size_t & index, JsonValue & v)
	{
		// start with t
		// target true
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		if (!__checkStr(s,index,"true"))
			return PARSE_STATE::PARSE_INVALID_VALUE;
		v.__value = true;
		v.type = JSONTYPE::BOOL_TYPE;
		return PARSE_STATE::PARSE_OK;
	}
	PARSE_STATE __parseFalse(const std::string_view s, size_t & index, JsonValue & v)
	{
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		if (!__checkStr(s,index,"false"))
			return PARSE_STATE::PARSE_INVALID_VALUE;
		v.__value = false;
		v.type = JSONTYPE::BOOL_TYPE;
		return PARSE_STATE::PARSE_OK;
	}
	PARSE_STATE __parseNull(const std::string_view s, size_t & index, JsonValue & v)
	{
		//start with n*
		// target null
		// n->u->l->l
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		if (!__checkStr(s,index,"null"))
			return PARSE_STATE::PARSE_INVALID_VALUE;
		v.__value = std::monostate{};
		v.type = JSONTYPE::NULL_TYPE;
		return PARSE_STATE::PARSE_OK;
	}
	PARSE_STATE __parseBool(const std::string_view s, size_t & index, JsonValue & v)
	{
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		return s[index] == 't' ? __parseTrue(s, index, v) : __parseFalse(s, index, v);
	}
	PARSE_STATE __parseNum(const std::string_view s, size_t & index, JsonValue & v)
	{
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		auto tmp = index;
		if (s[index] == '-') index++;
		if(index<s.size()&&s[index] == '0') index++;
		else
		{
			if(!(s[index]>='1'&&s[index]<='9'))
				return PARSE_INVALID_VALUE;
			for (; index < s.size(); index++)
			{
				if (!(isdigit(s[index])))
					break;
			}
		}
		// . or e
		if (index < s.size() && s[index] == '.')
		{
			index++;
			if (index >= s.size()||!(s[index] >= '0'&&s[index] <= '9'))
				return PARSE_INVALID_VALUE;
			for (; index < s.size(); index++)
			{
				if (!(isdigit(s[index])))
					break;
			}
		}
		if (index < s.size() && (s[index] == 'e' || s[index] == 'E'))
		{
			index++;
			if (index < s.size()&&(s[index] == '+' || s[index] == '-')) index++;
			if (index >= s.size()) return PARSE_INVALID_VALUE;
			for (; index < s.size(); index++)
			{
				if (!(isdigit(s[index])))
					break;
			}
		}
		const std::string nums_str(s.substr(tmp, index));
		std::stringstream ss;
		ss << nums_str;
		double num;
		ss >> num;
		v.__value = num;
		v.type = JSONTYPE::NUMBER_TYPE;
		return PARSE_OK;
	}
	PARSE_STATE __parseString(const std::string_view s, size_t & index, JsonValue & v)
	{
		// tail == '"'
		// head == '"'
		if (s.size() < 2)
			return PARSE_INVALID_STRING;
		std::string str;
		std::stringstream ss;
		index++;
		bool tail = false;
		for (;!tail; index++)
		{
			if(index==s.size())
				return PARSE_STATE::PARSE_INVALID_STRING;
			char c = s[index];
			switch (c)
			{
			case '"':
				tail = true;
				break;
			case '\\':
				index++;
				switch (s[index])
				{
				case '\\':
					ss << '\\';
					break;
				case 'n':
					ss << '\n';
					break;
				case 'b':
					ss << '\b';
					break;
				case 't':
					ss << '\t';
					break;
				case 'f':
					ss << '\f';
					break;
				case 'r':
					ss << '\r';
					break;
				case '/':
					ss << '/';
					break;
				case '\"':
					ss << '"';
					break;
				default:
					return PARSE_STATE::PARSE_INVALID_STRING;
				}
				break;
			default:
				if((unsigned char)(c)<0x20)
					return PARSE_STATE::PARSE_INVALID_STRING;
				ss << c;
				break;
			}
		}
		v.__value = ss.str();
		v.type = JSONTYPE::STRING_TYPE;
		return PARSE_STATE::PARSE_OK;
	}
	PARSE_STATE __parseArray(const std::string_view s, size_t & index, JsonArray & v)
	{
		// s[index] == '['
		index++;
		if (index == s.size())
			return PARSE_STATE::PARSE_INVALID_ARRAY;
		if (s[index] == ']')
		{
			// reach array tail;
			index++;
			return PARSE_STATE::PARSE_OK;
		}
		for (; index < s.size();)
		{
			__nopBlank(s, index);
			if (index == s.size()) break;
			if (s[index] == ',') index++;
			__nopBlank(s, index);
			if (index == s.size()) break;
			if (s[index] == ']')
			{
				index++;
				return PARSE_STATE::PARSE_OK;
			}
			JsonValue sub_value;
			auto sub_ret = __parseValue(s, index , sub_value);
			if (sub_ret!=PARSE_STATE::PARSE_OK)
			{
				return sub_ret;
			}
			v.insert(JsonNode("", std::move(sub_value)));
			__nopBlank(s, index);
		}
		return PARSE_STATE::PARSE_INVALID_ARRAY;
	}
	PARSE_STATE __parseJsonObj(const std::string_view s, size_t & index, JsonValue & v)
	{
		return PARSE_OK;
	}
	PARSE_STATE __parseValue(const std::string_view s, size_t & index, JsonValue & v)
	{
		if (index == s.size())
			return PARSE_STATE::PARSE_NEED_VALUE;
		JsonArray js_array;
		bool array_type = false;
		PARSE_STATE ret;
		switch (s[index])
		{
		case 't':
		case 'f': return  __parseBool(s, index, v);
		case 'n': return __parseNull(s, index, v);
		case '"': return __parseString(s, index, v);
		case '[': 
			array_type = true;
			ret = __parseArray(s, index, js_array);
			break;
		case '{': 
			return __parseJsonObj(s, index, v);
		default: return __parseNum(s, index, v);
		}
		if (array_type&&ret == PARSE_STATE::PARSE_OK)
		{
			v.__value = std::move(js_array);
			v.array_type = true;
			v.type = JSONTYPE::ARRAY_TYPE;
		}
		return ret;
	}
public:
	PARSE_STATE parseJson(const std::string_view s, JsonValue & v)
	{
		size_t index = 0;
		__nopBlank(s, index);
		PARSE_STATE ret = __parseValue(s, index, v);
		if (ret == PARSE_STATE::PARSE_OK)
		{
			__nopBlank(s, index);
			if (index != s.size())
			{
				v.__value = std::monostate{};
				return PARSE_STATE::PARSE_NEED_SINGULAR;
			}
		}
		return ret;
	}
};
class Json;
class CanJson
{
public:
	virtual Json toJson() = 0;
};
class Json
{
public:
	Json();
	~Json();
	Json(const Json & js);
	Json(Json && js);
	Json(const CanJson & cls);
	void addNode(const JsonNode & jn);
	void addNode(const JsonNode && jn);
	void parse(const std::string_view s);
	void read(const std::string & file_path);
	void dump();
	const JsonNode & operator [](const std::string & s);
	std::string toString()const;

	std::shared_ptr<JsonArray> jsons;
};