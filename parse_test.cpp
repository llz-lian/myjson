#include"Json.h"

class ParseTest:protected Parse
{
private:
	int test_num = 0;
	int pass_num = 0;
	template<class T>
	bool eq(T target, T  v)
	{
		test_num++;
		pass_num += target == v;
		return target == v;
	}
	void __testInvalid()
	{
		JsonValue v;
		auto testInvalid = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_STATE::PARSE_INVALID_VALUE); };
		testInvalid("+0");
		testInvalid("+1");
		testInvalid(".123");
		testInvalid("1.");
		testInvalid("Inf");
		testInvalid("inf");
		testInvalid("Nan");
		testInvalid("nul");
		testInvalid("naul");
		testInvalid("tru");
		testInvalid("fasle");
		testInvalid("!");

		auto testInvalidStr = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_STATE::PARSE_INVALID_STRING); };
		testInvalidStr("\"");
		testInvalidStr("\"aaaa");
		testInvalidStr("\"\\q\"");
		testInvalidStr("\"\0\"");
		testInvalidStr("\"\\x12\"");
		testInvalidStr("\"\x01\"");

		auto testInvalidArr = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_STATE::PARSE_INVALID_ARRAY); };
		testInvalidArr("[");
		testInvalidArr("[\"abc\",[false,]");
		testInvalidArr("[true,false,[]    ");
	}

	void __testBoolAndNull()
	{
		JsonValue v;
		
		eq(parseJson("true", v), PARSE_STATE::PARSE_OK);
		if (std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), true);

		eq(parseJson("false", v), PARSE_STATE::PARSE_OK);
		if (std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), false);

		eq(parseJson("null", v), PARSE_STATE::PARSE_OK);
		if (std::holds_alternative<Null>(v.__value))
			eq((Null)std::get<Null>(v.__value), Null{});


		eq(parseJson("    true   ", v), PARSE_STATE::PARSE_OK);
		if(std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value),true);

		eq(parseJson("   false   ", v), PARSE_STATE::PARSE_OK);
		if(std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), false);

		eq(parseJson("   null   ", v), PARSE_STATE::PARSE_OK);
		if (std::holds_alternative<Null>(v.__value))
			eq((Null)std::get<Null>(v.__value), Null{});


		eq(parseJson("", v), PARSE_STATE::PARSE_NEED_VALUE);
		eq(parseJson("     ", v), PARSE_STATE::PARSE_NEED_VALUE);

		eq(parseJson("false   x", v), PARSE_STATE::PARSE_NEED_SINGULAR);
		eq(parseJson("  null   x", v), PARSE_STATE::PARSE_NEED_SINGULAR);
		eq(parseJson("  true   x", v), PARSE_STATE::PARSE_NEED_SINGULAR);
	}

	void __testNums()
	{
		JsonValue v;
		auto nums_should_ok = [&](const double num, const std::string_view & s) {
			eq(parseJson(s, v), PARSE_STATE::PARSE_OK);
			if (std::holds_alternative<double>(v.__value))
			{
				eq((double)std::get<double>(v.__value), num);
			}
		};
		nums_should_ok(0.0, "0");
		nums_should_ok(0.0, "-0");
		nums_should_ok(0.0, "-0.0");
		nums_should_ok(1.0, "1");
		nums_should_ok(-1.0, "-1");
		nums_should_ok(1.5, "1.5");
		nums_should_ok(-1.5, "-1.5");
		nums_should_ok(3.1416, "3.1416");
		nums_should_ok(1E10, "1E10");
		nums_should_ok(1e10, "1e10");
		nums_should_ok(1E+10, "1E+10");
		nums_should_ok(1E-10, "1E-10");
		nums_should_ok(-1E10, "-1E10");
		nums_should_ok(-1e10, "-1e10");
		nums_should_ok(-1E+10, "-1E+10");
		nums_should_ok(-1E-10, "-1E-10");
		nums_should_ok(1.234E+10, "1.234E+10");
		nums_should_ok(1.234E-10, "1.234E-10");
		nums_should_ok(0.0, "1e-10000"); 

		nums_should_ok(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
		nums_should_ok(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
		nums_should_ok(-4.9406564584124654e-324, "-4.9406564584124654e-324");
		nums_should_ok(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
		nums_should_ok(-2.2250738585072009e-308, "-2.2250738585072009e-308");
		nums_should_ok(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
		nums_should_ok(-2.2250738585072014e-308, "-2.2250738585072014e-308");
		nums_should_ok(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
		nums_should_ok(-1.7976931348623157e+308, "-1.7976931348623157e+308");
	
	}

	void __testStrings()
	{
		JsonValue v;
		auto testString = [&](const std::string_view target_str,const std::string_view s) {
			eq(parseJson(s, v), PARSE_STATE::PARSE_OK);
			if (std::holds_alternative<String>(v.__value))
			{
				eq((std::string_view)(std::get<String>(v.__value)), target_str);
			}
		};
		testString("", " \"\" ");
		testString("abc", " \"abc\" ");
		testString("good string", "   \"good string\"");
		testString("some bad\n", "\"some bad\\n\"");
		testString("not good \\ /", "\"not good \\\\ /\"");
		testString("who use this?!! \" \\ / \b \f \n \r \t", "\"who use this?!! \\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
	}

	void __testArrays()
	{
		JsonValue v;
		auto testArray = [&](const std::string_view s,JsonValue & target) 
		{
			eq(parseJson(s, v), PARSE_STATE::PARSE_OK);
			if (std::holds_alternative<JsonArray>(v.__value))
			{
				eq(v.eq(target),true);
			}
		};
		JsonValue target = JsonValue(
			JsonArray{ 
				JsonNode("",JsonValue(1.00)),
				JsonNode("",JsonValue(JsonArray{
												JsonNode("",JsonValue(String("abc"))),
												JsonNode("",JsonValue(true)),
												},true)
						),
			},true);
		target.getType();
		testArray("[  1.00 , [\"abc\", true  ]   ]", target);


		JsonValue last = JsonValue(JsonArray{
							JsonNode("",JsonValue(2.0)),
							JsonNode("",JsonValue(String("def"))),
							JsonNode("",JsonValue(false))
			},true);
		JsonValue target2 = JsonValue(
			JsonArray{
				JsonNode("",JsonValue(1.00)),
				JsonNode("",JsonValue(JsonArray{
												JsonNode("",JsonValue(String("abc"))),
												JsonNode("",JsonValue(true)),
												JsonNode("",std::move(last))
												},true)
						),
			}, true);
		target2.getType();
		testArray("[  1.00 , [\"abc\", true,[2.00 , \"def\", false]  ]   ]", target2);
	}

	void __testObjects()
	{
		JsonValue v;
		auto testObj = [&](const std::string_view s, JsonValue& target)
		{
			eq(parseJson(s, v), PARSE_STATE::PARSE_OK);
			if (std::holds_alternative<JsonArray>(v.__value))
			{
				eq(v.eq(target), true);
			}
		};
		JsonValue n_value(std::monostate{});
		JsonValue f_value(false);
		JsonValue t_value(true);
		JsonValue i_value(123.0);
		JsonValue s_value(String("abc"));
		JsonValue a_value(JsonArray{ 
			JsonNode("",JsonValue(1.)),
			JsonNode("",JsonValue(2.)),
			JsonNode("",JsonValue(3.)) }, true);
		JsonValue o_value(JsonArray{
			JsonNode("1",JsonValue(1.)),
			JsonNode("2",JsonValue(2.)),
			JsonNode("3",JsonValue(3.))
			}, false);
		JsonArray nodes;
		nodes.insert(JsonNode("n",n_value));
		nodes.insert(JsonNode("f", f_value));
		nodes.insert(JsonNode("t",t_value));
		nodes.insert(JsonNode("i", i_value));
		nodes.insert(JsonNode("s", s_value));
		nodes.insert(JsonNode("a", a_value));
		nodes.insert(JsonNode("o", o_value));
		JsonValue target(std::move(nodes), false);
		target.getType();
		testObj(
			" { "
			"\"n\" : null , "
			"\"f\" : false , "
			"\"t\" : true , "
			"\"i\" : 123 , "
			"\"s\" : \"abc\", "
			"\"a\" : [ 1, 2, 3 ],"
			"\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
			" } ",
			target
		);
	}

public:
	void test()
	{
		__testBoolAndNull();
		__testNums();
		__testInvalid();
		__testStrings();
		__testArrays();
		__testObjects();
		std::cout << "passed tests:" << pass_num << std::endl;
		std::cout << "total tests:" << test_num << std::endl;
		if (pass_num == test_num)
			std::cout << "passed all tests" << std::endl;
	}
};

#include<array>
class A :public CanJson
{
private:
	int a = 0;
	int b = 0;
	long c = 0;
	String str = "";
	std::vector<std::vector<int>> nums;
	std::array<long, 2> lnums{};
	bool flag = false;
public:
	A()
	{
		a = 1; b = 2; str = "abcde"; nums = { {1,2,3},{4,5,6},{7,8,9} }; lnums = { 10000,20000 }, flag = true;
	}
	A(const Json& js)
	{
		a = js["a"];
		b = js["b"];
		c = js["c"];
		str = String(js["str"]);
		auto [num_begin, num_end] = js["nums"].iterArrayValue();
		for (int i = 0; num_begin != num_end; i++, num_begin++)
		{
			auto [sub_num_begin, sub_num_end] = num_begin->iterArrayValue();
			std::vector<int> sub;
			for (int j = 0; sub_num_begin != sub_num_end; i++, sub_num_begin++)
			{
				sub.push_back(*sub_num_begin);
			}
			nums.push_back(std::move(sub));
		}
		auto [arr_begin, arr_end] = js["lnums"].iterArrayValue();
		for (int i = 0; arr_begin != arr_end; i++, arr_begin++)
		{
			lnums[i] = *arr_begin;
		}
		flag = js["flag"];
	}
	Json toJson()const  override {
		auto a_v = genValue(a);
		auto b_v = genValue(b);
		auto c_v = genValue(c);
		auto str_v = genValue(str);
		auto nums_v = genValue(nums);
		auto lnums_v = genValue(lnums);
		auto flag_v = genValue<bool>(flag);
		JsonArray js;
		insertJsonArray(js, "a", a_v);
		insertJsonArray(js, "b", b_v);
		insertJsonArray(js, "c", c_v);
		insertJsonArray(js, "str", str_v);
		insertJsonArray(js, "nums", nums_v);
		insertJsonArray(js, "lnums", lnums_v);
		insertJsonArray(js, "flag", flag_v);
		return Json(std::move(js));
	};

	bool operator==(const A& b)const
	{
		if (a != b.a || this->b != b.b || c != b.c || str != b.str||flag!=b.flag|| lnums != b.lnums||nums!=b.nums)
			return false;
		return true;
	}

};

int main()
{
	ParseTest t;
	t.test();

	A a;
	Json a_js(a);
	std::cout << "save json looks like:" << std::endl;
	std::cout << a_js.toString() << std::endl;
	a_js.save("test.json");

	Json b_js;
	b_js.read("test.json");
	std::cout << "read json looks like:" << std::endl;
	std::cout << b_js.toString() << std::endl;
	A b(b_js);
	

	std::cout << "start cmp json a and json b" << std::endl;
	auto a_it = a_js.jsons.begin();
	auto b_it = b_js.jsons.begin();
	bool eq = true;
	while (a_it != a_js.jsons.end())
	{

		if (!a_it->valueEq(*b_it))
		{
			eq = false;
			break;
		}
		a_it++;
		b_it++;
	}
	if (eq)
	{
		std::cout << "cmp reuslt :json a == json b" << std::endl;
	}
	else
	{
		std::cout << "cmp reuslt :json a != json b" << std::endl;
	}
	std::cout << "start cmp class a and class b" << std::endl;
	if (a == b)
		std::cout << "cmp result : a == b" << std::endl;
	else
		std::cout << "cmp reuslt : a!=b" << std::endl;
}