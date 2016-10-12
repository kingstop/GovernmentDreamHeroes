#pragma once
/*  
#include "message/MoleMap.pb.h"
#include "common_type.h"
#include "database.h"

//class DBQuery;
struct tile_group
{
	int min_x_;
	int min_y_;
	int max_x_;
	int max_y_;
};

struct mole_construction_begin
{
	int congfig_id;
	int pos_x;
	int pos_y;
	int level;
};


class mapstorage
{
public:
	typedef std::map<int, tile_group> MapTileGroup;
	typedef std::map<int, const message::TileGroupProto*> MapSourceTileGroup;

public:
	mapstorage(void);
	virtual ~mapstorage(void);
	void init_from_wolrd_db(DBQuery* p);
	const MapTileGroup& getUnLockGroup();
	u32 get_area_flag(int pos_x, int pos_y);
	const std::vector<mole_construction_begin>& get_begin_constructions();
	const std::map<int, int>& get_begin_resource();
protected:
	message::TileMapProto  _TileMap;
	MapTileGroup _unlock_group;
	MapTileGroup _lock_group;
	MapSourceTileGroup _source_group;
	u32 _mole_farm_config[150][150];
	std::vector<mole_construction_begin> _begin_constructions;
	std::map<int, int> _begin_resource;

};
*/
