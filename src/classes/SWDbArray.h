/*
 * SWDbArray.h
 *
 *  Created on: 2010-08-21
 *      Author: Trog
 */

#ifndef SWDBARRAY_H_
#define SWDBARRAY_H_
#include "SWString.h"
#include "SWInt.h"
#include "SWInt64.h"
#include <vector>

/**
 * Zbior narzedzi do obslugi typu tablicowego SQL.
 */
class SWDbArray
{
public:
	template<class T>
	static SWString toQueryValue(const T &v, const SWString &quote = "");

	template<class T, template<class , class A = std::allocator<T> > class V>
	static V<T> toArray(const SWString &array, const SWString &quote = "");
};

/*!
 * Konwertuje wektor wartosci na napis reprezentujacy typ tablicowy SQL.
 *
 * \param v wektor wartosci - elementow tablicy
 * \param quote znak, ktorym opakowana bedzie kazda wartosc w tablicy
 * \return napis reprezentujacy typ tablicowy SQL
 *
 * Zwrocona wartosc bedzie opakowana z zewnatrz apostrofami, czyli
 * bedzie od razu gotowa do umieszcenia w zapytaniu SQL.
 */
template<class T>
SWString SWDbArray::toQueryValue(const T &v, const SWString &quote)
{
	SWString queryValue("{");
	typename T::const_iterator it = v.begin();
	if (it != v.end())
	{
		queryValue << quote << *it << quote;
		for (++it; it != v.end(); ++it)
		{
			queryValue << "," << quote << *it << quote;
		}
	}
	queryValue << "}";

	return queryValue;
}

/*!
 * Konwertuje napis zawierajacy typ tablicowy SQL na wektor wartosci.
 *
 * \param array napis reprezentujacy typ tablicowy SQL
 * \param quote znak, ktorym opakowana jest kazda wartosc w tablicy
 * \return wektor wartosci - elementow tablicy
 *
 * W przypadku niepoprawnych parametrow wejsciowych, wartosc zwracana jest
 * niezdefiniowana.
 */
template<class T, template<class , class A = std::allocator<T> > class V>
V<T> SWDbArray::toArray(const SWString &array, const SWString &quote)
{
	SWString a = array;
	int qs = quote.size();

	a.trim();
	a.erase(0, 1);
	a.erase(a.end() - 1, a.end());
	vector<SWString> splitted = a.split(",");
	V<T> v(splitted.size());
	int i = 0;
	for (vector<SWString>::iterator it = splitted.begin(); it != splitted.end(); ++it)
	{
		SWString &e = *it;
		e.trim();
		e.erase(e.begin(), e.begin() + qs);
		e.erase(e.begin() + e.size() - qs, e.end());
		v[i++] = T::fromString(e);
	}

	return v;
}

#endif /* SWDBARRAY_H_ */
