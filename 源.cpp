//int main()
//{
//	JsonValue last_value = JsonValue(JsonArray{ JsonNode("d",JsonValue(String("e"))) });
//
//	JsonNode lastnode = JsonNode("", JsonValue(JsonArray{ JsonNode("c",JsonValue(JsonArray{JsonNode("",last_value)},true)) }));//"c":[{"d":"e"}]
//	JsonValue last_arrat(JsonArray{
//			JsonNode("",JsonValue(1.0)),
//			JsonNode("",JsonValue(String("abc"))),
//			lastnode
//		}, true);
//
//	JsonValue d_e = JsonValue(JsonArray{ JsonNode("d",JsonValue(String("e"))) });
//	JsonValue c_d_e = JsonValue(JsonArray{ JsonNode("c",JsonValue(JsonArray{JsonNode("",d_e)},true)) });
//
//	JsonArray e_array = JsonArray{
//				JsonNode("",JsonValue{1.0}),
//				JsonNode("",JsonValue{String{"abc"}}),
//				JsonNode("",JsonValue{JsonArray{JsonNode{"a",JsonValue{String("b")}}}}),
//				JsonNode("",c_d_e),
//				JsonNode("",last_arrat)
//	};
//
//	JsonNode e = JsonNode("e", JsonValue(std::move(e_array),true));
//	JsonNode d = JsonNode("d", JsonValue(JsonArray{std::move(e)}));
//	JsonNode c = JsonNode("c", JsonValue(JsonArray{ JsonNode(std::move(d)) }));
//	JsonNode b = JsonNode("b", JsonValue(JsonArray{ JsonNode(std::move(c)) }));
//	JsonNode a = JsonNode("a", JsonValue(JsonArray{ JsonNode(std::move(b)) }));
//	Json j;
//	j.addNode(a);
//	std::cout << j.toString() << std::endl;
//	return 0;
//}