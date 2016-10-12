#include "stdafx.h"
#include "DreamHero.h"
#include "session.h"
#define _SAVE_PLAYER_TIME_  (10 * _TIME_SECOND_MSEL_)
#define _REMOVE_PLAYER_TIME_  (20 * _TIME_SECOND_MSEL_)
DreamHero::DreamHero()
{
	_info.set_gold(0);
	_info.set_name("test");
	_online = false;
	_day_offset_time = gGameConfig.getGlobalConfig().day_Refresh_time_ * 60 * 60;
	_current_chapter = -1;
	_current_section = -1;

	_current_task_count = 0;
	_last_task_advertisement_time = 0;
	_day_offset_time = 0;
	_gm_level = 0;
}


DreamHero::~DreamHero()
{
	StopSave();
}


message::MsgHeroData DreamHero::get_info()
{
	return _info;
}


u64 DreamHero::get_account()
{
	return _account;
}

void DreamHero::set_account(u64 account)
{
	_account = account;
	
}

void DreamHero::set_info(const message::MsgHeroDataDB2GS* info)
{
	_info.CopyFrom(info->data());
	_current_chapter = info->current_chapter();
	_current_section = info->current_section();
	_last_task_advertisement_time = info->last_task_advertisement_time();
	_current_task_count = info->free_task_count();
	int size_special_kills =  info->special_kills_size();
	_gm_level = info->gm_level();
	_special_kills.clear();
	for (int i = 0; i < size_special_kills; i ++)
	{
		const message::MsgMapSpecialKill& entry = info->special_kills(i);
		std::pair<int, int> pair_entry;
		pair_entry.first = entry.chapter_id();
		pair_entry.second = entry.section_id();
		int kills_length = entry.kills_size();
		for (size_t j = 0; j < kills_length; j++)
		{
			const message::MsgObjConfig entry_config = entry.kills(j);
			_special_kills[pair_entry].push_back(entry_config);
		}		
	}

}


void DreamHero::set_session(Session* session)
{
	_session = session;
}

Session* DreamHero::get_session()
{
	return _session;
}

std::vector<int> MapVersionFormat(std::string cur_version)
{
	std::vector<std::string> vcstr;
	std::vector<int> vcret;
	SplitStringA(cur_version, ".", vcstr);
	for (int i = 0; i < vcstr.size(); i ++)
	{
		if (isIntger(vcstr[i].c_str()))
		{
			vcret.push_back(atoi(vcstr[i].c_str()));
		}
		else
		{
			break;
		}
	}
	return vcret;
}


void DreamHero::StartSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == false)
	{
		gEventMgr.addEvent(this, &DreamHero::SaveHero, EVENT_SAVE_PLAYER_DATA_, _SAVE_PLAYER_TIME_, -1, 0);
	}
}

void DreamHero::StopSave()
{
	if (gEventMgr.hasEvent(this, EVENT_SAVE_PLAYER_DATA_) == true)
	{
		gEventMgr.removeEvents(this, EVENT_SAVE_PLAYER_DATA_);
	}
}

void DreamHero::StartDestroyTime()
{
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_) == false)
	{
		gEventMgr.addEvent(this, &DreamHero::Destroy, EVENT_DESTROY_PLAYER_, _REMOVE_PLAYER_TIME_, -1, 0);
	}
	else
	{
		gEventMgr.modifyEventTime(this, EVENT_DESTROY_PLAYER_, _REMOVE_PLAYER_TIME_);
	}
}

void DreamHero::StopDestroyClock()
{
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_)== true)
	{
		gEventMgr.removeEvents(this, EVENT_DESTROY_PLAYER_);
	}
}

void DreamHero::Destroy()
{
	if (_session)
	{
		_session->setDreamHero(NULL);

	}
	_parent->DestroyHero(this);
}

void DreamHero::set_parent(DreamHeroManager* p)
{
	_parent = p;
}

void DreamHero::set_online(bool online)
{
	if (gEventMgr.hasEvent(this, EVENT_DESTROY_PLAYER_) == false)
	{
		gEventMgr.removeEvents(this, EVENT_DESTROY_PLAYER_);
	}
	_online = online;
}

bool DreamHero::is_online()
{
	return _online;
}

int DreamHero::getGMLevel()
{
	return _gm_level;
}


void DreamHero::SetGMLevel(int level)
{
	_gm_level = level;
}

void DreamHero::dayRefresh(bool need_send_msg)
{
	bool can_Refresh_task = false;
	if (_last_task_advertisement_time != 0)
	{
		u64 temp_time = g_server_time - _day_offset_time;
		if (same_day(temp_time, _last_task_advertisement_time) == false)
		{			
			can_Refresh_task = true;
		}		 
	}
	else
	{
		can_Refresh_task = true;
	}

	if (can_Refresh_task)
	{
		_current_task_count = 0;
		if (_info.tasks_size() < gGameConfig.getGlobalConfig().hero_max_tasks_count_)
		{
			message::MsgTaskConfigInfo task_config_entry = RadnomTaskInfo();
			if (task_config_entry.taskid() != 0)
			{
				if (need_send_msg)
				{
					message::MsgS2CNewTaskNotify msg;
					msg.mutable_task()->CopyFrom(task_config_entry);
					sendPBMessage(&msg);
				}
				message::MsgTaskInfo* task_entry = _info.add_tasks();
				task_entry->set_argument_1(0);
				task_entry->set_taskid(task_config_entry.taskid());
				task_entry->set_usetime(0);		
			}
		}
	}
}


message::MsgTaskConfigInfo DreamHero::RadnomTaskInfo(int give_up_task)
{	
	std::vector<int> vcTemp;
	const MAPTASKS* map_tasks = gGameConfig.getMapTasks();
	MAPTASKS::const_iterator it = map_tasks->begin();
	int require_chapter = 0;
	int require_section = 0;
	int require_complete_task_count = 0;
	for (; it != map_tasks->end(); ++it)
	{
		const message::MsgTaskConfigInfo& config_entry = it->second;
		int tasks_length = _info.tasks_size();
		bool need_continue = false;
		for (int i = 0; i < tasks_length; i ++)
		{
			if (_info.tasks(i).taskid() == config_entry.taskid() || give_up_task == config_entry.taskid())
			{
				need_continue = true;
				break;
			}
		}

		if (need_continue)
		{
			continue;
		}

		require_complete_task_count = config_entry.require_unlock_complete_task_count();
		if (_info.complete_task_count() <require_complete_task_count)
		{
			continue;
		}
		require_chapter = config_entry.require_unlock_chapter();
		require_section = config_entry.require_unlock_section();
		if (require_chapter != 0)
		{
			need_continue = true;
			int record_size = _info.records_size();
			for (int i = 0; i < record_size; i ++)
			{
				message::MsgIntPair pair_entry = _info.records(i);
				if (pair_entry.number_1() ==  require_chapter)
				{
					if (require_section <= pair_entry.number_2())
					{
						need_continue = false;
					}					
					break;
				}								
			}
			if (need_continue)
			{
				continue;
			}
		}
		vcTemp.push_back(config_entry.taskid());
	}

	message::MsgTaskConfigInfo TaskConfigEntry;
	TaskConfigEntry.set_taskid(0);
	if (vcTemp.size() != 0)
	{
		int index = rand() % vcTemp.size();
		int id_task = vcTemp[index];
		const message::MsgTaskConfigInfo* task_config_entry = gGameConfig.getMapTask(id_task);
		if (task_config_entry != NULL)
		{
			TaskConfigEntry.CopyFrom(*task_config_entry);
		}
	}	
	return TaskConfigEntry;
}



void DreamHero::ReqUnlockChapter(const message::MsgC2SReqUnlockChapter* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int record_size_temp = _info.records_size();
	message::MsgS2CUnlockChapterACK msgACK;
	message::GameError en_error = message::Error_NO;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_current_gold(_info.gold());
	const message::MsgChapterConfigInfo* info_config = gGameConfig.getChapterConfigInfo(chapter_id_temp);
	if (info_config == NULL)
	{
		en_error = message::Error_UnlockChapterFailedNotFoundTheUnlockChapter;
	}
	else
	{
		if (_info.complete_task_count() < info_config->required_task_complete_count())
		{
			en_error = message::Error_UnlockChapterFailedYouHaveToCompleteEnoughTasks;
		}
		else
		{
			info_config->required_task_complete_count();
			en_error = message::Error_UnlockChapterFailedTheRequiredSectionNotPass;
			bool bret_find_required_chapter_section = false;
			for (int i = 0; i < record_size_temp; i++)
			{
				message::MsgIntPair pair_entry = _info.records(i);
				if (pair_entry.number_1() == chapter_id_temp)
				{
					en_error = message::Error_TheChapterIsNotLock;
					return;
				}

				if (info_config->required_chapter_id() == pair_entry.number_1())
				{
					if (pair_entry.number_2() >= info_config->required_section_id())
					{
						en_error = message::Error_NO;
					}
				}
			}

			if (en_error == message::Error_NO)
			{
				int gold_current = _info.gold();
				gold_current -= info_config->require_gold();
				if (gold_current < 0)
				{
					en_error = message::Error_UnlockChapterFailedYouNotHaveEnoughGold;
				}
				else
				{
					message::MsgIntPair* pair_entry = _info.add_records();
					pair_entry->set_number_1(chapter_id_temp);
					pair_entry->set_number_2(0);
					_info.set_gold(gold_current);
					msgACK.set_current_gold(gold_current);
				}
			}
		}		
	}
	msgACK.set_error(en_error);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqExitGame(const message::MsgC2SReqExitGame* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int section_id_temp = msg->section_id();
	int task_size = msg->task_infos_size();
	int complete_tasks_temp_size = msg->complete_tasks_size();
	message::MsgS2CExitGameACK msgACK;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_section_id(section_id_temp);
	msgACK.set_current_gold(0);
	msgACK.set_success(msg->success());
	msgACK.set_error(message::Error_NO);
	int cur_complete_task_count = _info.complete_task_count();
	if (chapter_id_temp == -1&& section_id_temp == -1 || chapter_id_temp != _current_chapter || section_id_temp != _current_section)
	{
		msgACK.set_error(message::Error_NotEnterTheExitGame);
	}
	else
	{
		google::protobuf::RepeatedPtrField< ::message::MsgTaskInfo >* temp_repeated = NULL;
		::google::protobuf::RepeatedPtrField< ::message::MsgTaskInfo >::iterator it;
		temp_repeated = _info.mutable_tasks();
		for (int i = 0; i < task_size; i++)
		{
			message::MsgTaskInfo task_entry = msg->task_infos(i);
			int current_task_size = _info.tasks_size();
			for (int i = 0; i < current_task_size; i++)
			{
				message::MsgTaskInfo* cur_info = _info.mutable_tasks(i);
				if (cur_info->taskid() == task_entry.taskid())
				{
					cur_info->CopyFrom(task_entry);
					break;
				}
			}
		}

		for (int i = 0; i < complete_tasks_temp_size; i++)
		{
			int task_id = msg->complete_tasks(i);
			const message::MsgTaskConfigInfo* info_task_config = gGameConfig.getMapTask(task_id);
			bool try_to_find_task_info = false;
			int task_gift_gold = 0;
			if (info_task_config != NULL)
			{
				temp_repeated = _info.mutable_tasks();
				it = temp_repeated->begin();
				for (; it != temp_repeated->end(); ++it)
				{
					message::MsgTaskInfo task_entry = (*it);
					if (task_entry.taskid() == task_id)
					{
						try_to_find_task_info = true;
						task_gift_gold = info_task_config->gift_gold();
						cur_complete_task_count++;
						temp_repeated->erase(it);
						break;
					}
				}
			}
			else
			{
				//need error log;
			}
			message::MsgIntPair* entry_pair = msgACK.add_task_gift();
			entry_pair->set_number_1(task_id);
			entry_pair->set_number_2(task_gift_gold);
		}
		int gold_entry = _info.gold();
		if (gold_entry < 0)
		{
			gold_entry = 0;
		}

		// need modify
		gold_entry += msg->gold();
		int length_task_gift = msgACK.task_gift_size();
		for (int i = 0; i < length_task_gift; i++)
		{
			const message::MsgIntPair entry = msgACK.task_gift(i);
			gold_entry += entry.number_2();
		}

		temp_repeated = _info.mutable_tasks();
		it = temp_repeated->begin();
		for (; it != temp_repeated->end(); ++it)
		{
			message::MsgTaskInfo* info_entry = msgACK.add_task_infos();
			info_entry->CopyFrom(*it);
		}
		if (msg->success())
		{
			bool find_chapter = false;
			int record_size = _info.records_size();
			for (int i = 0; i < record_size; i++)
			{
				message::MsgIntPair* pair_entry = _info.mutable_records(i);
				if (pair_entry->number_1() == chapter_id_temp)
				{
					if (section_id_temp == pair_entry->number_2() + 1)
					{
						pair_entry->set_number_2(section_id_temp);
						find_chapter = true;
					}
				}
			}
		}
		msgACK.set_current_gold(gold_entry);
		_info.set_gold(gold_entry);
		_info.set_complete_task_count(cur_complete_task_count);
		std::pair<int, int> pair_entry;
		pair_entry.first = _current_chapter;
		pair_entry.second = _current_section;
		SPECIALKILLS::iterator it_special_kill = _special_kills.find(pair_entry);
		if (it_special_kill == _special_kills.end())
		{
			std::vector<message::MsgObjConfig> vc_temp;
			_special_kills.insert(SPECIALKILLS::value_type(pair_entry, vc_temp));
		}
		int special_kill_size = msg->special_kill_list_size();
		for (int i = 0; i < special_kill_size; i ++)
		{
			const message::MsgObjConfig obj_config_entry = msg->special_kill_list(i);
			bool need_add = true;
			std::vector<message::MsgObjConfig>::iterator it_special_temp = _special_kills[pair_entry].begin();
			for (; it_special_temp != _special_kills[pair_entry].end(); ++ it_special_temp)
			{
				if (it_special_temp->id() == obj_config_entry.id() && it_special_temp->type() == obj_config_entry.type())
				{
					need_add = false;
					break;
				}
			}
			if (need_add)
			{
				_special_kills[pair_entry].push_back(obj_config_entry);
			}
		}
	}

	_current_chapter = -1;
	_current_section = -1;
	msgACK.set_complete_task_count(cur_complete_task_count);
	sendPBMessage(&msgACK);	
}

void DreamHero::RefreshTask(int give_up_task_id)
{
	message::GameError msgError = message::Error_NO;	
	if (_current_task_count > gGameConfig.getGlobalConfig().day_free_task_count_)
	{
		s64 diff_time = g_server_time - _last_task_advertisement_time;
		if (diff_time < gGameConfig.getGlobalConfig().day_task_advertisement_task_cd_)
		{
			msgError = message::Error_RefreshAdvertisementTaskFailedInCD;
		}
	}

	google::protobuf::RepeatedPtrField< message::MsgTaskInfo >* repeated_tasks = _info.mutable_tasks();
	if (msgError == message::Error_NO)
	{		
		if (give_up_task_id != 0)
		{
			msgError = message::Error_RefreshAdvertisementTaskFailedNotFoundGiveUpTaskID;
			google::protobuf::RepeatedPtrField< message::MsgTaskInfo >::iterator it = repeated_tasks->begin();
			for (; it != repeated_tasks->end(); ++it)
			{
				if (it->taskid() == give_up_task_id)
				{
					msgError = message::Error_NO;
					repeated_tasks->erase(it);
					break;
				}
			}
		}
		
		if (msgError == message::Error_NO)
		{
			if (repeated_tasks->size() <= gGameConfig.getGlobalConfig().hero_max_tasks_count_ - 1)
			{			
				_current_task_count++;
				_last_task_advertisement_time = g_server_time;
			}
			else
			{
				msgError = message::Error_RefreshAdvertisementTaskFailedUnknow;
			}
		}		
	}
	message::MsgTaskConfigInfo info_task_config = RadnomTaskInfo(give_up_task_id);
	bool have_task = false;
	if (info_task_config.taskid() != 0 && msgError == message::Error_NO)
	{
		have_task = true;
	}

	if (have_task)
	{
		message::MsgTaskInfo* entry = repeated_tasks->Add();
		entry->set_taskid(info_task_config.taskid());
		entry->set_argument_1(0);
		entry->set_usetime(0);
	}

	if (give_up_task_id != 0)
	{
		message::MsgS2CAdvertisementRefreshTaskACK msgACK;
		msgACK.set_give_up_task_id(give_up_task_id);
		if (have_task)
		{
			message::MsgTaskConfigInfo* info = msgACK.add_infos();
			info->CopyFrom(info_task_config);
		}
		msgACK.set_error(msgError);
		sendPBMessage(&msgACK);
	}
	else
	{
		message::MsgS2CAdvertisementApplyTaskACK msgACK;
		msgACK.set_error(msgError);
		if (have_task)
		{
			message::MsgTaskConfigInfo* info = msgACK.add_infos();
			info->CopyFrom(info_task_config);
		}
		sendPBMessage(&msgACK);
	}

}

void DreamHero::ReqAdvertisementApplyTask(const message::MsgC2SReqAdvertisementApplyTask* msg)
{
	RefreshTask(0);
}

void DreamHero::ReqReplaceTask(const message::MsgS2CCmdReqReplaceTask* msg)
{
	int task_count = msg->task_count();
	if (task_count > gGameConfig.getGlobalConfig().hero_max_tasks_count_)
	{
		task_count = gGameConfig.getGlobalConfig().hero_max_tasks_count_;
	}
	_info.clear_tasks();

	message::MsgS2CCmdReplaceTaskACK msgACK;
	for (size_t i = 0; i < task_count; i++)
	{
		message::MsgTaskConfigInfo info_task_config = RadnomTaskInfo();
		if (info_task_config.taskid() != 0)
		{
			message::MsgTaskConfigInfo* entry = msgACK.add_infos();
			entry->CopyFrom(info_task_config);

			message::MsgTaskInfo* entry_info = _info.mutable_tasks()->Add();
			entry_info->set_taskid(info_task_config.taskid());
			entry_info->set_argument_1(0);
			entry_info->set_usetime(0);
		}
	}
	msgACK.set_error(message::Error_NO);
	sendPBMessage(&msgACK);

}
void DreamHero::ReqModifyCurrentHero(int grid_id)
{
	int current_grid_id = grid_id;
	int heroes_length = _info.heroes_size();
	message::GameError en = message::Error_NO;
	message::MsgS2CModifyCurrentHeroACK msg;
	msg.set_current_grid(_info.current_hero());

	if (heroes_length > current_grid_id)
	{
		bool b_character = _info.heroes(current_grid_id);
		if (b_character)
		{
			_info.set_current_hero(current_grid_id);
		}
		else
		{
			en = message::Error_ModifyCurrentFailedTheCharacterIsLock;
		}

	}
	else
	{
		en = message::Error_ModifyCurrentFailedTheCharacterIsLock;
	}
	msg.set_error(en);
	sendPBMessage(&msg);
}

void DreamHero::ReqResetMap(const message::MsgC2SCmdReqResetMap* msg)
{
	msg->chapter_id();
	msg->section_id();
	std::pair<int, int> pair_entry;
	pair_entry.first = msg->chapter_id();
	pair_entry.second = msg->section_id();
	SPECIALKILLS::iterator it = _special_kills.find(pair_entry);

	if (it != _special_kills.end())
	{
		_special_kills.erase(it);
	}
	message::MsgS2CCmdResetMapACK msgACK;
	msgACK.set_chapter_id(msg->chapter_id());
	msgACK.set_section_id(msg->section_id());
	msgACK.set_error(message::Error_NO);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqModifyGold(const message::MsgC2SCmdReqModifyGold* msg)
{
	int current_gold = _info.gold();
	message::MsgS2CCmdModifyGoldACK msgACK;
	current_gold += msg->gold();
	if (current_gold < 0)
	{
		current_gold = 0;
	}
	_info.set_gold(current_gold);
	msgACK.set_gold(current_gold);
	sendPBMessage(&msgACK);
}

void DreamHero::ReqBuyHero(const message::MsgC2SReqBuyHero* msg)
{
	int buy_grid =  msg->grid();
	int buy_gold =  msg->gold();
	const message::MsgShopConfigInfo* buy_grid_config = gGameConfig.getShopConfig(buy_grid);
	message::MsgS2CBuyHeroACK msgACK;
	msgACK.set_grid(buy_grid);
	msgACK.set_current_gold(_info.gold());
	message::GameError en = message::Error_NO;
	if (buy_grid_config == NULL)
	{
		en = message::Error_BuyHeroFailedNotFoundGrid;
	}
	else
	{
		int cheap_gold =  gShopSalesPromotionManager.getCheapGold(buy_grid);
		int current_require_gold = buy_grid_config->require_gold() - cheap_gold;
		if (current_require_gold < 0)
		{
			current_require_gold = 0;
		}
		if (current_require_gold != msg->gold())
		{
			en = message::Error_BuyHeroFailedThePriceIsOld;
		}
		else
		{
			int current_gold = _info.gold();
			int modify_gold = current_gold - current_require_gold;
			if (modify_gold < 0)
			{
				en = message::Error_BuyHeroFailedNotEnoughGold;
			}
			else
			{
				_info.set_gold(modify_gold);
				int heroes_length = _info.heroes_size();
				if (heroes_length <= buy_grid + 1)
				{
					for (size_t i = 0; i <= buy_grid + 1 - heroes_length; i++)
					{
						_info.add_heroes(false);
					}
				}
				_info.set_heroes(buy_grid, true);
				heroes_length = _info.heroes_size();
			}
		}
	}
	msgACK.set_current_gold(_info.gold());
	msgACK.set_error(en);
	sendPBMessage(&msgACK);
}


void DreamHero::ReqAdvertisementRefreshTask(const message::MsgC2SReqAdvertisementRefreshTask* msg)
{

	int give_up_task_id_temp = msg->give_up_task_id();
	RefreshTask(give_up_task_id_temp);
}


void DreamHero::EnterGame(int chapter_id, int section_id, bool admin)
{
	int chapter_id_temp = chapter_id;
	int section_id_temp = section_id;
	int records_length = _info.records_size();
	message::MsgS2CEnterGameACK msgACK;
	msgACK.set_chapter_id(chapter_id_temp);
	msgACK.set_section_id(section_id_temp);
	message::GameError en_error = message::Error_CanNotEnterGameTheInstanceIsLock;
	if (admin)
	{
		en_error = message::Error_NO;
	}
	else
	{
		en_error = message::Error_CanNotEnterGameTheInstanceIsLock;
		if (chapter_id_temp == -1 && section_id_temp == -1)
		{

		}
		else
		{
			for (int i = 0; i < records_length; i++)
			{
				const message::MsgIntPair record_entry = _info.records(i);
				int chapter_config_id = record_entry.number_1();
				int section_config_id = record_entry.number_2();
				if (chapter_config_id == chapter_id_temp)
				{
					int section = section_config_id + 1;
					if (section_id_temp <= section)
					{
						en_error = message::Error_NO;
					}
					else
					{
						en_error = message::Error_CanNotEnterGameTheSectionIsLock;
					}
					break;
				}
			}
		}
	}

	if (en_error == message::Error_NO)
	{
		std::pair<int, int> pair_entry;
		pair_entry.first = chapter_id_temp;
		pair_entry.second = section_id_temp;
		SPECIALKILLS::iterator it = _special_kills.find(pair_entry);
		if (it != _special_kills.end())
		{
			std::vector<message::MsgObjConfig>::iterator it_obj_config = it->second.begin();
			for (; it_obj_config != it->second.end(); ++ it_obj_config)
			{				
				message::MsgObjConfig* temp_entry = msgACK.add_kill_list();
				temp_entry->CopyFrom((*it_obj_config));
			}
		}

		const MAPTYPEDROPBOXCONFIGS* type_map_dropboxs = gGameConfig.getMapDropBox(chapter_id_temp, section_id_temp);
		if (type_map_dropboxs != NULL)
		{
			MAPTYPEDROPBOXCONFIGS::const_iterator it_type_drop = type_map_dropboxs->begin();
			for (; it_type_drop != type_map_dropboxs->end(); ++it_type_drop)
			{
				MAPDROPBOXCONFIGS::const_iterator it_map_drop = it_type_drop->second.begin();
				for (; it_map_drop != it_type_drop->second.end(); ++it_map_drop)
				{
					message::MsgDropBoxConfig* drop_box_entry = msgACK.add_drop_box_configs();
					ObjDropBoxConfig box_entry = it_map_drop->second;
					drop_box_entry->set_base_gold(box_entry.base_gold_);
					drop_box_entry->set_random_gold(box_entry.random_gold_);
					drop_box_entry->mutable_obj()->set_id(box_entry.obj_id_);
					drop_box_entry->mutable_obj()->set_type(box_entry.type_);
				}
			}
		}
		_current_chapter = chapter_id_temp;
		_current_section = section_id_temp;
	}
	msgACK.set_error(en_error);
	sendPBMessage(&msgACK);
}


void DreamHero::SendResetGameACK(message::GameError en)
{
	message::MsgS2CCmdResetGameACK msg;
	msg.mutable_info()->CopyFrom(_info);
	msg.set_error(en);
	msg.set_current_advertisement_count(_current_task_count);
	msg.set_last_advertisement_time(_last_task_advertisement_time);
	sendPBMessage(&msg);
}

void DreamHero::ResetGame()
{
	LoadFromConfig();
	SendResetGameACK(message::Error_NO);
}

void DreamHero::ReqEnterGame(const message::MsgC2SReqEnterGame* msg)
{
	int chapter_id_temp = msg->chapter_id();
	int section_id_temp = msg->section_id();	
	EnterGame(chapter_id_temp, section_id_temp, false);
}

void DreamHero::SendClientInit()
{
	StopDestroyClock();
	dayRefresh(false);
	StartSave();
	message::MsgS2CHeroesInit msg;
	msg.mutable_info()->CopyFrom(_info);
	int length = _info.tasks_size();
	for (size_t i = 0; i < length; i++)
	{
		const message::MsgTaskInfo entry =  _info.tasks(i);
		if (entry.taskid() != 0)
		{
			const message::MsgTaskConfigInfo* entry_config = gGameConfig.getMapTask(entry.taskid());
			if (entry_config != NULL)
			{
				message::MsgTaskConfigInfo* cur_entry = msg.add_task_config_infos();
				cur_entry->CopyFrom(*entry_config);
			}
			else
			{
				//need log;
			}
		}
	}

	msg.set_free_advertisement_config_count(gGameConfig.getGlobalConfig().day_free_task_count_);
	msg.set_current_advertisement_count(_current_task_count);
	msg.set_last_advertisement_time(_last_task_advertisement_time);
	msg.set_advertisement_time_cd(gGameConfig.getGlobalConfig().day_task_advertisement_task_cd_);
	msg.set_gm_level(getGMLevel());
	sendPBMessage(&msg);
	_online = true;
}


void DreamHero::LoadFromConfig()
{

	_current_chapter = 0;
	_current_section = 0;
	_current_task_count = 0;
	_last_task_advertisement_time = 0;
	_info.mutable_tasks()->Clear();
	_info.mutable_records()->Clear();
	int length = gGameConfig.getGlobalConfig().hero_unlock_count_;
	_info.clear_heroes();
	for (size_t i = 0; i < length; i++)
	{
		_info.add_heroes(true);
	}
	message::MsgIntPair* pair_entry = _info.mutable_records()->Add();
	pair_entry->set_number_1(1);
	pair_entry->set_number_2(0);

	_info.set_gold(gGameConfig.getGlobalConfig().config_gold_);
	_info.set_complete_task_count(0);
	
	_info.set_current_hero(0);
	_info.set_name("normal");
	
}

void DreamHero::SaveHero()
{
	std::string sql_temp;
	int length = _info.records_size();
	std::string record_temp;
	std::string heroes_temp;
	std::string tasks_temp;
	std::string special_kill_temp;
	char sz_temp[256];
	char sz_temp_1[128];
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			record_temp += ";";
		}
		message::MsgIntPair pair_entry =  _info.records(i);
		sprintf(sz_temp, "%d,%d", pair_entry.number_1(), pair_entry.number_2());
		record_temp += sz_temp;
	}

	length = _info.heroes_size();
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			heroes_temp += ";";
		}

		int number_ = 0;
		bool b_temp =  _info.heroes(i);
		if (b_temp)
		{
			number_ = 1;
		}
		sprintf(sz_temp, "%d", number_);
		heroes_temp += sz_temp;
	}

	length = _info.tasks_size();
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0)
		{
			tasks_temp += ";";
		}
		message::MsgTaskInfo info_entry = _info.tasks(i);
		sprintf(sz_temp, "%d,%d,%d", info_entry.taskid(), info_entry.argument_1(), info_entry.usetime());
		tasks_temp += sz_temp;
	}

	SPECIALKILLS::iterator it = _special_kills.begin();
	special_kill_temp.clear();
	for (; it != _special_kills.end(); ++ it)
	{
		std::string temp_str;
		std::vector<message::MsgObjConfig>::iterator it_config = it->second.begin();
		for (; it_config != it->second.end(); ++ it_config)
		{
			temp_str += ":";			
			sprintf(sz_temp_1, "%d,%d", (*it_config).id(), (int)(*it_config).type());
			temp_str += sz_temp_1;
		}

		if (temp_str.empty() == false)
		{
			if (special_kill_temp.empty() == false)
			{
				special_kill_temp += ";";
			}
			sprintf(sz_temp, "%d,%d", it->first.first, it->first.second);
			special_kill_temp += sz_temp;
			special_kill_temp += temp_str;
		}
	}
	char temp[4096];
	std::string last_task_advertisement_time_temp;


	build_unix_time_to_string(_last_task_advertisement_time, last_task_advertisement_time_temp);
//#ifdef WIN32
	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,`special_kill`,\
		`current_hero`, `current_chapter`, `current_section`, `complete_task_count`,\
		 `free_task_count`,`last_task_advertisement_time`,`gm_level`, `current_task_count`) values \
		(%llu, 'normal', %d, '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s', %d, %d);",
		_account, _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), 
		special_kill_temp.c_str(), _info.current_hero(), _current_chapter,
		_current_section, _info.complete_task_count(), _current_task_count,
		last_task_advertisement_time_temp.c_str(), _gm_level, _current_task_count);
//#else
//	sprintf(temp, "replace into `character`(`account_id`, `name`, `gold`, `record_his`, `heroes_state`, `tasks`,\
//		`current_hero`, `current_chapter`, `current_section`, `current_gold`, `complete_task_count`, `free_task_count`,`last_task_advertisement_time`) values \
//		(%llu, 'normal', %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, '%s');",
//		_account, _info.gold(), record_temp.c_str(), heroes_temp.c_str(), tasks_temp.c_str(), _info.current_hero(), _current_chapter,
//		_current_section, _info.gold(), _info.complete_task_count(), _current_task_count, last_task_advertisement_time_temp.c_str());
//#endif


	message::MsgSaveDataGS2DB msg_db;
	msg_db.set_sql(temp);
	gGSDBClient.sendPBMessage(&msg_db, _session->getTranId());

}

void DreamHero::set_name(const char* name)
{
	_info.set_name(name);
}


void DreamHero::ReqGoldShopConfigs()
{
	message::MsgS2CGoldShopConfigsACK msg;
	const MAPGOLDSHOPCONFIGINFOS* infos = gGameConfig.getGoldShopConfigInfos();
	MAPGOLDSHOPCONFIGINFOS::const_iterator it = infos->begin();
	for (; it != infos->end(); ++ it)
	{
		message::MsgGoldShopConfigInfo* entry_info = msg.add_infos();
		entry_info->CopyFrom(it->second);
	}
	sendPBMessage(&msg);
}

const char* DreamHero::get_name()
{
	return _info.name().c_str();
}

void DreamHero::sendPBMessage(google::protobuf::Message* p)
{
	if (_session)
	{
		_session->sendPBMessage(p);
	}
	
}
