#include"Node.h"
JsonNode::JsonNode(const std::string & key)
	:__key(key) {}
JsonNode::JsonNode(const std::string & key, const JsonValue&val)
	:__key(std::move(key)), __value(std::move(val)) {}

JsonNode::JsonNode(JsonNode && node)
	:__key(std::move(node.__key)), __value(std::move(node.__value)) {}
JsonNode::JsonNode(const JsonNode & node)
	:__key(node.__key), __value(node.__value) {}
std::string JsonNode::toString()const
{
	return __key.size() == 0 ? __value.toString() : "\"" + __key + "\"" + " : " + __value.toString();
}
bool JsonNode::operator<(const JsonNode & b)const
{
	return this->__key < b.__key;
}
bool JsonNode::valueEq(const JsonNode & b) const
{
	return __key == b.__key&&__value.eq(b.__value);
}
const JsonNode & JsonNode::operator [](const std::string & s)
{
	JSONTYPE value_type = __value.getType();
	if (!(value_type == JSONTYPE::JSON_TYPE))
		throw std::runtime_error("wrong [], value type is: " + std::to_string((int)value_type));
	// value must be a set
	auto & json_nodes = std::get<JsonArray>(__value.__value);
	auto find_ret = json_nodes.find(JsonNode(s));
	if (find_ret == json_nodes.end())
		throw std::runtime_error("worng [] key is: " + s);
	return (*find_ret);// a jsonvalue not a node
}
const std::string & JsonNode::getKey() const
{
	return __key;
}