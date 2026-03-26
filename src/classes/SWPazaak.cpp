/*
 * SWPazaak.cpp
 *
 *  Created on: Feb 21, 2010
 *      Author: Trog
 */

#include "SWPazaak.h"
#include "SWDataBase.h"
#include "SWTcpSender.h"

list<SWPazaak::SWPazaakChallenge*> SWPazaak::challenges;
list<SWPazaakGame*> SWPazaak::games;

bool SWPazaak::SWPazaakChallenge::DefenderComparator::operator()(const SWPazaakChallenge *first,
		const SWPazaakChallenge *second) const
{
	if (defender == first->getDefender())
	{
		if (defender == second->getDefender())
		{
			return *first < *second;
		}

		return true;
	}

	return false;
}

bool SWPazaak::SWPazaakChallenge::operator<(const SWPazaakChallenge &challenge) const
{
	if (challenge.challenged < challenged)
	{
		return true;
	}

	if (challenge.challenged == challenged)
	{
		if (reward > challenge.reward)
		{
			return true;
		}

		if (reward == challenge.reward)
		{
			int cmp = aggressor.compare(challenge.aggressor);
			if (cmp < 0)
			{
				return true;
			}

			return defender.compare(challenge.defender) <= 0;
		}

		return false;
	}

	return false;
}

int SWPazaak::getPlayerId(SWDataBase &db, const SWString &playerName)
{
	SWString query("SELECT id FROM pazaak_player WHERE LOWER(name) = LOWER('");
	query << playerName << "') LIMIT 1";
	db.singleQuery(query);
	list<SWString> columns;
	columns.push_back("id");
	SWQueryResult *result = db.getResult(columns);

	if (!result || result->size() < 1)
	{
		return 0;
	}

	return SWInt::fromString((*result)[0]["id"]);
}

int SWPazaak::getCardId(SWDataBase &db, const SWPazaakCard &card, int playerId)
{
	SWString query("SELECT id FROM pazaak_card WHERE player_id=");
	query << playerId << " AND type=" << card.getType() << " AND sign=" << card.getSign() << " AND double_signed="
			<< (int) card.isDoubleSigned() << " ORDER BY id OFFSET 0 LIMIT 1";
	db.singleQuery(query);
	list<SWString> columns;
	columns.push_back("id");
	SWQueryResult *result = db.getResult(columns);

	if (!result || result->size() < 1)
	{
		return 0;
	}

	return SWInt::fromString((*result)[0]["id"]);
}

void SWPazaak::clearGames()
{
	for (list<SWPazaakGame*>::iterator it = games.begin(); it != games.end(); it = games.erase(it))
	{
		delete *it;
	}
}

void SWPazaak::clearChallenges()
{
	for (list<SWPazaakChallenge*>::iterator it = challenges.begin(); it != challenges.end(); it = challenges.erase(it))
	{
		delete *it;
	}
}

bool SWPazaak::addCard(const SWPazaakCard &card, const SWString &playerName, SWDataBase *db)
{
	bool deleteDb = false;
	if (!db)
	{
		db = new SWDataBase;
		deleteDb = true;
	}

	int playerId = getPlayerId(*db, playerName);
	if (playerId < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return false;
	}

	SWString query("INSERT INTO pazaak_card(player_id,type,sign,double_signed) VALUES(");
	query << playerId << "," << card.getType() << "," << card.getSign() << "," << (int) card.isDoubleSigned() << ")";

	bool ok = db->singleQuery(query);
	if (deleteDb)
	{
		delete db;
	}

	return ok;
}

bool SWPazaak::removeCard(const SWPazaakCard &card, const SWString &playerName, SWDataBase *db)
{
	bool deleteDb = false;
	if (!db)
	{
		db = new SWDataBase;
		deleteDb = true;
	}

	int playerId = getPlayerId(*db, playerName);
	if (playerId < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return false;
	}

	int cardId = getCardId(*db, card, playerId);
	if (cardId < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return false;
	}

	SWString query("DELETE FROM pazaak_card WHERE id=");
	query << cardId;

	bool ok = db->singleQuery(query);
	if (deleteDb)
	{
		delete db;
	}

	return ok;
}

void SWPazaak::loadGames(SWDataBase *db)
{
	bool deleteDb = false;
	if (!db)
	{
		db = new SWDataBase;
		deleteDb = true;
	}

	SWString query("SELECT ");
	query << "(SELECT p.name FROM pazaak_player p WHERE p.id=aggressor_id) AS aggressor, ";
	query << "(SELECT p.name FROM pazaak_player p WHERE p.id=defender_id) AS defender, ";
	query << "id, reward, CAST(EXTRACT(EPOCH FROM challenged) AS int) AS challenged FROM pazaak_game ";
	query << "WHERE winner_id IS NULL ORDER BY challenged DESC, reward DESC, aggressor, defender, id DESC";
	db->singleQuery(query);
	list<SWString> columns;
	columns.push_back("id");
	columns.push_back("aggressor");
	columns.push_back("defender");
	columns.push_back("reward");
	columns.push_back("challenged");
	SWQueryResult *result = db->getResult(columns);

	if (!result || result->size() < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return;
	}

	clearGames();
	for (SWQueryResult::size_type i = 0; i < result->size(); i++)
	{
		SWPazaakGame *game = new SWPazaakGame(SWInt::fromString((*result)[i]["id"]), (*result)[i]["aggressor"],
				(*result)[i]["defender"], SWInt::fromString((*result)[i]["reward"]), SWInt::fromString(
						(*result)[i]["challenged"]));
		games.push_back(game);
	}

	if (deleteDb)
	{
		delete db;
	}

	/* don't sort games, this was already done by the db */
}

bool SWPazaak::setPassword(const SWString &playerName, const SWString &password)
{
	if (password.find('\'') != SWString::npos || password.find("--") != SWString::npos)
	{ /* lamerskie sprawdzanie SQL-injection, ale poki co wystarczy */
		return false;
	}
	//TODO: zabezpieczyc password przed SQL-injection
	SWString query("SELECT pazaak_add_player('");
	query << playerName << "','" << password << "') AS pid";
	SWDataBase db;
	db.singleQuery(query);
	list<SWString> columns;
	columns.push_back("pid");
	SWQueryResult *result = db.getResult(columns);

	return result && result->size() > 0 && SWInt::fromString((*result)[0]["pid"]) > 0;
}

int SWPazaak::withdrawCredits(const SWString &playerName)
{
	SWString query("SELECT pazaak_withdraw_credits('");
	query << playerName << "') AS credits";
	SWDataBase db;
	db.singleQuery(query);
	list<SWString> columns;
	columns.push_back("credits");
	SWQueryResult *result = db.getResult(columns);

	if (!result || result->size() < 1)
	{
		return 0;
	}

	return SWInt::fromString((*result)[0]["credits"]);
}

SWPazaak::SWPazaakChallenge *SWPazaak::findChallenge(const SWString &aggressor, const SWString &defender)
{
	for (list<SWPazaakChallenge*>::const_iterator it = challenges.begin(); it != challenges.end(); it++)
	{
		if ((*it)->getAggressor() == aggressor && (*it)->getDefender() == defender)
		{
			return *it;
		}
	}

	return 0;
}

unsigned int SWPazaak::countCards(SWDataBase &db, int playerId)
{
	SWString query("SELECT count(*) AS amt FROM pazaak_card WHERE player_id=");
	query << playerId;
	db.singleQuery(query);
	list<SWString> columns;
	columns.push_back("amt");
	SWQueryResult *result = db.getResult(columns);

	if (!result || result->size() < 1)
	{
		return 0;
	}

	return SWInt::fromString((*result)[0]["amt"]);
}

bool SWPazaak::challenge(const SWString &aggressor, const SWString &defender, unsigned int reward)
{
	if (findChallenge(aggressor, defender))
	{
		return false;
	}

	SWDataBase db;

	int aggressorId = getPlayerId(db, aggressor);
	if (aggressorId < 1)
	{
		return false;
	}

	int defenderId = getPlayerId(db, defender);
	if (defenderId < 1)
	{
		return false;
	}

	if (countCards(db, aggressorId) < MIN_CARDS_IN_SIDEDECK || countCards(db, defenderId) < MIN_CARDS_IN_SIDEDECK)
	{
		return false;
	}

	if (reward < MIN_REWARD)
	{
		return false;
	}

	challenges.push_back(new SWPazaakChallenge(aggressor, defender, reward));
	return true;
}

bool SWPazaak::acceptChallenge(SWPazaakChallenge *challenge)
{
	if (!challenge)
	{
		return false;
	}

	SWDataBase db;

	int aggressorId = getPlayerId(db, challenge->getAggressor());
	if (aggressorId < 1)
	{
		return false;
	}

	int defenderId = getPlayerId(db, challenge->getDefender());
	if (defenderId < 1)
	{
		return false;
	}

	if (countCards(db, aggressorId) < MIN_CARDS_IN_SIDEDECK || countCards(db, defenderId) < MIN_CARDS_IN_SIDEDECK)
	{
		return false;
	}

	SWString query("INSERT INTO pazaak_game(aggressor_id,defender_id,challenged,reward) VALUES(");
	query << aggressorId << "," << defenderId << ",'" << challenge->getChallenged() << "'," << challenge->getReward()
			<< ")";

	bool ok = db.singleQuery(query);
	if (ok)
	{
		challenges.remove(challenge);
		delete challenge;
		loadGames(&db);
		SWTcpSender *sender = new SWTcpSender("127.0.0.1", 4010, "<request><reload><games/></reload></request>");
		sender->send();
	}

	return ok;
}

list<SWPazaakCard> *SWPazaak::getCards(const SWString &playerName, SWDataBase *db)
{
	bool deleteDb = false;
	if (!db)
	{
		db = new SWDataBase;
		deleteDb = true;
	}

	int playerId = getPlayerId(*db, playerName);
	if (playerId < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return 0;
	}

	SWString query("SELECT type,sign,double_signed FROM pazaak_card WHERE player_id=");
	query << playerId << " ORDER BY id";
	db->singleQuery(query);
	list<SWString> columns;
	columns.push_back("type");
	columns.push_back("sign");
	columns.push_back("double_signed");
	SWQueryResult *result = db->getResult(columns);

	if (!result || result->size() < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return 0;
	}

	list<SWPazaakCard> *cards = new list<SWPazaakCard> ;

	for (SWQueryResult::size_type i = 0; i < result->size(); i++)
	{
		SWPazaakCard card(static_cast<SWPazaakCard::Type> (SWInt::fromString((*result)[i]["type"])),
				static_cast<SWPazaakCard::Sign> (SWInt::fromString((*result)[i]["sign"])), SWPazaakCard::V_FIRST,
				static_cast<bool> (SWInt::fromString((*result)[i]["double_signed"])));

		cards->push_back(card);
	}

	if (deleteDb)
	{
		delete db;
	}

	return cards;
}

const SWString SWPazaak::listCards(const SWString &playerName)
{
	list<SWPazaakCard> *cards = SWPazaak::getCards(playerName);
	if (!cards)
	{
		return SWString("Nie posiadasz \xbf" "adnych kart w talii pomocniczej." NL);
	}

	int i = 1;
	char buf[MIL] =
	{ 0 };
	SWString buf2("Posiadasz nast\xeapuj\xb1" "ce karty, w talii pomocniczej:" NL);
	for (list<SWPazaakCard>::const_iterator it = cards->begin(); it != cards->end(); it++, i++)
	{
		snprintf(buf, MIL - 1, " %2d: %s" NL, i, it->description().c_str());
		buf2 << buf;
	}

	delete cards;
	return buf2;
}

SWPazaakCard *SWPazaak::getPlayerCard(const SWString &playerName, const SWInt &cardNo, SWDataBase *db)
{
	bool deleteDb = false;
	if (!db)
	{
		db = new SWDataBase;
		deleteDb = true;
	}

	int playerId = getPlayerId(*db, playerName);
	if (playerId < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return 0;
	}

	SWInt cNo = (cardNo > 0) ? SWInt(cardNo-1) : cardNo;

	SWString query("SELECT id,sign,type,double_signed FROM pazaak_card WHERE player_id=");
	query << playerId << " ORDER BY id OFFSET " << cNo << " LIMIT 1";
	db->singleQuery(query);

	list<SWString> columns;
	columns.push_back("id");
	columns.push_back("sign");
	columns.push_back("type");
	columns.push_back("double_signed");
	SWQueryResult *result = db->getResult(columns);


	if (!result || result->size() < 1)
	{
		if (deleteDb)
		{
			delete db;
		}
		return 0;
	}

	SWPazaakCard *card = SWPazaakCard::newInstance(SWInt::fromString((*result)[0]["type"]), SWInt::fromString(
			(*result)[0]["sign"]), SWPazaakCard::V_FIRST, SWInt::fromString((*result)[0]["double_signed"]));

	if (deleteDb)
	{
		delete db;
	}

	return card;
}

