#pragma once

#include "afxdialogex.h"

#include "CommonStrEdits.h"

#include <functional>
#include <mutex>
#include <string>

class AdString;

class NameEditChecker
{
public:
	using NameCheckFunc = std::function<std::string(const std::string&)>;

	static void CheckAndEditName(CEdit& nameEdit, CStatic& nameWarning);
	static void SetNameCheckFunc(NameCheckFunc nameCheckFuncInp);
private:
	static void RemoveRestrictedSymbs(AdString& str);

	static inline std::string restrictedSymbs = "{}* ";
	static inline NameCheckFunc nameCheckFunc = nullptr;
};
