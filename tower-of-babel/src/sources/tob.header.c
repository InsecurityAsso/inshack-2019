#include <linux/init.h>                 /* Macros used to mark up functions e.g. __init __exit */
#include <linux/module.h>               /* Core header for loading LKMs into the kernel */
#include <linux/device.h>               /* Header to support the kernel Driver Model */
#include <linux/kernel.h>               /* Contains types, macros, functions for the kernel */
#include <linux/fs.h>                   /* Header for the Linux file system support */
#include <linux/uaccess.h>              /* Required for the copy to user function */

#define  CLASS_NAME  "tob_class"        /* The device class -- this is a character device driver */
#define  DEVICE_NAME "tob"       /* The device will appear at /dev/tower_of_babel using this value */

MODULE_LICENSE("GPL");                                  /* The license type -- this affects available functionality */
MODULE_AUTHOR("koromodako");                            /* The author -- visible when you use modinfo */
MODULE_DESCRIPTION("TowerOfBabel character device");    /* The description -- see modinfo */
MODULE_VERSION("0.1.0");                                /* A version number to inform users */

#define MAX_BUF_SIZE    512
#define MAX_INPUT_SIZE  256
#define ERASE_BUF(buf) \
    buf.size=0; \
    memset(buf.data, 0, MAX_BUF_SIZE)

typedef struct buf {
    int  size;
    char data[MAX_BUF_SIZE];
} buf_t;

/* -----BEGIN INSERT----- */
