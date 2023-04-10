#pragma once
#include<sstream>
#ifdef DEUBG
#include<cassert>
#endif
#include"Node.h"
#include"Func.h"
enum class PARSE_STATE { PARSE_INVALID_VALUE, PARSE_OK, PARSE_NEED_VALUE, 
						 PARSE_NEED_SINGULAR, PARSE_INVALID_STRING, PARSE_INVALID_ARRAY ,
						 PARSE_NEED_KEY,PARSE_INVALID_OBJECT
};
class Parse
{
protected:
	void __nopBlank(const std::string_view s, size_t & index)
	{
		while (index < s.size() && (s[index] == ' '||s[index]=='\t'||s[index] == '\n'||s[index] == '\r'))
			index++;
	}
	bool __checkStr(const std::string_view s, size_t & index, const std::string_view & target)
	{
		auto check = [&](char c) {return index < s.size() && s[index++] == c; };
		for (size_t i = 0; i < target.size(); i++)
		{
			if (!check(target[i]))
				return false;
		}
		return true;
	}

	auto __getRawString(const std::string_view s, size_t& index)
	{
		if (s.size() < 2)
			return std::pair{PARSE_STATE::PARSE_INVALID_STRING,String("")};
		std::stringstream ss;
		index++;
		bool tail = false;
		for (; !tail; index++)
		{
			if (index == s.size())
				return std::pair{ PARSE_STATE::PARSE_INVALID_STRING ,String("") };
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
					return std::pair{ PARSE_STATE::PARSE_INVALID_STRING ,String("") };
				}
				break;
			default:
				if ((unsigned char)(c) < 0x20)
					return std::pair{ PARSE_STATE::PARSE_INVALID_STRING ,String("") };
				ss << c;
				break;
			}
		}
		return std::pair{ PARSE_STATE::PARSE_OK ,ss.str()};
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
				return PARSE_STATE::PARSE_INVALID_VALUE;
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
				return PARSE_STATE::PARSE_INVALID_VALUE;
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
			if (index >= s.size()) return PARSE_STATE::PARSE_INVALID_VALUE;
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
		return PARSE_STATE::PARSE_OK;
	}
	PARSE_STATE __parseString(const std::string_view s, size_t & index, JsonValue & v)
	{
		// tail == '"'
		// head == '"'
		auto [ret, str] = __getRawString(s, index);
		if (ret != PARSE_STATE::PARSE_OK)
			return ret;
		v.__value = std::move(str);
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
	PARSE_STATE __parseJsonObj(const std::string_view s, size_t & index, JsonArray& v)
	{
		// {   "key1"  :  |val1| , "key2" :|val2| , ....}
		index++;
		if (index == s.size())
			return PARSE_STATE::PARSE_INVALID_OBJECT;
		if (s[index] == '}')
		{
			// reach obj tail;
			index++;
			return PARSE_STATE::PARSE_OK;
		}
		// bbbb"key"bbbbb:bbbbb|val|
		for (; index < s.size();)
		{
			__nopBlank(s, index);
			if (index == s.size()) break;
			if (s[index] == '}')
			{
				index++;
				return PARSE_STATE::PARSE_OK;
			}
			auto key_ret = __getRawString(s, index);
			if (key_ret.first != PARSE_STATE::PARSE_OK)
				return key_ret.first;
			__nopBlank(s, index);
			if (index == s.size()) break;
			if (s[index] == ':') index++;
			__nopBlank(s, index);
			if (index == s.size()) break;
			JsonValue sub_value;
			auto sub_ret = __parseValue(s, index, sub_value);
			if (sub_ret != PARSE_STATE::PARSE_OK)
			{
				return sub_ret;
			}
			v.insert(JsonNode(std::move(key_ret.second), std::move(sub_value)));
			__nopBlank(s, index);
			if (index == s.size()) break;
			if (s[index] == ',') index++;
			__nopBlank(s, index);
		}
		return PARSE_STATE::PARSE_OK;
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
			ret = __parseJsonObj(s, index, js_array);
			break;
		default: return __parseNum(s, index, v);
		}
		if (array_type&&ret == PARSE_STATE::PARSE_OK)
		{
			v.__value = std::move(js_array);
			v.array_type = true;
			v.type = JSONTYPE::ARRAY_TYPE;
		}
		else
		{
			v.__value = std::move(js_array);
			v.type = JSONTYPE::JSON_TYPE;
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
template<class> inline constexpr bool is_array_or_vector_v = false;
template<class T, class A> inline constexpr bool is_array_or_vector_v<std::vector<T, A>> = true;
template<class T,int n> inline constexpr bool is_array_or_vector_v<std::array<T,n>> = true;
template<class> inline constexpr bool has_sub_array = false;
template<class T, class A> inline constexpr bool has_sub_array<std::vector<T, A>> = is_array_or_vector_v<T>;


class CanJson
{
private:
	template<class T>
	JsonValue __genValue(const T& t) const
	{
		if constexpr (is_array_or_vector_v<T>)
		{
			//vector or array
			JsonArray json_array;
			for (size_t i = 0; i < t.size(); i++)
			{
				if constexpr (has_sub_array<T>)
				{
					JsonValue v = genValue(t[i]);
					json_array.insert(JsonNode("", v));
				}
				else
				{
					json_array.insert(JsonNode("", JsonValue(t[i])));
				}
			}
			return JsonValue(std::move(json_array), true);
		}
		else
		{
			return JsonValue(T(t));
		}
		return JsonValue();
	}
public:
	virtual Json toJson() const = 0;
	void insertJsonArray(JsonArray & a,const String & k,const JsonValue & v)const
	{
		a.insert(JsonNode(k, v));
	}
	JsonValue genValue()const
	{
		return JsonValue();
	}
	template<class T>
	JsonValue genValue(const T & t) const
	{
		JsonValue v = __genValue(t);
		v.getType();
		return v;
	}
};
class Json:protected Parse
{
public:
	Json();
	~Json();
	Json(const Json & js);
	Json(JsonArray && a);
	Json(Json && js);
	Json(const CanJson & cls);
	void addNode(const JsonNode & jn);
	void addNode(JsonNode && jn);
	void read(const std::string & file_path);
	void save(const std::string & target_path);
	const JsonNode & operator [](const std::string & s) const;
	std::string toString()const;
	JsonArray jsons;
};