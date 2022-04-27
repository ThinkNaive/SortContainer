#include "pch.h"
#include <EventAPI.h>
#include <LoggerAPI.h>
#include <MC/Level.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Actor.hpp>
#include <MC/Player.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Container.hpp>
#include <LLAPI.h>
#include <unordered_set>
#include "ItemsQueue.h"
#include "EventHandler.h"

using namespace::std;

void PluginInit()
{
	Event::PlayerUseItemOnEvent::subscribe(onPlayerUseItemOn);
	Event::RegCmdEvent::subscribe(onRegCmdEvent);
}
