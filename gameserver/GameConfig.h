#pragma once
#include <vector>
#include <map>

struct TaskConfig
{
	int task_id_;
	message::TaskType en_;
	int argument_;
	int argument_1_;
	int need_time_;
	int require_chapter_;
	int require_section_;
	int require_complete_task_count_;
	int gift_gold_;
};


struct globalConfig
{
	int config_gold_;
	int hero_unlock_count_;
	int day_Refresh_time_;
	int day_free_task_count_;
	int day_task_advertisement_task_cd_;
	int hero_max_tasks_count_;
};
struct ShopSalesPromotionConfig
{
	int id_;
};

struct TimeShopSalesPromotionConfig : public ShopSalesPromotionConfig
{
	int grid_id_;
	int cheap_gold_;
	s64 begin_time_;
	s64 end_time_;
};



struct ObjDropBoxConfig
{
	int obj_id_;
	message::SubType type_;
	int base_gold_;
	int random_gold_;
};

struct IntPair
{
	int number_1_;
	int number_2_;
};
struct MapBehaviorConfig
{
	std::pair<int, int> key_;
	int obj_behavior_id_;
	int obj_count_;
	int max_pos_count_;

};


struct MapRandomObjConfig
{
	message::SubType type_;
	int obj_id_;
};

struct MapRandomObjsConfig
{
	std::pair<int, int> key_;
	int obj_behavior_id_;
	std::vector<MapRandomObjConfig> objs_;
};

typedef std::map<int, message::MsgTaskConfigInfo> MAPTASKS;
typedef std::map<int, message::MsgShopConfigInfo> MAPSHOPHEROCONFIGS;
typedef std::map<int, message::MsgChapterConfigInfo> MAPCHAPTERCONFIGINFOS;
typedef std::map<int, message::MsgGoldShopConfigInfo> MAPGOLDSHOPCONFIGINFOS;
typedef std::map<int, TimeShopSalesPromotionConfig> MAPTIMESHOPSALESPROMOTIONCONFIGS;
typedef std::map<int, ObjDropBoxConfig> MAPDROPBOXCONFIGS;
typedef std::map<message::SubType, MAPDROPBOXCONFIGS> MAPTYPEDROPBOXCONFIGS;
typedef std::map<int, MapBehaviorConfig> MAPBEHAVIORCONFIGS;
typedef std::map<std::pair<int, int>, MAPBEHAVIORCONFIGS> MAPALLBEHAVIORCONFIGS;
typedef std::map<int, MapRandomObjsConfig> MAPRANDOMOBJCONFIGS;
typedef std::map<std::pair<int, int>, MAPRANDOMOBJCONFIGS> MAPALLRANDOMOBJCONFIGS;
typedef std::map<std::pair<int, int>, MAPTYPEDROPBOXCONFIGS> MAPMAPDROPBOXCONFIGS;


class GameConfig
{
public:
	GameConfig();
	virtual ~GameConfig();
	void Load(DBQuery* p);
	const MAPSHOPHEROCONFIGS* getShopConfigs();
	const message::MsgShopConfigInfo* getShopConfig(int  grid_id);
	const MAPTASKS* getMapTasks();
	const message::MsgTaskConfigInfo* getMapTask(int id);
	const MAPCHAPTERCONFIGINFOS* getChapterConfigInfos();
	const MAPTIMESHOPSALESPROMOTIONCONFIGS* getTimeShopSalesPromotionConfigs();
	const TimeShopSalesPromotionConfig* getTimeShopSalesPromotionConfig(int id);
	const  message::MsgChapterConfigInfo* getChapterConfigInfo(int id);
	const MAPGOLDSHOPCONFIGINFOS* getGoldShopConfigInfos();
	const message::MsgGoldShopConfigInfo* getGoldShopConfigInfo(int id);


	const globalConfig& getGlobalConfig();
	const MAPTYPEDROPBOXCONFIGS* getMapDropBox(int chapter_id, int section_id);
protected:
	MAPTASKS _tasks;
	MAPSHOPHEROCONFIGS _shop_heroes;
	MAPTIMESHOPSALESPROMOTIONCONFIGS _shop_time_sales_promotion;
	MAPCHAPTERCONFIGINFOS _chapter_config_infos;
	MAPGOLDSHOPCONFIGINFOS _gold_shop_config_infos;
	MAPTYPEDROPBOXCONFIGS _drop_box_configs;
	MAPALLBEHAVIORCONFIGS _map_behavior_config;
	MAPALLRANDOMOBJCONFIGS _map_random_obj_configs;

	MAPMAPDROPBOXCONFIGS _map_drop_box_configs;
	globalConfig _global_config;
};

