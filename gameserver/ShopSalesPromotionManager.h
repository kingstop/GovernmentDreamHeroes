#pragma once
#include <iostream>
#include <map>
#include <vector>

class ShopSalesPromotion;

class ShopSalesPromotionManager : public EventableObject
{
	
public:
	ShopSalesPromotionManager();
	virtual ~ShopSalesPromotionManager();
	void init();
	void modifyCheapGold(int grid, int cheap_gold);
	int getCheapGold(int grid);
	void update();
protected:
	std::map<int, int> _cheap_gold;
	std::list<ShopSalesPromotion*> _shopSalesPromotions;
	
	
};

enum ShopSalesPromotionType
{
	ShopSalesPromotionType_Wait,
	ShopSalesPromotionType_Apply,
	ShopSalesPromotionType_Destroy
};

class ShopSalesPromotion : public EventableObject
{
public:
	ShopSalesPromotion();
	virtual ~ShopSalesPromotion();
public:
	virtual bool init(int id, ShopSalesPromotionManager* parent);
	virtual void apply();
	virtual void exit();
	ShopSalesPromotionType getType();
protected:
	ShopSalesPromotionManager* _parent;
	int _id;
	ShopSalesPromotionType _type;
};


class TimeShopPromotion : public ShopSalesPromotion
{
public:
	TimeShopPromotion();
	~TimeShopPromotion();
public:
	bool init(int id, ShopSalesPromotionManager* parent);
	void apply();
	void exit();

private:
	
};

