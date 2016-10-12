#include "stdafx.h"
#include "GameConfig.h"


GameConfig::GameConfig()
{
	
}


GameConfig::~GameConfig()
{
}


const message::MsgShopConfigInfo* GameConfig::getShopConfig(int  grid_id)
{
	message::MsgShopConfigInfo* shop_config = NULL;
	MAPSHOPHEROCONFIGS::iterator it = _shop_heroes.find(grid_id);
	if (it != _shop_heroes.end())
	{
		shop_config = &it->second;
	}
	return shop_config;
}

const MAPSHOPHEROCONFIGS* GameConfig::getShopConfigs()
{
	return &_shop_heroes;
}
const MAPTASKS* GameConfig::getMapTasks()
{
	return &_tasks;
}

const MAPGOLDSHOPCONFIGINFOS* GameConfig::getGoldShopConfigInfos()
{
	return &_gold_shop_config_infos;
}

const message::MsgGoldShopConfigInfo* GameConfig::getGoldShopConfigInfo(int id)
{
	message::MsgGoldShopConfigInfo* entry = NULL;
	MAPGOLDSHOPCONFIGINFOS::iterator it = _gold_shop_config_infos.find(id);
	if (it != _gold_shop_config_infos.end())
	{
		entry = &it->second;
	}
	return entry;
}


const  message::MsgChapterConfigInfo* GameConfig::getChapterConfigInfo(int id)
{
	message::MsgChapterConfigInfo* info_config = NULL;
	MAPCHAPTERCONFIGINFOS::iterator it = _chapter_config_infos.find(id);
	if (it != _chapter_config_infos.end())
	{
		info_config = &it->second;
	}
	return info_config;
}

const message::MsgTaskConfigInfo* GameConfig::getMapTask(int id)
{
	message::MsgTaskConfigInfo* info_temp = NULL;
	MAPTASKS::iterator it = _tasks.find(id);
	if (it != _tasks.end())
	{
		info_temp = &it->second;
	}
	return info_temp;

}

const MAPCHAPTERCONFIGINFOS* GameConfig::getChapterConfigInfos()
{
	return &_chapter_config_infos;
}

const MAPTIMESHOPSALESPROMOTIONCONFIGS* GameConfig::getTimeShopSalesPromotionConfigs()
{
	return &_shop_time_sales_promotion;
}


const TimeShopSalesPromotionConfig* GameConfig::getTimeShopSalesPromotionConfig(int id)
{
	TimeShopSalesPromotionConfig* config = NULL;
	MAPTIMESHOPSALESPROMOTIONCONFIGS::iterator it = _shop_time_sales_promotion.find(id);
	if (it != _shop_time_sales_promotion.end())
	{
		config = &it->second;
	}
	return config;
}


const MAPTYPEDROPBOXCONFIGS* GameConfig::getMapDropBox(int chapter_id, int section_id)
{
	MAPTYPEDROPBOXCONFIGS* ret = NULL;
	std::pair<int, int> entry_pair;
	entry_pair.first = chapter_id;
	entry_pair.second = section_id;
	MAPMAPDROPBOXCONFIGS::iterator it = _map_drop_box_configs.find(entry_pair);
	if (it != _map_drop_box_configs.end())
	{
		ret = &it->second;
	}
	return ret;
}

const globalConfig& GameConfig::getGlobalConfig()
{
	return _global_config;
}

void GameConfig::Load(DBQuery* p)
{

	if (p)
	{
		DBQuery& query = *p;
		query << "select * from `dream_heroes_task_config`;";
		SDBResult sResult = query.store();
		int rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i ++)
		{
			DBRow& row = sResult[i];
			message::MsgTaskConfigInfo TaskInfoConfig;
			TaskInfoConfig.set_taskid(row["task_id"]);	

			message::MsgTaskConditionInfo* info_1 = TaskInfoConfig.add_task_condition();
			int tempType = row["type_1"];
			int argument = row["argument_1"];
			int argument_1 = row["argument_1_1"];
			info_1->set_tasktype((message::TaskType)(tempType));
			info_1->set_argument_1(argument);
			info_1->set_argument_2(argument_1);

			message::MsgTaskConditionInfo* info_2 = TaskInfoConfig.add_task_condition();
			tempType = row["type_2"];
			argument = row["argument_2"];
			argument_1 = row["argument_2_1"];
			message::TaskType type_task = (message::TaskType)(tempType);
			info_2->set_tasktype(type_task);
			info_2->set_argument_1(argument);
			info_2->set_argument_2(argument_1);

			
			TaskInfoConfig.set_require_unlock_chapter(row["require_chapter"]);
			TaskInfoConfig.set_require_unlock_section(row["require_section"]);
			TaskInfoConfig.set_require_time(row["require_time"]);
			TaskInfoConfig.set_require_unlock_complete_task_count(row["require_complete_task_count"]);
			TaskInfoConfig.set_gift_gold(row["gift_gold"]);
			TaskInfoConfig.set_describe(row["describe"]);
			_tasks.insert(MAPTASKS::value_type(TaskInfoConfig.taskid(), TaskInfoConfig));
		}

		query.reset();
		sResult.clear();
		query << "select * from `hero_shop_config`;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i ++)
		{
			DBRow& row = sResult[i];
			message::MsgShopConfigInfo ShopConfig;
			ShopConfig.set_grid_id(row["grid_id"]);
			ShopConfig.set_hero_id(row["hero_id"]);
			ShopConfig.set_require_gold(row["require_gold"]);
			ShopConfig.set_cheape_gold(0);
			_shop_heroes.insert(MAPSHOPHEROCONFIGS::value_type(ShopConfig.grid_id(), ShopConfig));
		}
		query.reset();
		sResult.clear();

		query << "select * from `chapter_unlock_config`;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			message::MsgChapterConfigInfo ChapterConfig;
			ChapterConfig.set_chapter_id(row["chapter_id"]);
			ChapterConfig.set_required_chapter_id(row["require_chapter_id"]);
			ChapterConfig.set_required_section_id(row["require_section_id"]);
			ChapterConfig.set_required_task_complete_count(row["require_task_complete_count"]);
			ChapterConfig.set_require_gold(row["require_gold"]);
			ChapterConfig.set_max_section_count(row["max_section_count"]);
			_chapter_config_infos.insert(MAPCHAPTERCONFIGINFOS::value_type(ChapterConfig.chapter_id(), ChapterConfig));
		}

		query.reset();
		sResult.clear();
		query << "select * from `global_config`;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		if (rows_length != 0)
		{
			DBRow& row = sResult[0];
			_global_config.config_gold_ = row["config_gold"];
			_global_config.hero_unlock_count_ = row["hero_unlock_count"];
			_global_config.day_Refresh_time_ = row["day_refresh_time"];
			_global_config.day_free_task_count_ = row["day_free_task_count"];
			_global_config.day_task_advertisement_task_cd_ = row["day_task_advertisement_task_cd"];
			_global_config.hero_max_tasks_count_ =  row["hero_max_tasks_count"];
		}

		query.reset();
		sResult.clear();
		query << "select * , UNIX_TIMESTAMP(`begin_time`), UNIX_TIMESTAMP(`end_time`) from `time_shop_sales_promotion`;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i ++)
		{
			DBRow& row = sResult[i];
			TimeShopSalesPromotionConfig SalesPromotionConfig;
			SalesPromotionConfig.id_ = row["sales_promotion_id"];
			SalesPromotionConfig.grid_id_ = row["grid_id"];
			SalesPromotionConfig.cheap_gold_ = row["cheap_gold"];
			SalesPromotionConfig.begin_time_ = row["UNIX_TIMESTAMP(`begin_time`)"];
			SalesPromotionConfig.end_time_ = row["UNIX_TIMESTAMP(`end_time`)"];
			_shop_time_sales_promotion.insert(MAPTIMESHOPSALESPROMOTIONCONFIGS::value_type(SalesPromotionConfig.id_, SalesPromotionConfig));
		}


		query.reset();
		sResult.clear();
		query << "select * from gold_shop_config_info;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			message::MsgGoldShopConfigInfo entry;
			entry.set_id(row["shop_id"]);
			entry.set_describe(row["describe"]);
			entry.set_gold(row["gold"]);
			entry.set_resource_id(row["resource_id"]);
			entry.set_money(row["money"]);
			_gold_shop_config_infos[entry.id()] = entry;
		}

		query.reset();
		sResult.clear();
		query << "select * from drop_box;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			ObjDropBoxConfig entry;
			entry.obj_id_ = row["obj_id"];
			int temp = row["obj_type"];
			entry.type_ = (message::SubType)temp;
			entry.base_gold_ = row["base_gold"];
			entry.random_gold_ = row["random_gold"];

			MAPTYPEDROPBOXCONFIGS::iterator it = _drop_box_configs.find(entry.type_);
			if (it == _drop_box_configs.end())
			{
				MAPDROPBOXCONFIGS entry_map;
				_drop_box_configs.insert(MAPTYPEDROPBOXCONFIGS::value_type(entry.type_, entry_map));
			}
			_drop_box_configs[entry.type_][entry.obj_id_] = entry;

		}

		query.reset();
		sResult.clear();
		query << "select * from map_obj_count;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			MapBehaviorConfig entry;
			entry.key_.first = row["chapter_id"];
			entry.key_.second = row["section_id"];
			entry.obj_behavior_id_ = row["obj_behavior_id"];
			entry.obj_count_ = row["obj_count"];
			entry.max_pos_count_ = row["max_pos_count"];
			
			MAPALLBEHAVIORCONFIGS::iterator it = _map_behavior_config.find(entry.key_);
			if (it == _map_behavior_config.end())
			{
				MAPBEHAVIORCONFIGS map_entry;
				_map_behavior_config.insert(MAPALLBEHAVIORCONFIGS::value_type(entry.key_, map_entry));
			}
			_map_behavior_config[entry.key_][entry.obj_behavior_id_] = entry;
			
		}


		query.reset();
		sResult.clear();
		query << "select * from random_object;";
		sResult = query.store();
		rows_length = sResult.num_rows();
		for (int i = 0; i < rows_length; i++)
		{
			DBRow& row = sResult[i];
			std::pair<int, int> entry_pair;
			int behavior_id = row["behavior_id"];
			entry_pair.first = row["chapter_id"];
			entry_pair.second = row["section_id"];
			MapRandomObjConfig entry;
			entry.obj_id_ = row["obj_id"];
			int temp = row["obj_type"];
			entry.type_ = (message::SubType)temp;
			
			MAPALLRANDOMOBJCONFIGS::iterator it = _map_random_obj_configs.find(entry_pair);
			if (it == _map_random_obj_configs.end())
			{
				MAPRANDOMOBJCONFIGS map_entry;
				_map_random_obj_configs.insert(MAPALLRANDOMOBJCONFIGS::value_type(entry_pair, map_entry));
			}



			//// drop config
			MAPTYPEDROPBOXCONFIGS::iterator it_temp = _drop_box_configs.find(entry.type_);
			if (it_temp != _drop_box_configs.end())
			{
				MAPDROPBOXCONFIGS::iterator it_temp_id_config = _drop_box_configs[entry.type_].find(entry.obj_id_);
				if (it_temp_id_config != _drop_box_configs[entry.type_].end())
				{					
					MAPRANDOMOBJCONFIGS::iterator it_configs = _map_random_obj_configs[entry_pair].find(behavior_id);
					if (it_configs == _map_random_obj_configs[entry_pair].end())
					{
						MapRandomObjsConfig entry;
						entry.key_ = entry_pair;
						entry.obj_behavior_id_ = behavior_id;
						_map_random_obj_configs[entry_pair].insert(MAPRANDOMOBJCONFIGS::value_type(entry.obj_behavior_id_, entry));
					}
					_map_random_obj_configs[entry_pair][behavior_id].objs_.push_back(entry);

					MAPMAPDROPBOXCONFIGS::iterator it_drop_box_config = _map_drop_box_configs.find(entry_pair);
					if (it_drop_box_config == _map_drop_box_configs.end())
					{
						MAPTYPEDROPBOXCONFIGS map_entry;
						_map_drop_box_configs.insert(MAPMAPDROPBOXCONFIGS::value_type(entry_pair, map_entry));
					}

					MAPTYPEDROPBOXCONFIGS::iterator it_type_drop_box_config = _map_drop_box_configs[entry_pair].find(entry.type_);
					if (it_type_drop_box_config == _map_drop_box_configs[entry_pair].end())
					{
						MAPDROPBOXCONFIGS map_entry;
						_map_drop_box_configs[entry_pair].insert(MAPTYPEDROPBOXCONFIGS::value_type(entry.type_, map_entry));
					}
					_map_drop_box_configs[entry_pair][entry.type_][entry.obj_id_] = it_temp_id_config->second;
					
				}
				
			}

			
		}
	}	
}