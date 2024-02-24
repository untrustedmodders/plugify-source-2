#pragma once

#include <functional>
#include <string>
#include <vector>

#include <color.h>
#include <tier0/platform.h>

class LoggerScope {
	using This = LoggerScope;

public:
	using SendFunc = std::function<void (const char *)>;
	using SendColorFunc = std::function<void (const Color &, const char *)>;

	LoggerScope(const Color &rgbaInit, const char *pszStartWith = "", const char *pszEnd = "\n");

	LoggerScope &operator+=(const LoggerScope &);

	const Color &GetColor() const;
	const char *GetStartWith() const;
	const char *GetEnd() const;
	size_t Count() const;

	void SetColor(const Color &rgba);

	size_t Push(const char *pszContent);
	size_t Push(const Color &rgba, const char *pszContent);

	size_t PushFormat(const char *pszFormat, ...) FMTFUNCTION(2, 3);
	size_t PushFormat(const Color &rgba, const char *pszFormat, ...) FMTFUNCTION(3, 4);

	size_t Send(SendFunc funcOn);
	size_t SendColor(SendColorFunc funcOn);

	class Message
	{
	public:
		Message(const Color &rgbaInit, const char *pszContent = "");

		const Color &GetColor() const;
		const std::string &Get() const;
		size_t SetWithCopy(const char *pszContent);

	private:
		Color m_aColor;
		std::string m_sContent;
	}; // LoggerScope::Message

private:
	Color m_aColor;

	std::string m_aStartWith;
	std::vector<Message> m_vec;
	std::string m_aEnd;
}; // LoggerScope
