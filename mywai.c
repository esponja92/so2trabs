#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	struct passwd *pws;
	pws = getpwuid(geteuid());
	printf("%s \n", pws->pw_name);
	return 0;
}

