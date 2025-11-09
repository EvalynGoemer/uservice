#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "uservice.h"

int init_system = UNSUPPORTED_INIT_SYSTEM;

// fills in "init_system" with the current init system and exits the program if it cannot find one
void check_init_system() {
    /*
     * TODO: add support for more initsysems
     *
     * TODO: fill with more directories for systemd binaries that might exist in diffrent locations
     * only add binaries that are critical to systemd operation as to not make check take ages
     * maybe there is a better way to check but this is a dumb easy check to do real quick
     * currently this is known to work for the following init systems and OSes
     *
     * systemd:
     * - Arch Linux
     * - CachyOS
     * - RaspberryPi OS
     *
     * openRC:
     * - Alpine Linux
     *
     * INFO: Checks for the same executable should be part of the same if / if else tree and if not found
     * it should increase the init_system_confidence_levels for the respective init system if the file is found
     * it should decrease the init_system_confidence_levels for the respective init system if it is critical to operation
     * in having it work for the system and in operatin of this program to translate commands to
     * it should always set any_init_system_found to true if any file is found
    */

    int init_system_confidence_levels[NUMBER_OF_SUPPORTED_INIT_SYSTEMS] = {0};
    bool any_init_system_found = false;

    // detect systemd
    if (access("/usr/bin/systemctl", F_OK) == 0) {
        init_system_confidence_levels[SYSTEMD]++;
        any_init_system_found = true;
    } else {
        init_system_confidence_levels[SYSTEMD]--;
    }

    if (access("/usr/lib/systemd/systemd", F_OK) == 0) {
        init_system_confidence_levels[SYSTEMD]++;
        any_init_system_found = true;
    } else {
        init_system_confidence_levels[SYSTEMD]--;
    }

    if (access("/usr/lib/systemd/systemd-journald", F_OK) == 0) {
        init_system_confidence_levels[SYSTEMD]++;
        any_init_system_found = true;
    }

    // detect openRC
    if (access("/sbin/rc-service", F_OK) == 0) {
        init_system_confidence_levels[OPENRC]++;
        any_init_system_found = true;
    } else {
        init_system_confidence_levels[OPENRC]--;
    }

    if (access("/sbin/rc-update", F_OK) == 0) {
        init_system_confidence_levels[OPENRC]++;
        any_init_system_found = true;
    } else {
        init_system_confidence_levels[OPENRC]--;
    }

    if (any_init_system_found) {
        int best_init_system_found = UNSUPPORTED_INIT_SYSTEM;
        int best_init_system_score = 0;
        for (int i = 0; i < NUMBER_OF_SUPPORTED_INIT_SYSTEMS; i++) {
            if (best_init_system_score < init_system_confidence_levels[i]) {
                best_init_system_score = init_system_confidence_levels[i];
                best_init_system_found = i;
            }
        }

        if (best_init_system_found != UNSUPPORTED_INIT_SYSTEM) {
            init_system = best_init_system_found;
        } else {
            printf("FATAL: NO SUPPORTED INIT SYSTEM DETECTED;\n");
            exit(-1);
        }
    } else {
        printf("FATAL: NO SUPPORTED INIT SYSTEM DETECTED;\n");
        exit(-1);
    }

    #ifdef DEBUG
        printf("Best Found Init System: %s\n", init_system_name_dbg[init_system]);
        printf("Init System Confidence Levels\n");
        for (int i = 0; i < NUMBER_OF_SUPPORTED_INIT_SYSTEMS; i++) {
            printf("%s: %d\n",init_system_name_dbg[i], init_system_confidence_levels[i]);
        }
    #endif
}

void fork_and_execute_2arg(char command[], char arg1[], char arg2[]) {
    int pid = fork();
    if (pid < 0) {
        perror("FATAL: FAILED TO FORK");
        exit(-1);
    }

    if(pid == 0) {
        int status = execlp(command, command, arg1, arg2, NULL);
        if (status == -1) {
            printf("FATAL: CALLING \"%s %s %s\" FAILED\n", command, arg1, arg2);
            exit(-1);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        exit(0);
    }
}

void fork_and_execute_3arg(char command[], char arg1[], char arg2[], char arg3[]) {
    int pid = fork();
    if (pid < 0) {
        perror("FATAL: FAILED TO FORK");
        exit(-1);
    }

    if(pid == 0) {
        int status = execlp(command, command, arg1, arg2, arg3, NULL);
        if (status == -1) {
            printf("FATAL: CALLING \"%s %s %s %s\" FAILED\n", command, arg1, arg2, arg3);
            exit(-1);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    #ifdef DEBUG
        printf("Provided Arguments:\n");
        for (int i = 0; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    #endif

    if (argc != 3) {
        printf("Incorrect amount of arguments; Usage: uservice <OPERATION> <SERVICE>; Check README.MD for more details\n");
        exit(-1);
    }

    // check for config file and if it dosnt exist attempt auto detection
    // thanks @ymity (discord) for helping
    FILE *fptr;
    if ((fptr = fopen("/etc/uservice.conf", "r"))) {

        char fbuf[16] = {0};
        fgets(fbuf, 16, fptr);

        for (int i = 0; i < NUMBER_OF_SUPPORTED_INIT_SYSTEMS; i++) {
            if (strcmp(fbuf, init_system_name[i]) == 0) {
                init_system = i;
                break;
            }
        }
        fclose(fptr);
    } else{
        check_init_system();
    }

    // sanity check
    if (init_system == UNSUPPORTED_INIT_SYSTEM) {
        printf("FATAL: NO SUPPORTED INIT SYSTEM DETECTED;\n");
        exit(-1);
    }

    if (strcmp("start", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "start", argv[2]);
        } else if (init_system == OPENRC) {
            fork_and_execute_2arg("rc-service", argv[2], "start");
        }
    }

    if (strcmp("stop", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "stop", argv[2]);
        } else if (init_system == OPENRC) {
            fork_and_execute_2arg("rc-service", argv[2], "stop");
        }
    }

    if (strcmp("restart", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "restart", argv[2]);
        } else if (init_system == OPENRC) {
            fork_and_execute_2arg("rc-service", argv[2], "restart");
        }
    }

    if (strcmp("status", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "status", argv[2]);
        } else if (init_system == OPENRC) {
            fork_and_execute_2arg("rc-service", argv[2], "status");
        }
    }

    if (strcmp("enable", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "enable", argv[2]);
        } else if (init_system == OPENRC) {
            // assume run level default as its the most common
            fork_and_execute_3arg("rc-update", "add", argv[2], "default");
        }
    }

    if (strcmp("disable", argv[1]) == 0) {
        if (init_system == SYSTEMD) {
            fork_and_execute_2arg("systemctl", "disable", argv[2]);
        } else if (init_system == OPENRC) {
            // assume run level default as its the most common
            fork_and_execute_3arg("rc-update", "del", argv[2], "default");
        }
    }
}
