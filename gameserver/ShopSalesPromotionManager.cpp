#include "stdafx.h"
#include "ShopSalesPromotionManager.h"


ShopSalesPromotionManager::ShopSalesPromotionManager()
{
}


ShopSalesPromotionManager::~ShopSalesPromotionManager()
{
}

void ShopSalesPromotionManager::init()
{
	const MAPTIMESHOPSALESPROMOTIONCONFIGS* map_sales_promotions = gGameConfig.getTimeShopSalesPromotionConfigs();
	MAPTIMESHOPSALESPROMOTIONCONFIGS::const_iterator it = map_sales_promotions->begin();
	for (; it != map_sales_promotions->end(); ++ it)
	{
		TimeShopPromotion* entry = new TimeShopPromotion();
		if (entry->init(it->first, this))
		{
			_shopSalesPromotions.push_back(entry);
		}
		else
		{
			delete entry;
		}		
	}
}


void ShopSalesPromotionManager::update()
{
	std::list<ShopSalesPromotion*>::iterator it = _shopSalesPromotions.begin();
	while (it != _shopSalesPromotions.end())
	{
		ShopSalesPromotion* entry = (*it);
		if (entry->getType() == ShopSalesPromotionType_Destroy)
		{
			it = _shopSalesPromotions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void ShopSalesPromotionManager::modifyCheapGold(int grid, int cheap_gold)
{
	std::map<int, int>::iterator it =  _cheap_gold.find(grid);
	if (it == _cheap_gold.end())
	{
		_cheap_gold.insert(std::map<int, int>::value_type(grid,0));
	}
	int number = _cheap_gold[grid];
	number += cheap_gold;
	_cheap_gold[grid] = number;
}

int ShopSalesPromotionManager::getCheapGold(int grid)
{
	int cheap_gold = 0;
	std::map<int, int>::iterator it = _cheap_gold.find(grid);
	if (it != _cheap_gold.end())
	{
		cheap_gold = it->second;
	}
	return cheap_gold;
}

ShopSalesPromotionType ShopSalesPromotion::getType()
{
	return _type;
}

ShopSalesPromotion::ShopSalesPromotion()
{

}

ShopSalesPromotion::~ShopSalesPromotion()
{

}

void ShopSalesPromotion::apply()
{
	if (gEventMgr.hasEvent(this, EVENT_SHOP_SALES_PROMOTION_APPLY))
	{
		gEventMgr.removeEvents(this, EVENT_SHOP_SALES_PROMOTION_APPLY);
	}
	_type = ShopSalesPromotionType_Apply;
}

void ShopSalesPromotion::exit()
{
	if (gEventMgr.hasEvent(this, EVENT_SHOP_SALES_PROMOTION_EXIT))
	{
		gEventMgr.removeEvents(this, EVENT_SHOP_SALES_PROMOTION_EXIT);
	
	}
	_type = ShopSalesPromotionType_Wait;
}

bool ShopSalesPromotion::init(int id, ShopSalesPromotionManager* parent)
{
	_id = id;
	_parent = parent;
	_type = ShopSalesPromotionType_Wait;
	return true;
}


TimeShopPromotion::TimeShopPromotion()
{

}

TimeShopPromotion::~TimeShopPromotion()
{

}

bool TimeShopPromotion::init(int id, ShopSalesPromotionManager* parent)
{
	ShopSalesPromotion::init(id, parent);
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	if (config_entry == NULL)
	{
		return false;
	}

	if (g_server_time > config_entry->end_time_ ||config_entry->end_time_ <= config_entry->begin_time_)
	{
		return false;
	}

	s64 delay_time = config_entry->begin_time_ - g_server_time;
	if (delay_time <= 0)
	{
		apply();
	}
	else
	{		
		gEventMgr.addEvent(this, &TimeShopPromotion::apply, EVENT_SHOP_SALES_PROMOTION_APPLY, delay_time * 1000, 1, 0);
	}

	return true;
	
}

void TimeShopPromotion::exit()
{
	ShopSalesPromotion::exit();
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);
	_parent->modifyCheapGold(config_entry->grid_id_, -config_entry->cheap_gold_);
	_type = ShopSalesPromotionType_Destroy;
}

void TimeShopPromotion::apply()
{
	ShopSalesPromotion::apply();
	const TimeShopSalesPromotionConfig* config_entry = gGameConfig.getTimeShopSalesPromotionConfig(_id);	
	_parent->modifyCheapGold(config_entry->grid_id_, config_entry->cheap_gold_);
	s64 delay_time = g_server_time - config_entry->end_time_;

	if (delay_time > 0)
	{
		gEventMgr.addEvent(this, &TimeShopPromotion::apply, EVENT_SHOP_SALES_PROMOTION_EXIT, delay_time * 1000, 1, 0);
	}
	else
	{
		exit();
	}


}