#include "../../include/core.h"

/**
 * This File handles scanning the disks available
*/

#define MAX_DEVICES 128

typedef struct {
    char name[38];
    unsigned int major;
    unsigned int minor;
    unsigned long blocks;
} Device;

#include <stdio.h>
#include <sys/statfs.h>
#include <string.h> 

int is_valid_file_system_type(const char *device_name) {
    // Checks the file system type
    // In this current version, we are only supporting ext4 file system
    // This file system is mostly used by Linux-based distros

    struct statfs buf;

    if (statfs(device_name, &buf) != 0) {
        perror("Error retrieving filesystem info");
        return -1;
    }

    switch (buf.f_type) {
        case EXT4_SUPER_MAGIC:
            printf("File system type: ext4\n");
            break;
        case XFS_SUPER_MAGIC:
            printf("Unsupported File system type: xfs\n");
            return -1;
         
        case TMPFS_MAGIC:
            printf("Unsupported FileSystem type: tmpfs\n");
            break;
        case BTRFS_SUPER_MAGIC:
            printf("Unsupported File system type: btrfs\n");
            return -1;
        default:
            printf("File system type: unknown (0x%lx)\n", buf.f_type);
            break;
    }
    return 0;
}

int open_disk(const char *device_name) {
    int res = is_valid_file_system_type(device_name);
    int fd = open(device_name, O_RDONLY);
    if (fd < 0) {
        perror("Error: unable to open disk\nCheck your permissions");
        
        return -1;
    }
    printf("successfully opened the disk\n");
    return fd;
}

int scan_disk() {
    int fd;
    unsigned long num_sectors;
    Device devices[MAX_DEVICES];
    int device_count = 0;

    FILE *file = fopen("/proc/partitions", "r");
    if (file == NULL) {
        perror("Unable to open /proc/partitions");
        return 1 ;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        unsigned int major, minor;
        unsigned long blocks;
        char name[32];
        
        if (sscanf(buffer, "%u %u %lu %s", &major, &minor, &blocks, name) == 4) {
            devices[device_count].major = major;
            devices[device_count].minor = minor;
            devices[device_count].blocks = blocks;


            snprintf(devices[device_count].name, sizeof(devices[device_count].name), "/dev/%s", name);
            char i = device_count;
            printf("%i] Device: %s, major: %u, minor: %u, blocks: %lu\n",i, name, major, minor, blocks);
            device_count ++;
        }
    }

    fclose(file);

    if (device_count == 0) {
        printf("no device found");
        return 1;
    }

    int choice;

    printf("Enter the number of the device you want to select for recovery: ");
    if (scanf("%d", &choice) != 1 || choice < 0 || choice >= device_count) {
        printf("Invalid Selection\n");
        return 1;
    }

    printf("You have selected: %s (major: %u, minor: %u, blocks: %lu)\n", devices[choice].name, devices[choice].major, devices[choice].minor, devices[choice].blocks);

    open_disk(devices[choice].name);
    return 0;
}


