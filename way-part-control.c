#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/cpumask.h>  
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/pid.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>


typedef struct ioctl_data {
    uint32_t in_value;
    uint32_t* out_value_low;
    uint32_t* out_value_high;
}ioctl_data;




 /*
    [CLUSTERPARTCR_EL1]
    
    There are 4 cache ways architecturally available to us. We can allocate these to 8 different schemes(0-7).
    Bits[0:3] are used to allocate cache ways to scheme 0, Bits[4:7] are used to allocate cache ways to scheme 1, and so on
*/
void write_CLUSTERPARTCR_EL1(uint32_t value)
{
    // Write to CLUSTERPARTCR_EL1 register
    asm volatile (
        "msr S3_0_C15_C4_3, %0\n" // Move value into System register
        :
        : "r" (value)   // gp register for input
        : "memory"      // clobber --> ensure memory operations are serialized before/after
    );

}

 /*
    [CLUSTERPARTCR_EL1]
    
    There are 4 cache ways architecturally available to us. We can allocate these to 8 different schemes(0-7).
    Bits[0:3] are used to allocate cache ways to scheme 0, Bits[4:7] are used to allocate cache ways to scheme 1, and so on
*/
uint32_t read_CLUSTERPARTCR_EL1(void)
{   
    uint32_t value;
    asm volatile (
        "mrs %0, S3_0_C15_C4_3\n"
        : "=r" (value)
    );
    return value;
}



/*
    [CLUSTERSTASHSID_EL1]

    - Bits[0:2] are used to set the schemeID for stash transactions read over the interconnect
    - Bits[3:31] are read as zero

*/
void write_CLUSTERSTASHSID_EL1(uint32_t value)
{
    asm volatile (
        "msr S3_0_C15_C4_2, %0\n"
        :
        : "r" (value)
        : "memory"
    );
}


/*
    [CLUSTERTHREADSIDOVER_EL1]

    - Bits[0:2] are used to set the schemeID if the thread is masked
    - Bits[3:15] are read as zero
    - Bits[16:18] are the schemeID mask,
      a bit set in the mask causes the matching bit to be taken from this register rather than from the CLUSTERTHREADSID register.
    - Bits[19:31] are read as zero
*/
void write_CLUSTERTHREADSIDOVR_EL1(uint32_t value)
{
     asm volatile (
        "msr S3_0_C15_C4_7, %0\n"
        :
        : "r" (value)
        : "memory"
    );
}

/*
    [CLUSTERACPSID_EL1]

    - Bits[0:2] are used to set the schemeID for ACP transactions
    - Bits[3:31] are read as zero

*/
void write_CLUSTERACPSID_EL1(uint32_t value)
{
    asm volatile (
        "msr S3_0_C15_C4_1, %0\n"
        :
        : "r" (value)
        : "memory"
    );

}

/*
    [CLUSTERTHREADSID_EL1] 

    - Bits[0:2] are used to set the schemeID for the current thread
    - Bits[3:31] are read as zero
*/
void write_CLUSTERTHREADSID_EL1(uint32_t value)
{   
    asm volatile (
        "msr S3_0_C15_C4_0, %0\n"
        :
        : "r" (value)
        : "memory"
    );
}

/*
    [CLUSTERTHREADSID_EL1] 

    - Bits[0:2] are used to set the schemeID for the current thread
    - Bits[3:31] are read as zero
*/
uint32_t read_CLUSTERTHREADSID_EL1(void)
{   
    uint32_t value;
    asm volatile (
        "mrs %0, S3_0_C15_C4_0\n"
        : "=r" (value)
    );
    return value;
}



/*
    [CPUECTLR_EL1]
    See: https://developer.arm.com/documentation/100798/0401/vrj1494872408498
    * This register can only be written to when the system is idle *
*/
uint64_t read_CPUECTLR_EL1(void)
{
    uint64_t value;
    asm volatile (
        "mrs %0, cpuectlr_el1\n"
        : "=r" (value)
    );
    return value;
}




#define DEVICE_NAME "way-part"
#define IOCTL_WRITE_CLUSTERPARTCR _IO('k', 1)
#define IOCTL_WRITE_CLUSTERSTASHSID _IO('k', 2)
#define IOCTL_WRITE_CLUSTERTHREADSIDOVR _IO('k', 3)
#define IOCTL_WRITE_CLUSTERACPSID _IO('k', 4)
#define IOCTL_WRITE_CLUSTERTHREADSID _IO('k', 5)
#define IOCTL_READ_CLUSTERPARTCR _IO('k', 6)
#define IOCTL_READ_CLUSTERTHREADSID _IO('k', 10)
#define IOCTL_READ_CPUECTLR _IO('k', 11)

static int major_number;
static struct cdev my_cdev;

static int device_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static long IOCTL_Dispatch(struct file *file, unsigned int cmd, unsigned long arg) {
    struct ioctl_data data;
    int err;
    if ((err = copy_from_user(&data, (struct ioctl_data *)arg, sizeof(struct ioctl_data)))) 
        return -EFAULT; // Error copying data


    switch (cmd) {
        case IOCTL_WRITE_CLUSTERPARTCR:
        {
            printk(KERN_INFO "IOCTL_WRITE_CLUSTERPARTCR called\n");
            write_CLUSTERPARTCR_EL1(data.in_value); 
            break;
        }
        case IOCTL_WRITE_CLUSTERSTASHSID:
        {
            printk(KERN_INFO "IOCTL_WRITE_CLUSTERSTASHSID called\n");
            write_CLUSTERSTASHSID_EL1(data.in_value); 
            break;
        }
        case IOCTL_WRITE_CLUSTERTHREADSIDOVR:
        {
            printk(KERN_INFO "IOCTL_WRITE_CLUSTERTHREADSIDOVR called\n");
            write_CLUSTERTHREADSIDOVR_EL1(data.in_value); 
            break;
        }
        case IOCTL_WRITE_CLUSTERACPSID:
        {
            printk(KERN_INFO "IOCTL_WRITE_CLUSTERACPSID called\n");
            write_CLUSTERACPSID_EL1(data.in_value); 
            break;
        }
        case IOCTL_WRITE_CLUSTERTHREADSID:
        {
            printk(KERN_INFO "IOCTL_WRITE_CLUSTERTHREADSID called\n");
            write_CLUSTERTHREADSID_EL1(data.in_value); 
            break;
        }
        case IOCTL_READ_CLUSTERPARTCR:
        {
            printk(KERN_INFO "IOCTL_READ_CLUSTERPARTCR called\n");
            uint32_t value = read_CLUSTERPARTCR_EL1();
            if(!copy_to_user(data.out_value_low, &value, sizeof(value)))
                printk(KERN_INFO "copy_to_user() failed\n");
            break;
        }
        case IOCTL_READ_CLUSTERTHREADSID:
        {
            printk(KERN_INFO "IOCTL_READ_CLUSTERTHREADSID called\n");
            uint32_t value = read_CLUSTERTHREADSID_EL1();
            if (!copy_to_user(data.out_value_low, &value, sizeof(value)))
                printk(KERN_INFO "copy_to_user() failed\n");
            break;
        }
        case IOCTL_READ_CPUECTLR:
        {
            printk(KERN_INFO "IOCTL_READ_CPUECTLR called\n");
            uint64_t value = IOCTL_READ_CPUECTLR();
            uint32_t high = ((value >> 32) & __UINT32_MAX__);
            uint32_t low = (value & __UINT32_MAX__);
            if (!copy_to_user(data.out_value_low, &low, sizeof(low)) || !!copy_to_user(data.out_value_high, &high, sizeof(high)))
                printk(KERN_INFO "copy_to_user() failed\n");
            break;
        }
        default:
            return -EINVAL; // Invalid command
    }
    return 0;
}

static struct file_operations fops = {
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = IOCTL_Dispatch,
};

static int __init DriverLoad(void) {
    dev_t dev;
    int result;
    printk("DriverLoad() entry.\n");
    // Allocate device number
    result = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (result < 0) {
        printk(KERN_ERR "Failed to allocate device number\n");
        return result;
    }

    major_number = MAJOR(dev);

    // Initialize cdev structure
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // Add character device to the system
    result = cdev_add(&my_cdev, dev, 1);
    if (result < 0) {
        printk(KERN_ERR "Failed to add device to the system\n");
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        return result;
    }




    // Create device class
    struct class* my_class = class_create("way_part_class");
    if (IS_ERR(my_class)) {
        printk(KERN_ERR "Failed to create class\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(my_class);
    }

    // Create device node
    if (IS_ERR(device_create(my_class, NULL, dev, NULL, DEVICE_NAME))) {
        printk(KERN_ERR "Failed to create device node\n");
        class_destroy(my_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        return -1;
    }




    printk(KERN_INFO "DriverLoad() successful.\n");

    return 0;
}

static void __exit DriverUnload(void) {
    // Remove character device
    cdev_del(&my_cdev);

    // Release allocated device number
    unregister_chrdev_region(MKDEV(major_number, 0), 1);

    printk(KERN_INFO "Driver unloaded successfully\n");
}



module_init(DriverLoad);
module_exit(DriverUnload);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cole Strickler");
MODULE_DESCRIPTION("Cache Way Partitioning for RaspberryPi5");
