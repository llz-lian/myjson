#pragma once
#include<iostream>
#include"Node.h"
inline std::ostream & operator <<(std::ostream & o, const JsonNode & jn)
{
	o << jn.__value.toString();
	return o;
}