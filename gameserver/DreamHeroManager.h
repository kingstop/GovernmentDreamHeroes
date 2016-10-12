#pragma once

class DreamHero;
class DreamHeroManager : public EventableObject
{
public:
	typedef std::map<account_type, DreamHero*> MAPHEROS;
	typedef std::map<std::string, account_type> MAPSTRACCPAIR;
public:
	DreamHeroManager();
	virtual ~DreamHeroManager();
	void init();	
	DreamHero* CreateHero(message::MsgHeroDataDB2GS* HeroDataMsg, account_type acc, Session* session);
	DreamHero* CreateHero(account_type acc, Session* session);
	DreamHero* GetHeroByName(const char* name);
	DreamHero* GetHero(account_type account);
	void DestroyHero(DreamHero* p);
	void SaveDreamHeroes();
	bool is_save_all_heroes_ok();
	void save_all_heroes_ok();
	void CollectInfo();
protected:
	
	void eventPerHour();
	void dayRefresh();
protected:
	MAPHEROS _heroes;

	bool _save_all_heroes_ok;
};

