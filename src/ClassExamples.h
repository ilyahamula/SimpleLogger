#pragma once

#include <string>

class A
{
	friend std::string typeToString(const A& obj);
public:
	void doSomething();

private:
	int m_value = 21;
	double m_precision = 0.003;
};

class B
{
	friend std::string typeToString(const B& obj);
public:
	void performOperation();

private:
	A m_subObj;
	long m_uuid = static_cast<long>(434534683);
};

std::string typeToString(const A& obj);
std::string typeToString(const B& obj);
