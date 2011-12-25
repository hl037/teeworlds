/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#ifndef GAME_SERVER_DDRACECOMMANDS_H
#define GAME_SERVER_DDRACECOMMANDS_H
#undef GAME_SERVER_DDRACECOMMANDS_H // this file can be included several times

#ifndef CHAT_COMMAND
#define CHAT_COMMAND(name, params, flags, callback, userdata, help)
#endif

CHAT_COMMAND("credits", "", CFGFLAG_CHAT|CFGFLAG_SERVER, ConCredits, this, "Shows the credits of the DDRace mod")
CHAT_COMMAND("emote", "?si", CFGFLAG_CHAT|CFGFLAG_SERVER, ConEyeEmote, this, "Sets your tee's eye emote")
CHAT_COMMAND("eyeemote", "", CFGFLAG_CHAT|CFGFLAG_SERVER, ConEyeEmote, this, "Toggles use of standard eye-emotes on/off")
CHAT_COMMAND("settings", "?s", CFGFLAG_CHAT|CFGFLAG_SERVER, ConSettings, this, "Shows gameplay information for this server")
CHAT_COMMAND("help", "?r", CFGFLAG_CHAT|CFGFLAG_SERVER, ConHelp, this, "Shows help to command r, general help if left blank")
CHAT_COMMAND("info", "", CFGFLAG_CHAT|CFGFLAG_SERVER, ConInfo, this, "Shows info about this server")
CHAT_COMMAND("me", "r", CFGFLAG_CHAT|CFGFLAG_SERVER, ConMe, this, "Like the famous irc command '/me says hi' will display '<yourname> says hi'")
CHAT_COMMAND("pause", "", CFGFLAG_CHAT|CFGFLAG_SERVER, ConTogglePause, this, "Toggles pause on/off (if activated on server)")
CHAT_COMMAND("rank", "?r", CFGFLAG_CHAT|CFGFLAG_SERVER, ConRank, this, "Shows the rank of player with name r (your rank by default)")
CHAT_COMMAND("rules", "", CFGFLAG_CHAT|CFGFLAG_SERVER, ConRules, this, "Shows the server rules")
CHAT_COMMAND("team", "?i", CFGFLAG_CHAT|CFGFLAG_SERVER, ConJoinTeam, this, "Lets you join team i (shows your team if left blank)")
CHAT_COMMAND("top5", "?i", CFGFLAG_CHAT|CFGFLAG_SERVER, ConTop5, this, "Shows five ranks of the ladder beginning with rank i (1 by default)")
CHAT_COMMAND("showothers", "?i", CFGFLAG_CHAT|CFGFLAG_SERVER, ConShowOthers, this, "Whether to showplayers from other teams or not (off by default), optional i = 0 for off else for on")


/* START iDDRace chat commands */

//rescue
CHAT_COMMAND("r", "", CFGFLAG_CHAT, ConRescue, this, "/r-rescue")
CHAT_COMMAND("rescue", "", CFGFLAG_CHAT, ConRescue, this, "/r-rescue")

//dummy commands
CHAT_COMMAND("dummy", "", CFGFLAG_CHAT, ConDummy, this, "Creates your own Dummy")
CHAT_COMMAND("dummy_del", "", CFGFLAG_CHAT, ConDummyDelete, this, "Deletes your Dummy")
CHAT_COMMAND("dummy_change", "", CFGFLAG_CHAT, ConDummyChange, this, "Tele Dummy to you, and you where Dummy was before")
CHAT_COMMAND("dummy_hammer", "", CFGFLAG_CHAT, ConDummyHammer, this, "You Dummy does hammerFly")
CHAT_COMMAND("control_dummy", "", CFGFLAG_CHAT, ConDummyControl, this, "Go to spectators and rule your dummy")
CHAT_COMMAND("dummy_copy_move", "", CFGFLAG_CHAT, ConDummyCopyMove, this, "Dummy copies all your movement. Smth like multiclient.")


//short dummy commands
CHAT_COMMAND("d", "", CFGFLAG_CHAT, ConDummy, this, "Creates your own Dummy")
CHAT_COMMAND("dd", "", CFGFLAG_CHAT, ConDummyDelete, this, "Deletes your Dummy")
CHAT_COMMAND("dc", "", CFGFLAG_CHAT, ConDummyChange, this, "Tele Dummy to you, and you where Dummy was before")
CHAT_COMMAND("dh", "", CFGFLAG_CHAT, ConDummyHammer, this, "You Dummy does hammerFly")
CHAT_COMMAND("cd", "", CFGFLAG_CHAT, ConDummyControl, this, "Go to spectators and rule your dummy")
CHAT_COMMAND("dcm", "", CFGFLAG_CHAT, ConDummyCopyMove, this, "Dummy copies all your movement. Smth like multiclient.")

/* END   iDDRace chat commands */


#if defined(CONF_SQL)
CHAT_COMMAND("times", "?s?i", CFGFLAG_CHAT|CFGFLAG_SERVER, ConTimes, this, "/times ?s?i shows last 5 times of the server or of a player beginning with name s starting with time i (i = 1 by default)")
#endif
#undef CHAT_COMMAND

#endif