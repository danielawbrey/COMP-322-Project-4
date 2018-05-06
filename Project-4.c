#include <stdio.h>
#include <pwd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <grp.h>

bool isDir(const char*);
bool isFile(const char*);
char* getBaseName(const char*);
char* getPathName(const char*);
bool pathIsRelative(const char*);

#define true 1
#define false 0

int main(int argc, char **argv) {
	
	int errors, k, perm;
	struct stat buf;
	struct passwd *pwd;
	struct group grp;
	struct dirent *direntPtr;
	char *pPtr, *userDir, *filePath, *homePath, *completePath, *temp;	
	DIR *dirPtr;
	char date[10];
	struct tm *time;
	char cwd[4096];

	if(argc == 1) { printf("No input files found.\n"); }
	
	else {

		printf("\n");

		for (k = 1; k < argc; k++) {
	
			if(isDir(argv[k]) && pathIsRelative(argv[k])) {

				pwd = getpwuid(getuid());

				homePath = pwd->pw_dir;

				strcat(homePath, "/");	

				strcat(homePath, argv[k]);
				
				completePath = homePath; // strcat concatenates strings into first parameter so reassignment is done for clarity.

				argv[k] = completePath;
			}			

			if(stat(argv[k], &buf) == -1) {
				fprintf(stderr, "%s: cannot access %s \n", argv[0], argv[k]);
				printf("\n");
				errors++;
				continue;
			} 

			else if(buf.st_mode & 0400) { 
		
	      			if (getcwd(cwd, sizeof(cwd)) != NULL)
       					fprintf(stdout, "Current working dir: %s\n\n", cwd);
	
				printf("Permissions: ");				

				printf( (S_ISDIR(buf.st_mode)) ? "d" : "-");
		
				pwd = getpwuid(getuid());

				//dirPtr = opendir(argv[k]);

				//direntPtr = readdir(dirPtr);

				// User permissions 
				if(pwd->pw_uid == buf.st_uid) {	
					printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
					printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
					printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
				}

				// Group permissions
				if(pwd->pw_gid == buf.st_gid) {
					printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
					printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
					printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
				}

				// Permissions for other users
				if(direntPtr->d_name == argv[k]) {
					printf( (buf.st_mode & S_IROTH) ? "r" : "-");				
					printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
					printf( (buf.st_mode & S_IXOTH) ? "x" : "-");
					printf("\n");
				}
	
				printf ("Number of links: %lu \n",buf.st_nlink);

				pwd = getpwuid(buf.st_uid);
				printf("User name: %s \n", pwd->pw_name);
		
				grp = *getgrgid(buf.st_gid);
				printf("Group name: %s\n", grp.gr_name);
				
				printf ("Bytes: %lu \n", buf.st_size);		

				printf("Last modified time: %s", ctime(&buf.st_ctime));

				printf("Read: %s \n", argv[k]);

				printf("\n");
			} 

			else { printf("No read\n"); }
		}
	}

	if(errors) { return 1; }
	
	else { return 0; }
}

bool isDir(const char* target) {

	struct stat statbuf;

	stat(target, &statbuf);

	return !S_ISDIR(statbuf.st_mode);
}

bool isFile(const char* target) {

	struct stat statbuf;

	stat(target, &statbuf);

	return S_ISREG(statbuf.st_mode);
}

char* getBaseName(const char* target) {
	
	char* basec = strdup(target);
	char* bname = basename(basec);

	//printf("%s", bname);
	
	return bname;
}

char* getPathName(const char* target) {

	char* dirc = strdup(target);
	char* dname = dirname(dirc);

	//printf("%s", dname);
	
	return dname;
}

bool pathIsRelative(const char* target) {

	if(target[0] == '/' || isFile(target)) { return false; }

	return true;
}

