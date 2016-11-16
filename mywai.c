#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <grp.h>

int main(int argc, char** argv)
{
	int opt;
	bool show_user = false;
	bool show_group = false;
	bool show_all_groups = false;
	bool show_id = false;

	while ((opt = getopt(argc, argv, "ugiG")) != -1) {
        switch (opt) {
	        case 'u':
	        	show_user = true;
	        	break;
	        case 'g':
	        	show_group = true;
	        	break;
	        case 'G':
	        	show_all_groups = true;
	        	break;
	        case 'i':
	        	show_id = true;
	        	break;
	        default:
	            printf("Uso correto: %s [-ugiG]\n", argv[0]);
	            return (-1);
        }
    }


	struct passwd *pws;
	uid_t uid;
    struct group *grp;
	gid_t gid, *groups_list;

	int groups_found;
    

	if ((show_user && show_group) ||
		(show_user && show_all_groups) ||
		(show_group && show_all_groups)) {
		printf("Opções [-ugG] são mutuamente exclusivas\n");
		return -1;
	}

	if (show_user) {
		uid = getuid();
		if (uid == -1) {
			printf("ID de usuario nao encontrado\n");
			return -1;
		}
		if (!show_id) {
			pws = getpwuid(uid);
			if (pws != NULL) {
				printf("%s \n", pws->pw_name);
				return 0;
			} else {
				printf("Erro, motivo: %s\n", strerror(errno));
				return -1;
			}
		} else {
			printf("%d \n", uid);
			return 0;
		}
	}

	if (show_group) {
		gid = getgid();
		if (gid == -1) {
			printf("ID de grupo nao encontrado\n");
			return -1;
		}
		if (!show_id) {
			grp = getgrgid(gid);
			if (grp != NULL) {
				printf("%s \n", grp->gr_name);
				return 0;
			} else {
				printf("Erro, motivo: %s\n", strerror(errno));
				return -1;
			}
		} else {
			printf("%d \n", gid);
			return 0;
		}
	}

	if (show_all_groups) {
		uid = getuid();
		if (uid == -1) {
			printf("ID de usuario nao encontrado\n");
			return -1;
		}

		gid = getgid();
		if (gid == -1) {
			printf("ID de grupo nao encontrado\n");
			return -1;
		}
		pws = getpwuid(uid);
		// grp = getgrgid(gid);

		if (pws != NULL) {
			groups_list = (gid_t*)malloc(sizeof(gid_t) * sysconf(_SC_NGROUPS_MAX) + 1);
			groups_found = getgroups(sysconf(_SC_NGROUPS_MAX) + 1, groups_list);
			if (groups_found == -1)
			{
				printf("Erro, motivo: %s\n", strerror(errno));
				return -1;
			}

			int i;
			for (i = 0; i < groups_found; ++i)
			{
				grp = getgrgid(groups_list[i]);
				if (grp != NULL)
					printf("%s ", grp->gr_name);
			}
			printf("\n");
			return 0;
		} else {
			printf("Erro, motivo: %s\n", strerror(errno));
			return -1;
		}
	}

}

