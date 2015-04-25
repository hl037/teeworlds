#ifndef ACCOUNTSYSTEM_H
#define ACCOUNTSYSTEM_H

#define ACCSYS_FORMAT "[AccSys] "

enum
{
   MAX_USER_ACC_LENGTH = 32 + 128 + 2, // username + AP domain + @ + \0
	MAX_TOK_PROVIDER_LENGTH = 128 + 1, // AP domain + \0
	TOKEN_LENGTH = 128,
	CONST_ID_LENGTH = 10, // 8 + 2 (to fit with Base32)
};
   
#endif // ACCOUNTSYSTEM_H

