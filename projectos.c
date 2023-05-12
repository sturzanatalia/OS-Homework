#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <dirent.h>

void handle_regular_file(const char *path);
void handle_directory(const char *path);
void handle_symlink(const char *path);
void print_file_info(const char *path);
void print_file_permissions(const char *path);

int main(int argc, char *argv[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s <path1> <path2> ...\n", argv[0]);
        return 1;
    }

    for(int i = 1; i < argc; i++) {
        struct stat s;
        if(lstat(argv[i], &s) == -1) {
            perror("lstat");
            continue;
        }

        if(S_ISREG(s.st_mode)) {
            handle_regular_file(argv[i]);
        } else if(S_ISDIR(s.st_mode)) {
            handle_directory(argv[i]);
        } else if(S_ISLNK(s.st_mode)) {
            handle_symlink(argv[i]);
        } else {
            fprintf(stderr, "Unsupported file type for %s\n", argv[i]);
        }
    }

    return 0;
}

void handle_regular_file(const char *path) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // Child process
        char option;
        printf("Enter option for file %s: ", path);
        scanf("%c", &option);
        struct stat st;
        switch (option) {
            case 'n':
                printf("Name: %s\n", path);
                break;
            case 'd': 
                if (lstat(path, &st) != -1) {
                    printf("Size: %lld bytes\n", (long long) st.st_size);
                }
                break;
            case 'h': 
                if (lstat(path, &st) != -1) {
                    printf("Hard link count: %hu\n", st.st_nlink);
                }
                break;
            case 'm':
                if (lstat(path, &st) != -1) {
                    printf("Time of last modification: %s", ctime(&st.st_mtime));
                }
                break;
            case 'a':
                print_file_permissions(path);
                break;
            case 'l': {
                char linkname[256];
                printf("Enter the name of the symbolic link: ");
                scanf("%s", linkname);
                if (symlink(path, linkname) != -1) {
                    printf("Symbolic link created.\n");
                }
                break;
            }
            default:
                printf("Invalid option\n");
        }
        exit(0);
    } else {
        // Parent process
        int len = strlen(path);
        if(len > 2 && strcmp(path + len - 2, ".c") == 0) {
            // Handle the grading system for .c files
            char command[256];
            sprintf(command, "./grade.sh %s >> grades.txt", path);
            int ret = system(command);
            if(ret == -1) {
                perror("system");
                exit(EXIT_FAILURE);
            } else if(WIFEXITED(ret)) {
                int exit_status = WEXITSTATUS(ret);
                if(exit_status != 0) {
                    fprintf(stderr, "grade.sh exited with status %d\n", exit_status);
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "grade.sh did not terminate normally\n");
                exit(EXIT_FAILURE);
            }
        } else {
            // Count the number of lines in the file and print it
            char command[256];
            sprintf(command, "wc -l %s > tmp.txt", path);
            int ret = system(command);
            if(ret == -1) {
                perror("system");
                exit(EXIT_FAILURE);
            } else if(WIFEXITED(ret)) {
                int exit_status = WEXITSTATUS(ret);
                if(exit_status != 0) {
                    fprintf(stderr, "wc -l exited with status %d\n", exit_status);
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "wc -l did not terminate normally\n");
                exit(EXIT_FAILURE);
            }

            // Open the temporary file and read the number of lines
            FILE *tmp = fopen("tmp.txt", "r");
            if(tmp == NULL) {
                perror("fopen");
                exit(EXIT_FAILURE);
            }

            int lines;
            if(fscanf(tmp, "%d", &lines) != 1) {
                fprintf(stderr, "Failed to read number of lines from tmp.txt\n");
                exit(EXIT_FAILURE);
            }
            fclose(tmp);

            // Remove the temporary file
            if(remove("tmp.txt") != 0) {
                perror("remove");
                exit(EXIT_FAILURE);
            }

            printf("The file %s contains %d lines.\n", path, lines);
        }
       
    }
}

void handle_directory(const char *path) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // Child process
        char option;
        printf("Enter option for directory %s: ", path);
        scanf(" %c", &option);
        DIR *dir;
        struct dirent *entry;
        struct stat st;
        switch (option) {
            case 'n':
                printf("Name: %s\n", path);
                break;
            case 'd':
                {
                    long long total_size = 0;
                    if ((dir = opendir(path)) == NULL) {
                        perror("opendir");
                        break;
                    }
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
                break;
            case 'a':
                print_file_permissions(path);
                break;
            case 'c':
                {
                    int count = 0;
                    if ((dir = opendir(path)) == NULL) {
                        perror("opendir");
                        break;
                    }
                    while ((entry = readdir(dir)) != NULL) {
                        if (strstr(entry->d_name, ".c") != NULL) {
                            count++;
                        }
                    }
                    closedir(dir);
                    printf("Number of .c files: %d\n", count);
                }
                break;
            default:
                printf("Invalid option\n");
        }
        exit(0);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        printf("The process with PID %d has ended with the exit code %d\n", pid, WEXITSTATUS(status));
        char filename[1024];
        snprintf(filename, sizeof(filename), "%s_file.txt", path);
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            perror("fopen");
            return;
        }
        fprintf(file, "This file was created by the parent process.\n");
        fclose(file);
    }
}

void handle_symlink(const char *path) {
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork");
        return;
    } else if(pid == 0) {
        // First child process
        char option;
        printf("Enter option for symlink %s: ", path);
        scanf(" %c", &option);
        struct stat st;
        switch (option) {
            case 'n':
                printf("Name: %s\n", path);
                break;
            case 'l':
                if(unlink(path) == 0) {
                    printf("Symbolic link deleted.\n");
                } else {
                    perror("unlink");
                }
                break;
            case 'd':
                if(lstat(path, &st) == 0) {
                    printf("Size of symbolic link: %lld bytes\n", (long long)st.st_size);
                } else {
                    perror("lstat");
                }
                break;
            case 't':
                if(stat(path, &st) == 0) {
                    printf("Size of target file: %lld bytes\n", (long long)st.st_size);
                } else {
                    perror("stat");
                }
                break;
            case 'a':
                print_file_permissions(path);
                break;
            default:
                printf("Invalid option\n");
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

