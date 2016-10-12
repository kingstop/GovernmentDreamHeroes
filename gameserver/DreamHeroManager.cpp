#include "stdafx.h"
#include "DreamHeroManager.h"
#include "DreamHero.h"
#define _SAVE_COLLECT_TIME_  (10 * _TIME_SECOND_MSEL_)
DreamHeroManager::DreamHeroManager()
{

}


DreamHeroManager::~DreamHeroManager()
{
}

void DreamHeroManager::init()
{
	_save_all_heroes_ok = false;
	if (gEventMgr.hasEvent(this, EVENT_COLLECT_INFO_) == false)
	{
		gEventMgr.addEvent(this, &DreamHeroManager::CollectInfo, EVENT_COLLECT_INFO_, _SAVE_COLLECT_TIME_, -1, 0);
	}
	
	if (gEventMgr.hasEvent(this, EVENT_PER_HOUR) == false)
	{
		time_t server_time = g_server_start_time;
		
		tm* p1 = localtime(&server_time);
		int next_hour_second = p1->tm_min * 60 + p1->tm_sec;
		gEventMgr.addEvent(this, &DreamHeroManager::eventPerHour, EVENT_PER_HOUR, next_hour_second, -1, 0);


	}
}

void DreamHeroManager::eventPerHour()
{
	time_t server_time = g_server_time;
	tm* p1 = localtime(&server_time);
	int next_hour_second = p1->tm_min * 60 + p1->tm_sec;
	gEventMgr.modifyEventTime(this, EVENT_PER_HOUR, next_hour_second);
	
	if (p1->tm_hour == gGameConfig.getGlobalConfig().day_Refresh_time_)
	{
		dayRefresh();
	}
}


void DreamHeroManager::dayRefresh()
{
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end() ; ++ it)
	{
		it->second->dayRefresh();
	}
}

DreamHero* DreamHeroManager::GetHero(account_type account)
{
	DreamHero* hero = NULL;
	MAPHEROS::iterator it = _heroes.find(account);
	if (it != _heroes.end())
	{
		hero = it->second;
		hero->set_parent(this);
		//hero->StopDestroyClock();
	}
	
	
	return hero;
}

DreamHero* DreamHeroManager::CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		const message::MsgHeroData* entry_msg = HeroDataMsg->mutable_data();
		hero->set_info(HeroDataMsg);
		hero->set_account(acc);
		_heroes.insert(MAPHEROS::value_type(hero->get_account(), hero));
	}	
	hero->set_session(session);
	hero->set_parent(this);
	hero->StopDestroyClock();
	return hero;
}

DreamHero* DreamHeroManager::CreateHero(account_type acc, Session* session)
{
	DreamHero* hero = GetHero(acc);
	if (hero == NULL)
	{
		hero = new DreamHero();
		hero->LoadFromConfig();
	}
	hero->set_session(session);
	hero->set_account(acc);
	hero->set_parent(this);
	hero->StopDestroyClock();
	_heroes.insert(MAPHEROS::value_type(hero->get_account(), hero));
	return hero;
}



void DreamHeroManager::SaveDreamHeroes()
{
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* hero = it->second;
		hero->SaveHero();
	}
	Mylog::log_server(LOG_INFO, "game server save all heroes count[%d]!", _heroes.size());
	message::MsgSaveAllHeroesGS2DB msg;
	gGSDBClient.sendPBMessage(&msg, 0);	
}


void DreamHeroManager::DestroyHero(DreamHero* p)
{
	MAPHEROS::iterator it = _heroes.find(p->get_account());
	if (it != _heroes.end())
	{
		DreamHero* p_temp = it->second;
		if (p_temp == p)
		{
			delete p_temp;
			_heroes.erase(it);
		}
		else
		{
			//need log;
		}
	}
	else
	{
		//need log;
	}
}

void DreamHeroManager::CollectInfo()
{
	int offline_count = 0;
	int online_count = 0;
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* hero = it->second;
		if (hero)
		{
			if (hero->is_online())
			{
				online_count++;
			}
			else
			{
				offline_count++;
			}			
		}
	}
	int current_player = offline_count + online_count;

	//Mylog::log_server(LOG_INFO, "current player count[%d], online player count[%d], offline player count[%d]", current_player, online_count, offline_count);
}

DreamHero* DreamHeroManager::GetHeroByName(const char* name)
{
	DreamHero* hero = NULL;
	MAPHEROS::iterator it = _heroes.begin();
	for (; it != _heroes.end(); ++ it)
	{
		DreamHero* temp_hero = it->second;
		if (temp_hero->get_name() == name)
		{
			hero = temp_hero;
			break;
		}
	}
	//hero->StopDestroyClock();
	return hero;
}

bool DreamHeroManager::is_save_all_heroes_ok()
{
	return _save_all_heroes_ok;
}

void DreamHeroManager::save_all_heroes_ok()
{
	_save_all_heroes_ok = true;
}

