#pragma once
#include "common_type.h"
#include "iostream"
#include <vector>
#include <map>
/* 
#include "message/mole_common_server.pb.h"


struct mole_construction_data
{
	u64 id_;
	s32 config_id_;
	s32 level_;
	s32 pos_x_;
	s32 pos_y_;
	bool recycle_;
	void set(const message::MoleConstrustion* info)
	{
		id_ = info->sid();
		config_id_ = info->config_id();
		level_ = info->level();
		pos_x_ = info->pos_x();
		pos_y_ = info->pos_y();
		recycle_ = info->recycle();
	}

	void get(message::MoleConstrustion* entry)
	{
		entry->set_sid(id_);
		entry->set_config_id(config_id_);
		entry->set_pos_x(pos_x_);
		entry->set_pos_y(pos_y_);
		entry->set_level(level_);
		entry->set_recycle(recycle_);	
	}
};

struct mole_construction_action_data
{
	int action_type_;
	int start_time_;	
	int argument_;
	int action_id_;
	u64 construction_id_;
	void set(const message::ActionInfo* info)
	{
		action_type_ = info->action_type();
		start_time_ = info->start_time();
		action_id_ = info->action_id();
		construction_id_ = info->construction_id();
		argument_ = info->argument();
	}

	void get( message::ActionInfo* info)
	{
		info->set_action_type(action_type_);
		info->set_start_time(start_time_);
		info->set_action_id(action_id_);
		info->set_construction_id(construction_id_);
		info->set_argument(argument_);
	}
};
typedef std::map<int, int> MAPINT;
typedef std::map<u64, MAPINT> MAP_CONSTRUCTIONS_RESOURCE;
struct gs_farm_data
{
	gs_farm_data();
	~gs_farm_data();

	message::MoleFarmInfo* create_base_info();
	void set(const message::MoleFarmInfo* info);
	void clear_data();
	void clear_action();

	std::vector<s32> unlock_areas_;
	std::map<u64, mole_construction_data*> constructions_;
	std::map<int, int> resource_;
	std::map<u64, mole_construction_action_data*> constructions_actions_;
	MAP_CONSTRUCTIONS_RESOURCE constructions_resource_;
	std::string guid_;
	int level_;
	int exp_;
};

struct mole_farm_data : public gs_farm_data
{
	mole_farm_data(void);
	~mole_farm_data(void);
	mole_farm_data(const message::MoleFarmInfoFull* info, u16 gs);
	message::MoleFarmInfoFull* create_message_info();
	//message::MoleFarmInfo* create_base_info();
	message::MoleFarmInfoExtra* create_extra_info();
	//void set(const message::MoleFarmInfo* info);

	u64 acc_id_;	
	u16 gs_;
	u32 flag_;
	u32 create_time_;

	//buffs;
	//std::vector<int32> 
};
*/
