/*
 * SWDataBase.cpp
 *
 *  Created on: 2009-09-11
 *      Author: Trog
 */

#include "SWDataBase.h"
#include "../defines.h"
#include "../mud.h"
#include "../swpqxx.h"

SWDataBase::SWDataBase(bool connectOnCreation) :
	connectionString(SQL_CONN_STRING), dbConnection(0), dbTx(0)
{
	if (connectOnCreation)
	{
		connect();
	}
}

SWDataBase::SWDataBase(const SWString &connectionString, bool connectOnCreation) :
	connectionString(connectionString), dbConnection(0), dbTx(0)
{
	if (connectOnCreation)
	{
		connect();
	}
}

SWDataBase::~SWDataBase()
{
	if (dbTx)
	{
		delete dbTx;
	}
	if (dbConnection)
	{
		delete dbConnection;
	}
	for (list<SWQueryResult *>::iterator it = results.begin(); it != results.end(); it = results.erase(it))
	{
		delete *it;
	}
}

bool SWDataBase::connect()
{
	try
	{
		dbConnection = new connection(connectionString);
	} catch (const broken_connection &e)
	{
		if (dbConnection)
		{
			delete dbConnection;
			dbConnection = 0;
		}
		bug( "cannot connect to the database, message: %s", e.what() );
		return false;
	}
	return true;
}

bool SWDataBase::isConnected() const
{
	return dbConnection != 0 && dbConnection->is_open();
}

void SWDataBase::beginTransaction(const SWString &txId)
{
	if (!isConnected())
	{
		return;
	}

	if (dbTx)
	{
		delete dbTx;
	}

	dbTx = new work(*dbConnection, txId);
}

void SWDataBase::commitTransaction()
{
	if (!dbTx)
	{
		return;
	}

	try
	{
		dbTx->commit();
		delete dbTx;
		dbTx = 0;
	} catch (const exception &e)
	{
		bug( "transaction commit failed: %s", e.what() );
	}
}

bool SWDataBase::singleQuery(const SWString &query)
{
	if (!isConnected())
	{
		return false;
	}

	try
	{
		beginTransaction();
		queryResult = dbTx->exec(query.c_str());
		commitTransaction();
	} catch (const exception &e)
	{
		bug( "exception thrown during singleQuery: %s", e.what() );
		return false;
	}
	return true;
}

bool SWDataBase::query(const SWString &query)
{
	if (!isConnected())
	{
		return false;
	}

	try
	{
		queryResult = dbTx->exec(query);
	} catch (const exception &e)
	{
		bug( "exception thrown during query: %s", e.what() );
		return false;
	}
	return true;
}

SWQueryResult *SWDataBase::getResult(const list<SWString> &columns)
{
	SWQueryResult *qResult = new SWQueryResult(this);

	for (result::const_iterator rIt = queryResult.begin(); rIt != queryResult.end(); rIt++)
	{
		SWQueryResult::SWQueryResultMap row;
		for (list<SWString>::const_iterator it = columns.begin(); it != columns.end(); it++)
		{
			try
			{
				(void)queryResult.column_number(*it);
			} catch (const exception &e)
			{
				bug( "exception thrown during getResult, column name: %s, message: %s", it->c_str(), e.what() );
				qResult->dispose();
				return 0;
			}
			row.add(*it, (*rIt)[*it].as(SWString()));
		}
		qResult->add(row);
	}

	if (qResult->size() < 1)
	{
		qResult->dispose();
		qResult = 0;
	}

	return qResult;
}

