#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

#define MAX_OPTIONS 10

// Regular file options
#define REG_NAME 'n'
#define REG_SIZE 'd'
#define REG_HARD_LINKS 'h'
#define REG_LAST_MOD 'm'
#define REG_ACCESS 'a'
#define REG_LINK 'l'

// Symbolic link options
#define LINK_NAME 'n'
#define LINK_DELETE 'l'
#define LINK_SIZE 'd'
#define LINK_TARGET_SIZE 't'
#define LINK_ACCESS 'a'

// Directory options
#define DIR_NAME 'n'
#define DIR_SIZE 'd'
#define DIR_ACCESS 'a'
#define DIR_NUM_FILES 'c'

// Utility function to print access rights
void print_access(mode_t mode) {
    printf("User:\n");
    printf("Read - %s\n", (mode & S_IRUSR) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWUSR) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXUSR) ? "yes" : "no");
    printf("\n");
    printf("Group:\n");
    printf("Read - %s\n", (mode & S_IRGRP) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWGRP) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXGRP) ? "yes" : "no");
    printf("\n");
    printf("Others:\n");
    printf("Read - %s\n", (mode & S_IROTH) ? "yes" : "no");
    printf("Write - %s\n", (mode & S_IWOTH) ? "yes" : "no");
    printf("Exec - %s\n", (mode & S_IXOTH) ? "yes" : "no");
}

// Utility function to read a string from standard input
char* read_string() {
    char* str = malloc(256);
    fgets(str, 256, stdin);
    str[strcspn(str, "\n")] = 0;
    return str;
}

// Regular file options menu
void regular_file_menu(char* path) {
    struct stat sb;
    if (lstat(path, &sb) == -1) {
        perror("lstat");
        exit(EXIT_FAILURE);
    }
    printf("Name: %s\n", path);
    printf("Type: Regular file\n");
    char options[MAX_OPTIONS];
    printf("Options:\n");
    printf("-n Name\n");
    printf("-d Size\n");
    printf("-h Hard link count\n");
    printf("-m Time of last modification\n");
    printf("-a Access rights\n");
    printf("-l Create symbolic link\n");
    scanf("%s", options);
    char* p = options;
    while (*p != '\0') {
        switch (*p) {
            case REG_NAME:
                printf("Name: %s\n", path);
                break;
            case REG_SIZE:
                printf("Size: %ld bytes\n", sb.st_size);
                break;
            case REG_HARD_LINKS:
                printf("Hard link count: %ld\n", sb.st_nlink);
                break;
            case REG_LAST_MOD:
                printf("Time of last modification: %s", ctime(&sb.st_mtime));
                break;
            case REG_ACCESS:
                printf("Access rights:\n");
                print_access(sb.st_mode);
            break;
        case REG_LINK: {
            printf("Enter name of symbolic link: ");
            char* link_name = read_string();
            if (link(path, link_name) == -1) {
                perror("link");
                exit(EXIT_FAILURE);
            }
            printf("Created symbolic link %s -> %s\n", link_name, path);
            free(link_name);
            break;
        }
        default:
            printf("Error: invalid option '%c'\n", *p);
            regular_file_menu(path);
    }
    p++;
}
}

// Symbolic link options menu
void symbolic_link_menu(char* path) {
struct stat sb;
if (lstat(path, &sb) == -1) {
perror("lstat");
exit(EXIT_FAILURE);
}
printf("Name: %s\n", path);
printf("Type: Symbolic link\n");
char options[MAX_OPTIONS];
printf("Options:\n");
printf("-n Name\n");
printf("-l Delete symbolic link\n");
printf("-d Size of symbolic link\n");
printf("-t Size of target file\n");
printf("-a Access rights\n");
scanf("%s", options);
char* p = options;
while (*p != '\0') {
switch (*p) {
case LINK_NAME:
printf("Name: %s\n", path);
break;
case LINK_DELETE:
if (unlink(path) == -1) {
perror("unlink");
exit(EXIT_FAILURE);
}
printf("Deleted symbolic link %s\n", path);
return;
case LINK_SIZE:
printf("Size of symbolic link: %ld bytes\n", sb.st_size);
break;
case LINK_TARGET_SIZE: {
struct stat target_sb;
if (stat(path, &target_sb) == -1) {
perror("stat");
exit(EXIT_FAILURE);
}
printf("Size of target file: %ld bytes\n", target_sb.st_size);
break;
}
case LINK_ACCESS:
printf("Access rights:\n");
print_access(sb.st_mode);
break;
default:
printf("Error: invalid option '%c'\n", *p);
symbolic_link_menu(path);
}
p++;
}
}

// Directory options menu
void directory_menu(char* path) {
struct stat sb;
if (lstat(path, &sb) == -1) {
perror("lstat");
exit(EXIT_FAILURE);
}
printf("Name: %s\n", path);
printf("Type: Directory\n");
char options[MAX_OPTIONS];
printf("Options:\n");
printf("-n Name\n");
printf("-d Size\n");
printf("-a Access rights\n");
printf("-c Total number of files with the .c extension\n");
scanf("%s", options);
char* p = options;
while (p != '\0') {
switch (p) {
case DIR_NAME:
printf("Name: %s\n", path);
break;
case DIR_SIZE:
printf("Size: %ld bytes\n", sb.st_size);
break;
case DIR_ACCESS:
printf("Access rights:\n");
print_access(sb.st_mode);
break;
case DIR_NUM_FILES: {
DIR dir = opendir(path);
if (dir == NULL) {
perror("opendir");
exit(EXIT_FAILURE);
}
int num_files = 0;
struct dirent entry;
while ((entry = readdir(dir)) != NULL) {
if (entry->d_type == DT_REG && strstr(entry->d_name, ".c") != NULL
                    num_files++;
                }
            }
            closedir(dir);
            printf("Total number of files with the .c extension: %d\n", num_files);
            break;
        }
        default:
            printf("Error: invalid option '%c'\n", *p);
            directory_menu(path);
    }
    p++;
}
}

// Main function
int main(int argc, char* argv[]) {
for (int i = 1; i < argc; i++) {
char* path = argv[i];
struct stat sb;
if (lstat(path, &sb) == -1) {
perror("lstat");
exit(EXIT_FAILURE);
}
printf("Name: %s\n", path);
printf("Type: ");
switch (sb.st_mode & S_IFMT) {
case S_IFREG:
printf("Regular file\n");
regular_file_menu(path);
break;
case S_IFLNK:
printf("Symbolic link\n");
symbolic_link_menu(path);
break;
case S_IFDIR:
printf("Directory\n");
directory_menu(path);
break;
default:
printf("Unknown file type\n");
}
}
return 0;
}