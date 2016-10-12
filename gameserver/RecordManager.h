#pragma once
enum RecordType
{
	RecordTypeEnterGame,
	RecordTypeLeaveGame,
	RecordTypeTaskComplete,
	RecordTypeTaskAccepte,
	RecordTypeTaskGiveUp,
	RecordTypeChapterUnlock,
	RecordTypeGoldModify,
	RecordTypeBuyHero,
	RecordTypeMax
};


class RecordManager
{
public:
	typedef std::vector<std::string> RECORDS;
	enum GoldModifyType
	{
		GoldModify_LeaveGame,
		GoldModify_CompleteTask,
		GoldModify_UnlockChapter,
		GoldModify_BuyHero,
	};
public:
	RecordManager();
	virtual ~RecordManager();
public:
	void enterGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id);
	void leaveGameRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, bool success, int gold);
	void taskCompleteRecord(account_type acc, const char* nick_name, int chapter_id, int section_id, int task_id, int gold);
	void taskAccepteRecordRecord(account_type acc, const char* nick_name, int task_id);
	void taskGiveUpRecord(account_type acc, const char* nick_name, int task_id);
	void chapterUnlockRecord(account_type acc, const char* nick_name, int chapter_id, int gold);
	void buyHeroRecord(account_type acc, const char* nick_name, int grid, int gold);
	void goldModifyRecord(account_type acc, const char* nick_name, int gold, GoldModifyType en);
	const char* getCurTime();
	void update();
public:
	RECORDS _record[RecordTypeMax];
	std::string _sql_head[RecordTypeMax];
	char _szTemp[512];
	std::string _cur_time;
};

