#include "pch.h"
#include <MC/ItemStack.hpp>
#include <MC/Container.hpp>
#include <algorithm>
#include <cstdlib>
#include "ItemsQueue.h"
#include <LoggerAPI.h>

using namespace::std;

Logger logger("SortContainer");

ItemsQueue::ItemsQueue()
{
	head = (Node*)malloc(sizeof(Node));
	if (head) {
		head->auxID = 0;
		head->count = 0;
		head->maxStackSize = 0;
		head->next = nullptr;
		head->item = nullptr;
	}
	ifInit = false;
	con = nullptr;
	skipIndex = 0;
}

ItemsQueue::ItemsQueue(Container* container, int skipIndex)
{
	con = container;
	this->skipIndex = skipIndex;
	ifInit = false;
	head = (Node*)malloc(sizeof(Node));
	Node* tail = nullptr;
	vector<vector<int>> itemsInfo = this->getItemsInfo(con);
	this->arrangeItemsByAuxID(itemsInfo);

	if (head) {
		head->auxID = 0;
		head->count = 0;
		head->maxStackSize = 0;
		head->next = nullptr;
		head->item = nullptr;
		tail = head;
	}
	else {
		logger.warn("Allocating memory for (Node*)head error in ItemsQueue::ItemsQueue");
		return;
	}

	for (vector<int> info : itemsInfo) {
		if (tail != nullptr) {
			tail->next = (Node*)malloc(sizeof(Node));
			tail = tail->next;

			if (!tail) {
				logger.warn("Allocating memory for (Node*)tail error in ItemsQueue::ItemsQueue");
				return;
			}

			tail->auxID = info[0];
			tail->count = info[1];
			tail->maxStackSize = info[2];
			tail->item = con->getItem(info[3]).clone_s();
			tail->next = nullptr;
		}
	}

	ifInit = true;
}

ItemsQueue::~ItemsQueue()
{
	ifInit = false;

	while (head) {
		Node* temp = head;
		head = head->next;
		if (temp->item) {
			delete temp->item;
			temp->item = nullptr;
		}
		free(temp);
	}
}

bool ItemsQueue::sort()
{
	if (!ifInit || !head) return false;
	Node* prev = head;
	Node* curr = head->next;
	int countBeforeSort = countItems();

	while (curr) {
		while (curr && (curr->auxID == 0 || curr->count == 0)) {
			prev->next = curr->next;
			if (curr->item) {
				delete curr->item;
				curr->item = nullptr;
			}
			free(curr);
			curr = prev->next;
		}

		if (curr && curr->next) {
			if (curr->maxStackSize <= 1 || curr->auxID != curr->next->auxID || curr->count >= curr->maxStackSize) {
				prev = curr;
				curr = curr->next;
			}
			else {
				if (curr->count + curr->next->count <= curr->maxStackSize) {
					int addend = curr->next->count;
					curr->item->add(addend);
					curr->next->item->remove(addend);
					curr->count += addend;
					curr->next->count -= addend;

					Node* temp = curr->next;
					curr->next = temp->next;
					if (temp->item) {
						delete temp->item;
						temp->item = nullptr;
					}
					free(temp);
				}
				else {
					int addend = curr->maxStackSize - curr->count;
					curr->item->add(addend);
					curr->next->item->remove(addend);
					curr->count += addend;
					curr->next->count -= addend;

					prev = curr;
					curr = curr->next;
				}
			}
		}
		else {
			break;
		}
	}

	int countAfterSort = countItems();
	if (countBeforeSort != countAfterSort) return false;

	int size = con->getContainerSize();
	Node* p = head->next;
	ItemStack* nullItem = ItemStack::create();

	for (int i = skipIndex; i < size; ++i) {
		if (p) {
			con->setItem(i, *p->item);
			p = p->next;
		}
		else {
			con->setItem(i, *nullItem);
		}
	}

	delete nullItem;
	return true;
}

void ItemsQueue::log()
{
	if (!ifInit) return;
	Node* p = head->next;
	string info = "";

	while (p) {
		if (p->auxID != 0 && p->count > 0) {
			info += "ID" + to_string(p->auxID) + "#" + to_string(p->count) + " ";
		}

		p = p->next;
	}

	logger.info(info);
}

vector<vector<int>> ItemsQueue::getItemsInfo(Container* container)
{
	int size = container->getContainerSize();
	vector<vector<int>> info;

	for (int i = skipIndex; i < size; ++i) {
		vector<int> e;
		e.emplace_back(container->getItem(i).getIdAux());
		e.emplace_back(container->getItem(i).getCount());
		e.emplace_back(container->getItem(i).getMaxStackSize());
		e.emplace_back(i);
		info.emplace_back(e);
	}

	return info;
}

bool ItemsQueue::compare(const vector<int> a, const vector<int> b)
{
	return a[0] < b[0];
}

void ItemsQueue::arrangeItemsByAuxID(vector<vector<int>>& itemsInfo)
{
	std::sort(itemsInfo.begin(), itemsInfo.end(), compare);
}

int ItemsQueue::countItems()
{
	if (!ifInit) return 0;
	Node* p = head->next;
	int count = 0;

	while (p) {
		if (p->auxID != 0 && p->count > 0) {
			count += p->count;
		}

		p = p->next;
	}

	return count;
}
