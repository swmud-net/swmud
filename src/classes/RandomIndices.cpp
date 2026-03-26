#include "../mud.h"
#include "RandomIndices.h"

MOB_INDEX_DATA *RandomIndices::getMobIndex()
{

	int count = 0;

	for( int key = 0; key < MAX_KEY_HASH; key++ )
		for( [[maybe_unused]] auto* pMobIndex : mob_index_hash[key] )
			count++;

	int rnd = number_range( 0, count );
	count = 0;

	for( int key = 0; key < MAX_KEY_HASH; key++ )
		for( auto* pMobIndex : mob_index_hash[key] )
			if( count++ == rnd )
				return pMobIndex;


//	bug( "RandomIndices::getMobIndex: random mob not found" );
	return NULL;
}

ROOM_INDEX_DATA *RandomIndices::getRoomIndex()
{
	int count = 0;

	for( int key = 0; key < MAX_KEY_HASH; key++ )
		for( [[maybe_unused]] auto* pRoomIndex : room_index_hash[key] )
			count++;

	int rnd = number_range( 0, count );

	for( int key = 0, count = 0; key < MAX_KEY_HASH; key++ )
		for( auto* pRoomIndex : room_index_hash[key] )
			if( count++ == rnd )
				return pRoomIndex;


	return NULL;
}

OBJ_INDEX_DATA *RandomIndices::getObjIndex()
{
	int count = 0;

	for( int key = 0; key < MAX_KEY_HASH; key++ )
		for( [[maybe_unused]] auto* pObjIndex : obj_index_hash[key] )
			count++;

	int rnd = number_range( 0, count );

	for( int key = 0, count = 0; key < MAX_KEY_HASH; key++ )
		for( auto* pObjIndex : obj_index_hash[key] )
			if( count++ == rnd )
				return pObjIndex;


	return NULL;
}
