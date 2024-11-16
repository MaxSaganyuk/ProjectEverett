#pragma once

#include <string>
#include <vector>

#include "LGL.h"

namespace LGLUtils
{
	template<typename Type>
	bool _SetShaderUniformStructImpl(LGL& lgl, const std::string& structName, const std::vector<std::string>& valueNames, size_t i, Type value)
	{
		return lgl.SetShaderUniformValue(structName + '.' + valueNames[i], value);
	}

	template<typename Type, typename... Types>
	bool _SetShaderUniformStructImpl(LGL& lgl, const std::string& structName, const std::vector<std::string>& valueNames, size_t i, Type value, Types... values)
	{
		bool res = lgl.SetShaderUniformValue(structName + '.' + valueNames[i], value);

		// Can't decide if I should stop processing the whole structure if one of the elements failed.
		// return SetShaderUniformStruct(structName, std::move(valueNames), values...) && res;
		return res && _SetShaderUniformStructImpl(lgl, structName, valueNames, ++i, values...);
	}

	template<typename Type>
	bool SetShaderUniformStruct(LGL& lgl, const std::string& structName, const std::vector<std::string>& valueNames, Type value)
	{
		return _SetShaderUniformValueImpl(lgl, structName, valueNames, 0, value);
	}

	template<typename Type, typename... Types>
	bool SetShaderUniformStruct(LGL& lgl, const std::string& structName, const std::vector<std::string>& valueNames, Type value, Types... values)
	{
		// In C++20 and after could probably be made as a compile time check
		if (valueNames.size() - 1 != sizeof...(Types))
		{
			assert(false && "Mismatch between amount of valueNames and values");
			return false;
		}

		return _SetShaderUniformStructImpl(lgl, structName, valueNames, 0, value, values...);
	}

}