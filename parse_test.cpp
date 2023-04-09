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
		auto testInvalid = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_INVALID_VALUE); };
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

		auto testInvalidStr = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_INVALID_STRING); };
		testInvalidStr("\"");
		testInvalidStr("\"aaaa");
		testInvalidStr("\"\\q\"");
		testInvalidStr("\"\0\"");
		testInvalidStr("\"\\x12\"");
		testInvalidStr("\"\x01\"");

		auto testInvalidArr = [&](const std::string_view s) {eq(parseJson(s, v), PARSE_INVALID_ARRAY); };
		testInvalidArr("[");
		testInvalidArr("[\"abc\",[false,]");
		testInvalidArr("[true,false,[]    ");
	}

	void __testBoolAndNull()
	{
		JsonValue v;
		
		eq(parseJson("true", v), PARSE_OK);
		if (std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), true);

		eq(parseJson("false", v), PARSE_OK);
		if (std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), false);

		eq(parseJson("null", v), PARSE_OK);
		if (std::holds_alternative<Null>(v.__value))
			eq((Null)std::get<Null>(v.__value), Null{});


		eq(parseJson("    true   ", v), PARSE_OK);
		if(std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value),true);

		eq(parseJson("   false   ", v), PARSE_OK);
		if(std::holds_alternative<bool>(v.__value))
			eq((bool)std::get<bool>(v.__value), false);

		eq(parseJson("   null   ", v), PARSE_OK);
		if (std::holds_alternative<Null>(v.__value))
			eq((Null)std::get<Null>(v.__value), Null{});


		eq(parseJson("", v), PARSE_NEED_VALUE);
		eq(parseJson("     ", v), PARSE_NEED_VALUE);

		eq(parseJson("false   x", v), PARSE_NEED_SINGULAR);
		eq(parseJson("  null   x", v), PARSE_NEED_SINGULAR);
		eq(parseJson("  true   x", v), PARSE_NEED_SINGULAR);
	}

	void __testNums()
	{
		JsonValue v;
		auto nums_should_ok = [&](const double num, const std::string_view & s) {
			eq(parseJson(s, v), PARSE_OK);
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
			eq(parseJson(s, v), PARSE_OK);
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
			eq(parseJson(s, v), PARSE_OK);
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
		std::cout << v.toString() << std::endl;


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
		std::cout << v.toString() << std::endl;
	}

public:
	void test()
	{
		__testBoolAndNull();
		__testNums();
		__testInvalid();
		__testStrings();
		__testArrays();
		std::cout << "passed tests:" << pass_num << std::endl;
		std::cout << "total tests:" << test_num << std::endl;
		if (pass_num == test_num)
			std::cout << "passed all tests" << std::endl;
	}
};

int main()
{
	ParseTest t;
	t.test();
}