#include <EventAPI.h>
#include <regCommandAPI.h>

bool onPlayerUseItemOn(Event::PlayerUseItemOnEvent ev);
bool onRegCmdEvent(Event::RegCmdEvent e);

class SortCommand : public Command {
public:
	void execute(CommandOrigin const& ori, CommandOutput& outp) const;
	static void setup(CommandRegistry* registry);
};