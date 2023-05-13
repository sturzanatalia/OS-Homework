#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

void handle_regular_file(const char *path);
void handle_directory(const char *path);
void handle_symlink(const char *path);
void print_file_info(const char *path);
void print_file_permissions(const char *path);

void print_file_menu() {
    printf("\nRegular File Options:\n");
    printf("-n: Name\n");
    printf("-d: Size\n");
    printf("-h: Hard link count\n");
    printf("-m: Time of last modification\n");
    printf("-a: Access rights\n");
    printf("-l: Create symbolic link (provide a name for the link in the next step)\n\n");
    //printf("Enter your option(s): ");
}

void print_symlink_menu() {
    printf("\nSymbolic Link Options:\n");
    printf("-n: Name\n");
    printf("-d: Size of symbolic link\n");
    printf("-t: Size of target file\n");
    printf("-a: Access rights\n");
    printf("-l: Delete symbolic link\n\n");
    //printf("Enter your option(s): ");
}

void print_dir_menu() {
    printf("\nDirectory Options:\n");
    printf("-n: Name\n");
    printf("-d: Size\n");
    printf("-a: Access rights\n");
    printf("-c: Total number of files with the .c extension\n\n");
    //printf("Enter your option(s): ");
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Please provide at least one file path argument.\n");
        return 1;
    }

    for(int i = 1; i < argc; i++) {
        struct stat s;
        if(lstat(argv[i], &s) == -1) {
            perror("lstat");
            continue;
        }

        if(S_ISREG(s.st_mode)) {
            print_file_menu();
            handle_regular_file(argv[i]);
        } else if(S_ISDIR(s.st_mode)) {
            print_dir_menu();
            handle_directory(argv[i]);
        } else if(S_ISLNK(s.st_mode)) {
            print_symlink_menu();
            handle_symlink(argv[i]);
        } else {
            fprintf(stderr, "Unsupported file type for %s\n", argv[i]);
        }
    }

    return 0;
}

void handle_regular_file(const char* filePath) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // Child process
        // Prompt for options
        printf("Enter options: ");
        char options[100]; // Assuming maximum 100 characters for options string
        scanf("%s", options);
        struct stat fileStat;
        // Print file information based on options
        if (strchr(options, 'n') != NULL) {
            printf("Name: %s\n", filePath);
        }
        if (strchr(options, 'd') != NULL) {
            printf("Size: %lld bytes\n", fileStat.st_size);
        }
        if (strchr(options, 'h') != NULL) {
            printf("Hard link count: %hu\n", fileStat.st_nlink);
        }
        if (strchr(options, 'm') != NULL) {
            char modTime[20];
            strftime(modTime, sizeof(modTime), "%Y-%m-%d %H:%M:%S", localtime(&fileStat.st_mtime));
            printf("Last modified time: %s\n", modTime);
        }
        if (strchr(options, 'a') != NULL) {
            printf("Access rights:\n");
            printf("User:\n");
            printf("Read - %s\n", (fileStat.st_mode & S_IRUSR) ? "yes" : "no");
            printf("Write - %s\n", (fileStat.st_mode & S_IWUSR) ? "yes" : "no");
            printf("Exec - %s\n", (fileStat.st_mode & S_IXUSR) ? "yes" : "no");
            printf("Group:\n");
            printf("Read - %s\n", (fileStat.st_mode & S_IRGRP) ? "yes" : "no");
            printf("Write - %s\n", (fileStat.st_mode & S_IWGRP) ? "yes" : "no");
            printf("Exec - %s\n", (fileStat.st_mode & S_IXGRP) ? "yes" : "no");
            printf("Others:\n");
            printf("Read - %s\n", (fileStat.st_mode & S_IROTH) ? "yes" : "no");
            printf("Write - %s\n", (fileStat.st_mode & S_IWOTH) ? "yes" : "no");
            printf("Exec - %s\n", (fileStat.st_mode & S_IXOTH) ? "yes" : "no");
        }
        if (strchr(options, 'l') != NULL) {
            printf("Enter the name of the symbolic link: ");
            char linkName[100];
            scanf("%s", linkName);
            if (symlink(filePath, linkName) == -1) {
                perror("Error in creating symbolic link");
            } else {
                printf("Symbolic link created successfully.\n");
            }
        }
        printf("\n");
        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            printf("The process with PID %d has ended with the exit code %d\n", pid, WEXITSTATUS(status));
        }

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            return;
        }

        pid_t pid2 = fork();
        if(pid2 < 0) {
            perror("fork");
            return;
        } else if(pid2 == 0) {
            // Second child process
            // Close the read end of the pipe
            close(pipefd[0]);
            // Redirect stdout to the write end of the pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
            // Check if file has .c extension
            int len = strlen(filePath);
            if(len > 2 && strcmp(filePath + len - 2, ".c") == 0) {
                execlp("./compile.sh", "compile.sh", filePath, NULL);
            } else {
                // Print number of lines for non .c files
                char command[256];
                sprintf(command, "wc -l %s", filePath);
                system(command);
            }
            exit(0);
        } else {
            // Parent process
            int status;
            waitpid(pid2, &status, 0);
            if(WIFEXITED(status)) {
                printf("The process with PID %d has ended with the exit code %d\n", pid2, WEXITSTATUS(status));
            }
            // Close the write end of the pipe
            close(pipefd[1]);
            // Read the output from the pipe
            char buffer[256];

            int numRead = read(pipefd[0], buffer, sizeof(buffer) - 1);  // leave space for null terminator
            close(pipefd[0]);
            if (numRead >= 0) {
                buffer[numRead] = '\0';  // null terminate the string
            } else {
                perror("read");
                return;
            }
            //printf("Buffer contents: '%s'\n", buffer);  // Add this line

            close(pipefd[0]);
            int errors = 0, warnings = 0;
            FILE *grades = fopen("grades.txt", "a");
            if(grades == NULL) {
                perror("fopen");
                return;
            }

            // If file has .c extension, compute score based on errors and warnings
            int len = strlen(filePath);
            if(len > 2 && strcmp(filePath + len - 2, ".c") == 0) {
                sscanf(buffer, "%d,%d", &errors, &warnings);
                int score = 0;
                if(errors > 0) {
                    score = 1;
                } else if(warnings > 10) {
                    score = 2;
                } else {
                    score = 2 + 8*(10 - warnings)/10;
                }

                fprintf(grades, "%s: %d\n", filePath, score);
            } else {
                // If file does not have .c extension, print number of lines
                int lines = 0;
                sscanf(buffer, "%d", &lines);
                fprintf(grades, "%s: %d lines\n", filePath, lines);
            }
            fclose(grades);
        }
    }
}

void handle_directory(const char *path) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // First child process
        char options[1024];
        printf("Enter options for directory %s: ", path);
        scanf("%s", options);
        
        DIR *dir;
        struct dirent *entry;
        struct stat st;

        if (strchr(options, 'n')) {
            printf("Name: %s\n", path);
        }
        if (strchr(options, 'd')) {
            long long total_size = 0;
            if ((dir = opendir(path)) == NULL) {
                perror("opendir");
            } else {
                while ((entry = readdir(dir)) != NULL) {
                    char filepath[1024];
                    snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
                    if (lstat(filepath, &st) == -1) {
                        perror("lstat");
                        continue;
                    }
                    if (entry->d_type == DT_REG) {
                        total_size += st.st_size;
                    }
                }
                closedir(dir);
                printf("Size: %lld bytes\n", total_size);
            }
        }
        if (strchr(options, 'a')) {
            print_file_permissions(path);
        }
        if (strchr(options, 'c')) {
            int count = 0;
            if ((dir = opendir(path)) == NULL) {
                perror("opendir");
            } else {
                while ((entry = readdir(dir)) != NULL) {
                    if (strstr(entry->d_name, ".c") != NULL) {
                        count++;
                    }
                }
                closedir(dir);
                printf("Number of .c files: %d\n", count);
            }
        }
        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("The process with PID %d has ended with the exit code %d\n", pid, WEXITSTATUS(status));

        pid_t pid2 = fork();
        if(pid2 < 0) {
            perror("fork");
            return;
        } else if(pid2 == 0) {
            // Second child process
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s_file.txt", path);
            FILE *file = fopen(filename, "w");
            if (file == NULL) {
                perror("fopen");
                exit(1);
            }
            fprintf(file, "This file was created by the second child process.\n");
            fclose(file);
            exit(0);
        } else {
            // Parent process
            waitpid(pid2, &status, 0);
            printf("The process with PID %d has ended with the exit code %d\n", pid2, WEXITSTATUS(status));
        }
    }
}

void handle_symlink(const char *path) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // First child process
        char options[1024];
        printf("Enter options for symlink %s: ", path);
        scanf("%s", options);

        struct stat st;
        if (strchr(options, 'n') || strchr(options, 'd') || strchr(options, 't') || strchr(options, 'a')) {
            // Check if the symlink still exists before performing any operations
            if(lstat(path, &st) == -1) {
                perror("lstat");
                printf("The symlink is no longer existent\n");
                exit(0);
            }
        }
        if (strchr(options, 'n')) {
            printf("Name: %s\n", path);
        }
        if (strchr(options, 'l')) {
            if(unlink(path) == 0) {
                printf("Symbolic link deleted.\n");
            } else {
                perror("unlink");
            }
        }
        if (strchr(options, 'd')) {
            printf("Size of symbolic link: %lld bytes\n", (long long)st.st_size);
        }
        if (strchr(options, 't')) {
            if(stat(path, &st) == 0) {
                printf("Size of target file: %lld bytes\n", (long long)st.st_size);
            } else {
                perror("stat");
            }
        }
        if (strchr(options, 'a')) {
            print_file_permissions(path);
        }

        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("The process with PID %d has ended with the exit code %d\n", pid, WEXITSTATUS(status));

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            return;
        } else if (pid2 == 0) {
            // Second child process
            // Attempt to change the permissions of the symbolic link
            if(chmod(path, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP) == -1) {
                perror("chmod");
            }
            exit(0);
        } else {
            // Parent process
            waitpid(pid2, &status, 0);
            printf("The process with PID %d has ended with the exit code %d\n", pid2, WEXITSTATUS(status));
        }
    }
}

void print_file_info(const char *path) {
    struct stat st;
    int ret = stat(path, &st);
    if(ret != 0) {
        perror("stat");
        return;
    }

    if(S_ISLNK(st.st_mode)) {
        ret = lstat(path, &st);
        if(ret != 0) {
            perror("lstat");
            return;
        }
        printf("The file is a symbolic link.\n");
    } else {
        printf("The file is not a symbolic link.\n");
    }

    printf("Information for %s:\n", path);
    printf("---------------------------\n");
    printf("File Size: \t\t%lld bytes\n", (long long)st.st_size);
    printf("Number of Links: \t%ld\n", (long)st.st_nlink);
    printf("File inode: \t\t%ld\n", (long)st.st_ino);

    printf("File Permissions: \t");
    printf( (S_ISDIR(st.st_mode)) ? "d" : "-");
    printf( (st.st_mode & S_IRUSR) ? "r" : "-");
    printf( (st.st_mode & S_IWUSR) ? "w" : "-");
    printf( (st.st_mode & S_IXUSR) ? "x" : "-");
    printf( (st.st_mode & S_IRGRP) ? "r" : "-");
    printf( (st.st_mode & S_IWGRP) ? "w" : "-");
    printf( (st.st_mode & S_IXGRP) ? "x" : "-");
    printf( (st.st_mode & S_IROTH) ? "r" : "-");
    printf( (st.st_mode & S_IWOTH) ? "w" : "-");
    printf( (st.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
}

void print_file_permissions(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("stat");
        return;
    }

    printf("Access rights for %s:\n", path);
    printf("User: ");
    printf("\n\tRead - %s", (st.st_mode & S_IRUSR) ? "yes" : "no");
    printf("\n\tWrite - %s", (st.st_mode & S_IWUSR) ? "yes" : "no");
    printf("\n\tExec - %s", (st.st_mode & S_IXUSR) ? "yes" : "no");

    printf("\nGroup: ");
    printf("\n\tRead - %s", (st.st_mode & S_IRGRP) ? "yes" : "no");
    printf("\n\tWrite - %s", (st.st_mode & S_IWGRP) ? "yes" : "no");
    printf("\n\tExec - %s", (st.st_mode & S_IXGRP) ? "yes" : "no");

    printf("\nOthers: ");
    printf("\n\tRead - %s", (st.st_mode & S_IROTH) ? "yes" : "no");
    printf("\n\tWrite - %s", (st.st_mode & S_IWOTH) ? "yes" : "no");
    printf("\n\tExec - %s\n", (st.st_mode & S_IXOTH) ? "yes" : "no");
}

