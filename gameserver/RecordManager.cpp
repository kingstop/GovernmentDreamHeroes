#include "stdafx.h"
#include "RecordManager.h"


RecordManager::RecordManager()
{
	
	_sql_head[RecordTypeEnterGame] = "insert into `enter_game_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `record_time`) values ";
	_sql_head[RecordTypeLeaveGame] = "insert into `leave_game_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `sucess`, `gold`, `record_time`) values";
	_sql_head[RecordTypeTaskComplete] = "insert into `task_complete_record`(`account_id`, `nick_name`, `chapter_id`, `section_id`, `task_id`, `gold`, `record_time`) values";
	_sql_head[RecordTypeTaskAccepte] = "insert into `task_accepte_record`(`account_id`, `nick_name`, `task_id`, `record_time`) values";
	_sql_head[RecordTypeTaskGiveUp] = "insert into `task_give_up_record`(`account_id`, `nick_name`, `task_id`, `record_time`) values";
	_sql_head[RecordTypeChapterUnlock] = "insert into `chapter_unlock_record`(`account_id`, `nick_name`, `chapter_id`, `gold`, `record_time`) values";
	_sql_head[RecordTypeGoldModify] = "insert into `modify_gold_record`(`account_id`, `nick_name`, `gold`, `modify_type`, `record_time`) values";
	_sql_head[RecordTypeBuyHero] = "insert into `buy_hero_record`(`account_id`, `nick_name`, `grid_hero`, `gold`, `record_time`) values";
}


RecordManager::~RecordManager()
{
}

const char* RecordManager::getCurTime()
{
	build_unix_time_to_string(g_server_time, _cur_time);
	return _cur_time.c_str();
}

void RecordManager::enterGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, '%s')", acc, nick_name, chapter_id, section_id, getCurTime());
	_record[RecordTypeEnterGame].push_back(_szTemp);
}

void RecordManager::leaveGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, bool success, int gold)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, %d, %d,'%s')", acc, nick_name, chapter_id, section_id,success , gold, getCurTime());
	_record[RecordTypeLeaveGame].push_back(_szTemp);
}

void RecordManager::taskCompleteRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int task_id, int gold)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, %d, %d, '%s')", acc, nick_name, chapter_id, section_id, task_id, gold, getCurTime());
	_record[RecordTypeTaskComplete].push_back(_szTemp);
}

void RecordManager::taskAccepteRecordRecord(account_type acc, const char* nick_name, int task_id)
{
	sprintf(_szTemp, "(%lu, '%s', %d, '%s')", acc, nick_name, task_id, getCurTime());
	_record[RecordTypeTaskAccepte].push_back(_szTemp);
}

void RecordManager::taskGiveUpRecord(account_type acc, const char* nick_name, int task_id)
{
	sprintf(_szTemp, "(%lu, '%s', %d, '%s')", acc, nick_name, task_id, getCurTime());
	_record[RecordTypeTaskGiveUp].push_back(_szTemp);
}
void RecordManager::chapterUnlockRecord(account_type acc, const char* nick_name, int chapter_id, int gold)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, '%s')", acc, nick_name, chapter_id, gold, getCurTime());
	_record[RecordTypeChapterUnlock].push_back(_szTemp);
}
void RecordManager::buyHeroRecord(account_type acc, const char* nick_name, int grid, int gold)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, '%s')", acc, nick_name, grid, gold, getCurTime());
	_record[RecordTypeBuyHero].push_back(_szTemp);
}
void RecordManager::goldModifyRecord(account_type acc, const char* nick_name, int gold, GoldModifyType en)
{
	sprintf(_szTemp, "(%lu, '%s', %d, %d, '%s')", acc, nick_name, gold, (int)en, getCurTime());
	_record[RecordTypeGoldModify].push_back(_szTemp);
}

void RecordManager::update()
{
	int max_count = 10;
	for (size_t i = RecordTypeEnterGame; i < RecordTypeMax; i++)
	{		
		RECORDS::iterator it = _record[i].begin();
		for (int j = 0; it != _record[i].end(); ++ it, j ++)
		{

		}
	}
}