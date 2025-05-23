#pragma once

#include <type_traits>
#include <unordered_map>

template<typename Type>
concept OnlyPointers = std::is_pointer_v<Type>;

template<OnlyPointers Type>
class PtrHasher
{
public:
	size_t operator()(const Type value) const noexcept
	{
		return std::hash<std::remove_const_t<std::remove_pointer_t<Type>>>{}(*value);
	}
};

template<OnlyPointers Type>
class PtrComparator
{
public:
	//using is_transparent = void; Not valid for unordered_map in C++14, sadly

	bool operator()(const Type left, const Type right) const
	{
		return *left == *right;
	}
};

template<typename KeyType, typename ValueType>
using UnorderedPtrMap = std::unordered_map<KeyType, ValueType, PtrHasher<KeyType>, PtrComparator<KeyType>>;