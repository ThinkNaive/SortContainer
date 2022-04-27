#include "pch.h"
#include <MC/Container.hpp>

typedef struct Node {
	int auxID;
	int count;
	int maxStackSize;
	ItemStack* item;
	struct Node* next;
} Node;

class ItemsQueue {
public:
	ItemsQueue();
	ItemsQueue(Container* container, int skipIndex = 0);
	~ItemsQueue();
	bool sort();
	void log();

private:
	Node* head;
	bool ifInit;
	Container* con;
	int skipIndex;
	vector<vector<int>> getItemsInfo(Container* container);
	static bool compare(const vector<int> a, const vector<int> b);
	void arrangeItemsByAuxID(vector<vector<int>>& itemsInfo);
	int countItems();
};