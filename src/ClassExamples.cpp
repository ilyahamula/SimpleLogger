#include "ClassExamples.h"

void A::doSomething()
{
}

void B::performOperation()
{
}

std::string typeToString(const A& obj)
{
	return "type A { m_value = " + std::to_string(obj.m_value)
		+ " m_precision = " + std::to_string(obj.m_precision) + "}";
}

std::string typeToString(const B& obj)
{
	return "type B { m_subObj = " + typeToString(obj.m_subObj) +
		" m_uuid = " + std::to_string(obj.m_uuid) + "}";
}
