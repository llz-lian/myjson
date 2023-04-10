#include<ostream>
#include<fstream>
#include<filesystem>
#include"Json.h"
#include"Node.h"
Json::Json() :jsons(JsonArray()) {};
Json::~Json() {};
Json::Json(const Json & js) :jsons(js.jsons) {}
Json::Json(JsonArray&& a)
{
	jsons = std::move(a);
}
;
Json::Json(Json && js) :jsons(js.jsons) {};
Json::Json(const CanJson & cls) {
	jsons = cls.toJson().jsons;
};
void Json::addNode(const JsonNode & jn)
{
	jsons.insert(jn);
}
void Json::addNode(JsonNode && jn)
{
	jsons.insert(std::move(jn));
}
void Json::read(const std::string & file_path) 
{
	std::ifstream infile;
	infile.open(file_path,std::ios::in);
	String str;
	char c;
	while (infile>>c)
	{
		str.push_back(c);
	}
	JsonValue v;
	if (parseJson(str, v) != PARSE_STATE::PARSE_OK)
	{
		throw std::runtime_error("json type wrong");
	}
	jsons = std::move(std::get<JsonArray>(v.__value));
};
void Json::save(const std::string & file_path) 
{
	std::ofstream ofile;
	ofile.open(file_path, std::ios::out);
	ofile << toString();
	ofile.close();
};
const JsonNode & Json::operator [](const std::string & s) const
{
	auto find_ret = jsons.find(JsonNode(s));
	if (find_ret == jsons.end())
		throw std::runtime_error("wrong [] key is: " + s);
	
	return (*find_ret);
}
std::string Json::toString()const
{
	std::string result;
	result += "{\n";
	for (auto & node : jsons)
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