#include"Json.h"
#include"Node.h"
Json::Json() :jsons(std::make_shared<JsonArray>()) {};
Json::~Json() {};
Json::Json(const Json & js) :jsons(js.jsons) {};
Json::Json(Json && js) :jsons(js.jsons) {};
Json::Json(const CanJson & cls) {};
void Json::addNode(const JsonNode & jn)
{
	jsons->insert(jn);
}
void Json::addNode(const JsonNode && jn)
{
	jsons->insert(std::move(jn));
}
void Json::parse(const std::string_view s) {};
void Json::read(const std::string & file_path) {};
void Json::dump() {};
const JsonNode & Json::operator [](const std::string & s)
{
	auto find_ret = jsons->find(JsonNode(s));
	if (find_ret == jsons->end())
		throw std::runtime_error("wrong [] key is: " + s);
	return (*find_ret);
}
std::string Json::toString()const
{
	std::string result;
	result += "{\n";
	for (auto & node : *jsons)
	{
		result.push_back('\t');
		result += node.toString();
		result.push_back(',');
		result.push_back('\n');
	}
	result.pop_back();
	result.pop_back();
	result += "\n}\n";
	return result;
}