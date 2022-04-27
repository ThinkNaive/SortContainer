#include "pch.h"
#include <MC/Block.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Container.hpp>
#include <MC/ServerPlayer.hpp>
#include <unordered_set>
#include <regCommandAPI.h>
#include "ItemsQueue.h"
#include "EventHandler.h"

using namespace::std;

unordered_set<string> containerNames{
	"minecraft:barrel",
	"minecraft:shulker_box",
	"minecraft:undyed_shulker_box",
	"minecraft:trapped_chest",
	"minecraft:chest",
	"minecraft:ender_chest"
};

unordered_set<string> magicWandNames = {
	"minecraft:spyglass"
};

bool onPlayerUseItemOn(Event::PlayerUseItemOnEvent ev) {
	if (!ev.mBlockInstance.getBlock()->isContainerBlock()) return true;

	string blockName = ev.mBlockInstance.getBlock()->getTypeName();
	string itemName = ev.mItemStack->getTypeName();

	if (containerNames.find(blockName) == containerNames.end()
		|| magicWandNames.find(itemName) == magicWandNames.end())
		return true;

	Container* container = nullptr;

	if (blockName == "minecraft:ender_chest") {
		container = ev.mPlayer->getEnderChestContainer();
	}
	else {
		container = ev.mBlockInstance.getContainer();
	}

	ItemsQueue iq(container);

	if (iq.sort()) {
		ev.mPlayer->sendText("Order!");
	}
	else {
		ev.mPlayer->sendText("What a pity!");
	}

	return true;
}

bool onRegCmdEvent(Event::RegCmdEvent e) {
	SortCommand::setup(e.mCommandRegistry);
	return true;
}

void SortCommand::execute(CommandOrigin const& ori, CommandOutput& outp) const {
	ServerPlayer* wp = ori.getPlayer();

	if (wp) {
		Container* container = &wp->getInventory();
		ItemsQueue iq(container, 9);

		if (iq.sort()) {
			outp.success("Order!");
		}
		else {
			outp.error("What a pity!");
		}
	}
	else {
		outp.error("[SortContainer] You are not a player!");
	}
}

void SortCommand::setup(CommandRegistry* registry)
{
	registry->registerCommand("sort", "Sort Inventory.", CommandPermissionLevel::Any,
		{ (CommandFlagValue)0 }, { (CommandFlagValue)0x80 });
	registry->registerOverload<SortCommand>("sort");
}

