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
        printf("Invalid Selection");
        return 1;
    }

    printf("You have selected: %s (major: %u, minor: %u, blocks: %lu)\n", devices[choice].name, devices[choice].major, devices[choice].minor, devices[choice].blocks);

    return 0;
}
