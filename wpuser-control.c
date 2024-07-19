#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sched.h>
#include <stdint.h>

#define DEVICE_FILE "/dev/way-part"
#define IOCTL_WRITE_CLUSTERPARTCR _IO('k', 1)
#define IOCTL_WRITE_CLUSTERSTASHSID _IO('k', 2)
#define IOCTL_WRITE_CLUSTERTHREADSIDOVR _IO('k', 3)
#define IOCTL_WRITE_CLUSTERACPSID _IO('k', 4)
#define IOCTL_WRITE_CLUSTERTHREADSID _IO('k', 5)
#define IOCTL_READ_CLUSTERPARTCR _IO('k', 6)
#define IOCTL_READ_CLUSTERTHREADSID _IO('k', 10)
#define IOCTL_READ_CPUECTLR _IO('k', 11)
#define IOCTL_READ_CLUSTERECTLR _IO('k', 21)


typedef struct ioctl_data {
    uint32_t in_value;
    uint32_t* out_value_low;
    uint32_t* out_value_high;
}ioctl_data;


void pin_cpu(long cpu)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset); // Initialize CPU set to all zeros

    // Add CPU cores to the CPU set
    CPU_SET(cpu, &cpuset); // Add CPU core 0
    pid_t pid = getpid(); // Get PID of the current process
    int result = sched_setaffinity(pid, sizeof(cpuset), &cpuset);
    if (result != 0) {
        perror("sched_setaffinity");
        exit(EXIT_FAILURE);
    }
    return;
}



int main(int argc, char* argv[]) {
    int fd;
    int ret;
    uint32_t read_value_low = 0; // lower 32 bits
    uint32_t read_value_high = 0; // upper 32 bits

    if (argc != 4 )
    {
        printf("[USAGE]: waypart [reg#] [value in hexadecimal] [cpu #]\n");
        printf("Reg Numbers:\n1: CLUSTERPARTCR\n2: CLUSTERSTASHSID\n3: CLUSTERTHREADSIDOVR\n4: CLUSTERACPSID\n5: CLUSTERTHREADSID\n");
        printf("6: Read CLUSTERPARTCR\n10:Read CLUSTERTHREADSID\n");
        exit(0);
    }


    

    long reg = strtol(argv[1], NULL, 10);
    long value = strtol(argv[2], NULL, 16);
    long cpu = strtol(argv[3], NULL, 10);
    if (reg <= 0 || reg > 22)
    {
        printf("Invalid register number.\n");
        exit(-1);
    }
    pin_cpu(cpu);


    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device file");
        return 1;
    }

    ioctl_data data;
    data.in_value = value;
    data.out_value_low = &read_value_low;
    data.out_value_high = &read_value_high;
    switch(reg)
    {
        case 1: ret = ioctl(fd, IOCTL_WRITE_CLUSTERPARTCR, &data); break;
        case 2: ret = ioctl(fd, IOCTL_WRITE_CLUSTERSTASHSID, &data); break;
        case 3: ret = ioctl(fd, IOCTL_WRITE_CLUSTERTHREADSIDOVR, &data); break;
        case 4: ret = ioctl(fd, IOCTL_WRITE_CLUSTERACPSID, &data); break;
        case 5: ret = ioctl(fd, IOCTL_WRITE_CLUSTERTHREADSID, &data); break;
        case 6: ret = ioctl(fd, IOCTL_READ_CLUSTERPARTCR, &data); break;
        case 10: ret = ioctl(fd, IOCTL_READ_CLUSTERTHREADSID, &data); break;
        case 11: ret = ioctl(fd, IOCTL_READ_CPUECTLR, &data); break;
        case 21: ret = ioctl(fd, IOCTL_READ_CLUSTERECTLR, &data); break;
        default:
        {
            printf("Invalid register number.\n");
            close(fd);
            exit(-1);
        }
    }

    if (ret < 0) {
        perror("IOCTL_INC_COUNT ioctl failed");
        close(fd);
        return -1;
    }

    if (read_value_low)
        printf("Read value: 0x%x\n", read_value_low);
    if (read_value_high)
        printf("Read Value high: 0x%x\n", read_value_high);
    
    // Close the device file
    close(fd);

    return 0;
}
