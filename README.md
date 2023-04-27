#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void displayRegularFileInfo(const char* path) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror("lstat");
        return;
    }

    printf("File name: %s\n", path);
    printf("File type: Regular file\n");
    printf("Size: %ld bytes\n", st.st_size);
    printf("Hard link count: %ld\n", st.st_nlink);
    printf("Time of last modification: %s", ctime(&st.st_mtime));
    printf("Access rights:\n");
    printf("User:\n");
    printf("Read: %s\n", (st.st_mode & S_IRUSR) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWUSR) ? "yes" : "no");
    printf("Exec: %s\n", (st.st_mode & S_IXUSR) ? "yes" : "no");
    printf("Group:\n");
    printf("Read: %s\n", (st.st_mode & S_IRGRP) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWGRP) ? "yes" : "no");
    printf("Exec: %s\n", (st.st_mode & S_IXGRP) ? "yes" : "no");
    printf("Others:\n");
    printf("Read: %s\n", (st.st_mode & S_IROTH) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWOTH) ? "yes" : "no");
    printf("Exec: %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
}

void displaySymbolicLinkInfo(const char* path) {
    struct stat st;
    if (lstat(path, &st) == -1) {
        perror("lstat");
        return;
    }

    printf("File name: %s\n", path);
    printf("File type: Symbolic link\n");
    printf("Size of symbolic link: %ld bytes\n", st.st_size);
    printf("Size of target file: %ld bytes\n", st.st_blocks * 512);
    printf("Access rights:\n");
    printf("User:\n");
    printf("Read: %s\n", (st.st_mode & S_IRUSR) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWUSR) ? "yes" : "no");
    printf("Exec: %s\n", (st.st_mode & S_IXUSR) ? "yes" : "no");
    printf("Group:\n");
    printf("Read: %s\n", (st.st_mode & S_IRGRP) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWGRP) ? "yes" : "no");
    printf("Exec: %s\n", (st.st_mode & S_IXGRP) ? "yes" : "no");
    printf("Others:\n");
    printf("Read: %s\n", (st.st_mode & S_IROTH) ? "yes" : "no");
    printf("Write: %s\n", (st.st_mode & S_IWOTH) ? "yes" : "no");
printf("Exec: %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
}

int main(int argc, char* argv[]) {
// Loop through each argument (file path)
for (int i = 1; i < argc; i++) {
printf("Processing file: %s\n", argv[i]);
    // Determine file type
    struct stat st;
    if (lstat(argv[i], &st) == -1) {
        perror("lstat");
        continue;
    }

    // Display file information based on file type
    if (S_ISREG(st.st_mode)) {
        //displayRegularFileInfo(argv[i]);???
        printf("Interactive menu:\n");
        printf("-n: Display file name\n");
        printf("-d: Display file size\n");
        printf("-h: Display hard link count\n");
        printf("-m: Display time of last modification\n");
        printf("-a: Display access rights\n");
        printf("-I: Create symbolic link (provide link name as input)\n");

        // Get options from user input
        char options[256];
        printf("Enter desired options: ");
        scanf("%s", options);

        // Process options
        for (int j = 0; j < strlen(options); j++) {
            switch(options[j]) {
                case 'n':
                    printf("- File name: %s\n", argv[i]);
                    break;
                case 'd':
                    printf("- Size: %ld bytes\n", st.st_size);
                    break;
                case 'h':
                    printf("- Hard link count: %ld\n", st.st_nlink);
                    break;
                case 'm':
                    printf("- Time of last modification: %s", ctime(&st.st_mtime));
                    break;
                case 'a':
                    printf("- Access rights:\n");
                    printf("  User:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IRUSR) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWUSR) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXUSR) ? "yes" : "no");
                    printf("  Group:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IRGRP) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWGRP) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXGRP) ? "yes" : "no");
                    printf("  Others:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IROTH) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWOTH) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
                    break;
                case 'I':
                    printf("- Enter link name: ");
                    char linkName[256];
                    scanf("%s", linkName);
                    if (symlink(argv[i], linkName) == -1) {
                        perror("symlink");
                    } else {
                        printf("- Symbolic link '%s' created successfully.\n", linkName);
                    }
                    break;
                default:
                    printf("- Invalid option: %c\n", options[j]);
                   
break;
}
}
} else if (S_ISLNK(st.st_mode)) {
//displaySymbolicLinkInfo(argv[i]);
printf("Interactive menu:\n");
printf("-n: Display link name\n");
printf("-1: Delete symbolic link\n");
printf("-d: Display size of symbolic link\n");
printf("-t: Display size of target file\n");
printf("-a: Display access rights\n");
        // Get options from user input
        char options[256];
        printf("Enter desired options: ");
        scanf("%s", options);

        // Process options
        for (int j = 0; j < strlen(options); j++) {
            switch(options[j]) {
                case 'n':
                    printf("- Link name: %s\n", argv[i]);
                    break;
                case '1':
                    if (unlink(argv[i]) == -1) {
                        perror("unlink");
                    } else {
                        printf("- Symbolic link '%s' deleted successfully.\n", argv[i]);
                        break;
                    }
                case 'd': {
                    char linkTarget[256];
                    ssize_t bytesRead = readlink(argv[i], linkTarget, sizeof(linkTarget) - 1);
                    if (bytesRead == -1) {
                        perror("readlink");
                    } else {
                        linkTarget[bytesRead] = '\0';
                        printf("- Size of symbolic link: %ld bytes\n", bytesRead);
                    }
                    break;
                }
                case 't': {
                    struct stat targetSt;
                    if (stat(argv[i], &targetSt) == -1) {
                        perror("stat");
                    } else {
                        printf("- Size of target file: %ld bytes\n", targetSt.st_size);
                    }
                    break;
                }
                case 'a':
                    printf("- Access rights:\n");
                    printf("  User:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IRUSR) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWUSR) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXUSR) ? "yes" : "no");
                    printf("  Group:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IRGRP) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWGRP) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXGRP) ? "yes" : "no");
                    printf("  Others:\n");
                    printf("  Read: %s\n", (st.st_mode & S_IROTH) ? "yes" : "no");
                    printf("  Write: %s\n", (st.st_mode & S_IWOTH) ? "yes" : "no");
                    printf("  Exec: %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
                    break;
                default:
                    printf("- Invalid option: %c\n", options[j]);
                    break;
            }
        }
    } else {
        printf("- File type not supported: %s\n", argv[i]);
    }
}

else if (S_ISDIR(st.st_mode)) {
    printf("Interactive menu:\n");
    printf("-n: Display directory name\n");
    printf("-d: Display directory size\n");
    printf("-a: Display access rights\n");
    printf("-c: Display total number of files with .c extension\n");

    // Get options from user input

    char options[256];
    printf("Enter desired options: ");
    scanf("%s", options);

    // Process options
    for (int j = 0; j < strlen(options); j++) {
        switch(options[j]) {
            case 'n':
                printf("- Directory name: %s\n", argv[i]);
                break;
            case 'd': {
                DIR* dir = opendir(argv[i]);
                if (dir == NULL) {
                    perror("opendir");
                } else {
                    long totalSize = 0;
                    
                    struct dirent* entry;
                    while ((entry = readdir(dir)) != NULL) {
                        struct stat st;
                        char path[PATH_MAX];
                        snprintf(path, PATH_MAX, "%s/%s", argv[i], entry->d_name);
                        if (stat(path, &st) == 0) {
                            totalSize += st.st_size;
                        }
                    }
                    printf("- Directory size: %ld bytes\n", totalSize);
                    closedir(dir);
                }
                break;
            }
            case 'a':
                printf("- Access rights:\n");
                printf("  User:\n");
                printf("  Read: %s\n", (st.st_mode & S_IRUSR) ? "yes" : "no");
                printf("  Write: %s\n", (st.st_mode & S_IWUSR) ? "yes" : "no");
                printf("  Exec: %s\n", (st.st_mode & S_IXUSR) ? "yes" : "no");
                printf("  Group:\n");
                printf("  Read: %s\n", (st.st_mode & S_IRGRP) ? "yes" : "no");
                printf("  Write: %s\n", (st.st_mode & S_IWGRP) ? "yes" : "no");
                printf("  Exec: %s\n", (st.st_mode & S_IXGRP) ? "yes" : "no");
                printf("  Others:\n");
                printf("  Read: %s\n", (st.st_mode & S_IROTH) ? "yes" : "no");
                printf("  Write: %s\n", (st.st_mode & S_IWOTH) ? "yes" : "no");
                printf("  Exec: %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
                break;
            case 'c': {
                DIR* dir = opendir(argv[i]);
                if (dir == NULL) {
                    perror("opendir");
                } else {
                    int count = 0;
                    struct dirent* entry;
                    while ((entry = readdir(dir)) != NULL) {
                        if (entry->d_type == DT_REG) {
                            char* extension = strrchr(entry->d_name, '.');
                            if (extension != NULL && strcmp(extension, ".c") == 0) {
                                count++;
                            }
                            
                        }
                    }
                    printf("- Total number of files with .c extension: %d\n", count);
                    closedir(dir);
                }
                break;
            }
            default:
                printf("- Invalid option: %c\n", options[j]);
                break;
        }
    }
} 

return 0;
}
