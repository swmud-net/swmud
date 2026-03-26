/*
 * SWFake.h
 *
 *  Created on: Aug 10, 2010
 *      Author: Trog
 */

#ifndef SWFAKE_H_
#define SWFAKE_H_
#include "../SWDbEntity.h"
#include "SWPlayerCharacter.h"
#include <vector>

using namespace std;

namespace player
{

class SWFake: public SWDbEntity
{
	SWString name;
	vector<SWString> inflect;
	SWString title;
	SWString longDesc;
	SWString desc;

	SWDbEntityDataMapping getDataMapping() const;

public:
	SWFake() :
		inflect(vector<SWString> (SWPlayerCharacter::inflectMaxElements))
	{
	}

	const SWString getTableName() const
	{
		return "fake_pchar";
	}

	const SWString &getDesc() const
	{
		return longDesc;
	}

	const vector<SWString> &getInflect() const
	{
		return inflect;
	}

	const SWString &getLong() const
	{
		return longDesc;
	}

	const SWString &getName() const
	{
		return name;
	}

	const SWString &getTitle() const
	{
		return title;
	}

	void setDesc(const SWString &Desc)
	{
		this->desc = Desc;
	}

	void setInflect(const vector<SWString> &inflect)
	{
		this->inflect = inflect;
	}

	void setLong(const SWString &longDesc)
	{
		this->longDesc = longDesc;
	}

	void setName(const SWString &name)
	{
		this->name = name;
	}

	void setTitle(const SWString &title)
	{
		this->title = title;
	}

	void setInflect(const SWInt &no, const SWString &inflect)
	{
		this->inflect[no] = inflect;
	}
};

}

#endif /* SWFAKE_H_ */
