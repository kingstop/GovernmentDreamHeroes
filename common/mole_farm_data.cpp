#include "mole_farm_data.h"
/*
gs_farm_data::gs_farm_data()
{

}

gs_farm_data::~gs_farm_data()
{
	std::map<u64, mole_construction_data*>::iterator it = constructions_.begin();
	for (; it != constructions_.end(); ++ it)
	{
		mole_construction_data* entry = it->second;
		delete entry;
	}
}

void gs_farm_data::clear_data()
{
	unlock_areas_.clear();	
	resource_.clear();
	constructions_resource_.clear();
	std::map<u64, mole_construction_data*>::iterator it_construction = constructions_.begin();
	for (; it_construction != constructions_.end(); ++ it_construction)
	{
		delete it_construction->second;
	}
	constructions_.clear();
	clear_action();
}

void gs_farm_data::clear_action()
{
	std::map<u64, mole_construction_action_data*>::iterator it_construction_action =  constructions_actions_.begin();
	for (; it_construction_action != constructions_actions_.end(); ++ it_construction_action)
	{
		delete it_construction_action->second;
	}
	constructions_actions_.clear();
}

void gs_farm_data::set(const message::MoleFarmInfo* info)
{
	clear_data();
	int construction_size = info->construstions_size();
	for (int i = 0; i < construction_size; i ++)
	{
		const message::MoleConstrustion entry = info->construstions(i);
		if (constructions_.find(entry.sid()) != constructions_.end())
		{
			//LOG ERROR
		}
		else
		{
			mole_construction_data* entry_data = new mole_construction_data();
			entry_data->set(&entry);
			constructions_[entry_data->id_] = entry_data;
		}		
	}

	int unlock_areas_size = info->unlock_areas_size();
	for (int i = 0; i < unlock_areas_size; i ++)
	{
		int un_lock = info->unlock_areas(i);
		unlock_areas_.push_back(un_lock);
	}

	int resource_size_ = info->resource_size();
	for (int i = 0; i < resource_size_; i ++)
	{
		const message::int32pair entry = info->resource(i);
		resource_[entry.number_1()] = entry.number_2();
	}


	int counstruction_resource_size = info->construction_resource_size();
	for (int i = 0; i < counstruction_resource_size; i ++)
	{
		const message::ConstructionResource entry = info->construction_resource(i);
		int entry_size = entry.resource_size();
		MAPINT temp_map;
		for (int j = 0; j < entry_size; j ++)
		{
			const message::int32pair pair_entry = entry.resource(j);
			temp_map[pair_entry.number_1()] = pair_entry.number_2();
		}
		constructions_resource_[entry.construction_id()] = temp_map;
	}

	int action_size = info->actions_size();
	for(int i = 0; i < action_size; i ++)
	{
		message::ActionInfo action_entry = info->actions(i);
		std::map<u64, mole_construction_action_data*>::iterator it_action = constructions_actions_.find(action_entry.action_id());
		if (it_action != constructions_actions_.end())
		{
			delete it_action->second;
			constructions_actions_.erase(it_action);
		}
		mole_construction_action_data* data_entry = new mole_construction_action_data();
		data_entry->set(&action_entry);
		constructions_actions_[data_entry->construction_id_] = data_entry;
	}
	level_ = info->level();
	exp_ = info->exp();
	guid_ = info->guid();
}


message::MoleFarmInfo* gs_farm_data::create_base_info()
{
	message::MoleFarmInfo* base_info_entry = new message::MoleFarmInfo();
	base_info_entry->set_guid(guid_);
	std::vector<s32>::iterator it_unlock = unlock_areas_.begin();
	for (; it_unlock != unlock_areas_.end(); ++ it_unlock)
	{
		base_info_entry->add_unlock_areas(*it_unlock);
	}

	std::map<u64, mole_construction_data*>::iterator it_construction = constructions_.begin();
	for (; it_construction != constructions_.end(); ++ it_construction)
	{
		u64 sid_temp = it_construction->first;
		mole_construction_data* entry = it_construction->second;
		message::MoleConstrustion* construction_entry = base_info_entry->add_construstions();
		entry->get(construction_entry);
	}

	std::map<int, int>::iterator it_resource = resource_.begin();
	for (; it_resource != resource_.end(); ++ it_resource)
	{
		int config_id_entry = it_resource->first;
		int count_entry = it_resource->second;
		message::int32pair* entry = base_info_entry->add_resource();
		entry->set_number_1(config_id_entry);
		entry->set_number_2(count_entry);		
	}

	std::map<u64, mole_construction_action_data*>::iterator it_action =  constructions_actions_.begin();
	for (; it_action != constructions_actions_.end(); ++ it_action)
	{
		mole_construction_action_data* action_entry_data = it_action->second;
		message::ActionInfo* action_msg = base_info_entry->add_actions();
		action_entry_data->get(action_msg);
	}

	MAP_CONSTRUCTIONS_RESOURCE::iterator it_construction_resource =  constructions_resource_.begin();
	for (; it_construction_resource != constructions_resource_.end(); ++ it_construction_resource)
	{
		message::ConstructionResource* msg_entry =  base_info_entry->add_construction_resource();
		msg_entry->set_construction_id(it_construction_resource->first);
		MAPINT::iterator it_intpair = it_construction_resource->second.begin();
		for (; it_intpair != it_construction_resource->second.end(); ++ it_intpair)
		{
			message::int32pair* msg_pair_entry = msg_entry->add_resource();
			msg_pair_entry->set_number_1(it_intpair->first);
			msg_pair_entry->set_number_2(it_intpair->second);
		}
	}
	base_info_entry->set_level(level_);
	base_info_entry->set_exp(exp_);
	return base_info_entry;
}

//////////////////////////////////////////////////////////////////////////
mole_farm_data::mole_farm_data(void):flag_(0)
{
}


mole_farm_data::~mole_farm_data(void)
{
}

mole_farm_data::mole_farm_data(const message::MoleFarmInfoFull* info, u16 gs):flag_(0)
{
	set(&info->base_info());

	acc_id_ = info->extra_info().account();
	gs_ = gs;
	create_time_ = info->extra_info().createtime();
}


message::MoleFarmInfoExtra* mole_farm_data::create_extra_info()
{
	message::MoleFarmInfoExtra* extra_info_entry = new message::MoleFarmInfoExtra();
	extra_info_entry->set_account(acc_id_);
	extra_info_entry->set_createtime(create_time_);
	extra_info_entry->set_flag(flag_);
	return extra_info_entry;
}

message::MoleFarmInfoFull* mole_farm_data::create_message_info()
{
	message::MoleFarmInfoFull* fullentry = new message::MoleFarmInfoFull();
	message::MoleFarmInfo* base_info_entry = create_base_info();
	message::MoleFarmInfoExtra* extra_info_entry = create_extra_info();
	fullentry->set_allocated_base_info(base_info_entry);
	fullentry->set_allocated_extra_info(extra_info_entry);
	return fullentry;
}
*/
