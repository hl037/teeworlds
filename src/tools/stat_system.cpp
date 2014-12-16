/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <base/system.h>

int main(int argc, const char **argv)
{
	dbg_logger_stdout();
	dbg_msg("Stat system", "Hello world");

	return 0;
}
