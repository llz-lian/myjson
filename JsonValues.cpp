#include"JsonValues.h"
#include"Node.h"

JsonValue::JsonValue() {};
JsonValue::JsonValue(Value &&val) :__value(std::move(val)) {}
JsonValue::JsonValue(Value &&val, bool array_type)
	:__value(std::move(val)), array_type(array_type) {}
JsonValue::JsonValue(const JsonValue & v)
	:__value(v.__value), array_type(v.array_type), type(v.type) {}
JsonValue::JsonValue(JsonValue && v)
	:__value(std::move(v.__value)), array_type(v.array_type), type(v.type) {}


JSONTYPE JsonValue::getType()
{
	if (type != JSONTYPE::NO_TYPE)
		return type;
	std::visit(
		[&](auto && arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, double>)
			{
				this->type = JSONTYPE::NUMBER_TYPE;
			}
			else if constexpr (std::is_same_v<T, String>)
			{
				this->type = JSONTYPE::STRING_TYPE;
			}
			else if constexpr (std::is_same_v<T, Null>)
			{
				this->type = JSONTYPE::NULL_TYPE;
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				this->type = JSONTYPE::BOOL_TYPE;
			}
			else if constexpr (std::is_same_v<T, JsonArray>)
			{
				this->type = this->array_type ? JSONTYPE::ARRAY_TYPE : JSONTYPE::JSON_TYPE;
				JsonArray & this_array = std::get<JsonArray>(__value);
				for (auto & item : this_array)
				{
					JsonNode & node = const_cast<JsonNode&>(item);//  dangerous !
					node.valueType();
				}
			}
		}, __value);
	return type;
}
std::string JsonValue::toString()const
{
	std::string result;
	std::visit(
		[&](auto && arg)
		{
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, double>)
			{
				result += std::to_string(arg);
			}
			else if constexpr (std::is_same_v<T, String>)
			{
				result.push_back('\"');
				result += arg;
				result.push_back('\"');
			}
			else if constexpr (std::is_same_v<T, Null>)
			{
				result += "null";
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				result += arg ? "true" : "false";
			}
			else if constexpr (std::is_same_v<T, JsonArray>)
			{
				if (this->array_type)
				{
					result += '[';
					for (auto && v : arg)
					{
						result += v.toString() + ",";
					}
					result.pop_back();
					result += ']';
				}
				else
				{
					result += '{';
					for (auto && v : arg)
					{
						result += v.toString() + ",";
					}
					result.pop_back();
					result += '}';
				}
			}
		}, __value);
	return result;
}

bool JsonValue::eq(const JsonValue & v)const
{
	if (v.type == JSONTYPE::NO_TYPE || type == JSONTYPE::NO_TYPE)
		return false;
	if (v.type != type)
		return false;
	if (type == JSONTYPE::NULL_TYPE)
		return true;
	if (type == JSONTYPE::BOOL_TYPE)
		return std::get<bool>(__value) == std::get<bool>(v.__value);
	if(type == JSONTYPE::NUMBER_TYPE)
		return std::get<double>(__value) == std::get<double>(v.__value);
	if(type == JSONTYPE::STRING_TYPE)
		return std::get<String>(__value) == std::get<String>(v.__value);
	if (type == JSONTYPE::ARRAY_TYPE)
	{
		const JsonArray & this_array = std::get<JsonArray>(__value);
		const JsonArray & v_array = std::get<JsonArray>(v.__value);
		if (this_array.size() != v_array.size())
			return false;
		auto this_it = this_array.begin();
		auto v_it = v_array.begin();
		while (this_it != this_array.end())
		{
			if (!this_it->valueEq(*v_it))
				return false;
			this_it++;
			v_it++;
		}
	}
	return true;
}
