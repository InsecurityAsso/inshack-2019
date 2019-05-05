/* -----END INSERT----- */

static int gmajor;               /* Stores the device number -- determined automatically */
static const char *gempty_str="";
static buf_t gintbuf={
    .size=0,
    .data={0}
};                    /* Internal buffer */
static int gopen_cnt=0;          /* Counts the number of times the device is opened */
static int grelease_cnt=0;       /* Counts the number of times the device is released */
static struct class* gtob_cdev_cls=NULL;   /* The device-driver class struct pointer */
static struct device* gtob_cdev_dev=NULL;   /* The device-driver device struct pointer */
/* state-related indexes */
static int gstage_idx=0;
static int gstage_done=0;
/*
 * Character device functions
 */
static int     tob_open(struct inode *, struct file *);
static int     tob_release(struct inode *, struct file *);
static ssize_t tob_read(struct file *, char *, size_t, loff_t *);
static ssize_t tob_write(struct file *, const char *, size_t, loff_t *);
/*
 * Util functions
 */
static void tob_xor(char *buf, int bsz, const char *key, int ksz);
static const char* tob_load_trace(int id, const char* level);
static const char* tob_load_resource(int id);
/*
 * Devices are represented as file structure in the kernel. The file_operations structure from
 * /linux/fs.h lists the callback functions that you wish to associated with your file operations
 * using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations gtob_file_ops={
    .open=tob_open,
    .read=tob_read,
    .write=tob_write,
    .release=tob_release,
};
/* The LKM initialization function
 *
 * The static keyword restricts the visibility of the function to within this C file. The __init
 * macro means that for a built-in driver (not a LKM) the function is only used at initialization
 * time and that it can be discarded and its memory freed up after that point.
 *
 * Returns 0 if successful
 */
static int __init tob_init(void)
{
    printk(tob_load_trace(0, KERN_INFO));
    /* try to dynamically allocate a major number for the device -- more difficult but worth it */
    gmajor=register_chrdev(0, DEVICE_NAME, &gtob_file_ops);
    if (gmajor<0){
       printk(tob_load_trace(1, KERN_ALERT));
       return gmajor;
    }
    printk(tob_load_trace(2, KERN_INFO), gmajor);
    /* register the device class */
    gtob_cdev_cls = class_create(THIS_MODULE, CLASS_NAME);
    /* check for error and clean up if there is */
    if (IS_ERR(gtob_cdev_cls)){
       unregister_chrdev(gmajor, DEVICE_NAME);
       printk(tob_load_trace(3, KERN_ALERT));
       /* correct way to return an error on a pointer */
       return PTR_ERR(gtob_cdev_cls);
    }
    printk(tob_load_trace(4, KERN_INFO));
    /* register the device driver */
    gtob_cdev_dev = device_create(gtob_cdev_cls, NULL, MKDEV(gmajor, 0), NULL, DEVICE_NAME);
    /* clean up if there is an error */
    if (IS_ERR(gtob_cdev_dev)){
       /* repeated code but the alternative is goto statements */
       class_destroy(gtob_cdev_cls);
       unregister_chrdev(gmajor, DEVICE_NAME);
       printk(tob_load_trace(5, KERN_ALERT));
       return PTR_ERR(gtob_cdev_dev);
    }
    /* made it! device was initialized */
    printk(tob_load_trace(6, KERN_INFO));
    return 0;
}
/*
 * The LKM cleanup function
 * Similar to the initialization function, it is static. The __exit macro notifies that if this
 * code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit tob_cleanup(void)
{
    /* remove the device */
    device_destroy(gtob_cdev_cls, MKDEV(gmajor, 0));
    /* unregister the device class */
    class_unregister(gtob_cdev_cls);
    /* remove the device class */
    class_destroy(gtob_cdev_cls);
    /* unregister the major number */
    unregister_chrdev(gmajor, DEVICE_NAME);
    printk(tob_load_trace(7, KERN_INFO));
}
/*
 * The device open function that is called each time the device is opened
 * This will only increment the gopen_cnt counter in this case.
 * [in] inodep: A pointer to an inode object (defined in linux/fs.h)
 * [in] filep:  A pointer to a file object (defined in linux/fs.h)
 */
static int tob_open(struct inode *inodep, struct file *filep)
{
    gopen_cnt++;
    printk(tob_load_trace(8, KERN_INFO), gopen_cnt);
    return 0;
}
/*
 * This function is called whenever device is being read from user space i.e. data is
 * being sent from the device to the user. In this case is uses the copy_to_user() function to
 * send the buffer string to the user and captures any errors.
 * [in] filep:  A pointer to a file object (defined in linux/fs.h)
 * [in] buffer: The pointer to the buffer to which this function writes the data
 * [in] len:    The length of the b
 * [in] offset: The offset if required
 */
#define MIN(a,b) ((a)<(b)?(a):(b))
static ssize_t tob_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int size=0;
    int error_cnt=0;
    if(len<=0) {
        return 0;
    }
    /* copy_to_user has the format (*to, *from, size) and returns 0 on success */
    tob_load_resource(gstage_done?gcsuccess:gcfailure);
    size=MIN(gintbuf.size, len);
    error_cnt=copy_to_user(buffer, gintbuf.data, size);
    /* if true then have success */
    if (error_cnt==0){
        printk(tob_load_trace(9, KERN_INFO), size);
        /* clear the position to the start and return 0 */
        return size;
    }
    printk(tob_load_trace(10, KERN_INFO), error_cnt);
    /* failed -- return a bad address message (i.e. -14) */
    return -EFAULT;
}
#undef MIN
/*
 * This function is called whenever the device is being written to from user space i.e.
 * data is sent to the device from the user. The data is copied to the message[] array in this
 * LKM using the sprintf() function along with the length of the string.
 * [in] filep:  A pointer to a file object
 * [in] buffer: The buffer to that contains the string to write to the device
 * [in] len:    The length of the array of data that is being passed in the const char buffer
 * [in] offset: The offset if required
 */
static ssize_t tob_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    if(len>MAX_BUF_SIZE-1) {
        return -EFBIG;
    }
    printk(tob_load_trace(11, KERN_INFO), len);
    tob_load_trace(gbase_idx+gstage_idx, NULL);
    if(!memcmp(gintbuf.data, buffer, gintbuf.size)) {
        printk(tob_load_trace(12, KERN_INFO));
        gstage_idx++;
        if(gbase_idx+gstage_idx>=gresd_sz) {
            printk(tob_load_trace(13, KERN_INFO));
            gstage_idx=0;
            gstage_done=1;
        }
    } else {
        printk(tob_load_trace(14, KERN_INFO));
        gstage_idx=0;
        gstage_done=0;
    }
    return len;
}
/*
 * The device release function that is called whenever the device is
 * closed/released by the userspace program
 * [in] inodep: A pointer to an inode object (defined in linux/fs.h)
 * [in] filep:  A pointer to a file object (defined in linux/fs.h)
 */
static int tob_release(struct inode *inodep, struct file *filep)
{
    grelease_cnt++;
    printk(tob_load_trace(15, KERN_INFO), grelease_cnt);
    return 0;
}
/*
 *
 */
static void tob_xor(char *buf, int bsz, const char *key, int ksz)
{
    int i;
    for(i=0;i<bsz;i++) {
        buf[i]^=key[i%ksz];
    }
}
/*
 *
 */
static const char* tob_load_trace(int id, const char *level)
{
    int res_sz;
    int start=(level==NULL?0:1);
    ERASE_BUF(gintbuf);
    if(id>gresd_sz) {
        return gempty_str;
    }
    res_sz=gresd_data[id].size;
    if(res_sz<MAX_BUF_SIZE-1) {
        memcpy(gintbuf.data+start, gresd_data[id].data, res_sz);
        tob_xor(gintbuf.data+start, res_sz, gresk.data, gresk.size);
    } else {
        return gempty_str;
    }
    if(start) {
        gintbuf.data[0]=level[0];
    }
    /* ensure null byte at the end */
    gintbuf.data[MAX_BUF_SIZE-1]=0;
    gintbuf.size=strlen(gintbuf.data);
    return gintbuf.data;
}
/*
 *
 */
static const char* tob_load_resource(int id)
{
    int res_sz;
    ERASE_BUF(gintbuf);
    res_sz=gcresd[id].size;
    if(res_sz<MAX_BUF_SIZE) {
        memcpy(gintbuf.data, gcresd[id].data, res_sz);
        tob_xor(gintbuf.data, res_sz, gresk.data, gresk.size);
    } else {
        return gempty_str;
    }
    /* ensure null byte at the end */
    gintbuf.data[MAX_BUF_SIZE-1]=0;
    gintbuf.size=strlen(gintbuf.data);
    return gintbuf.data;
}
/*
 * A module must use the module_init() module_exit() macros from linux/init.h,
 * which identify the initialization function at insertion time and the cleanup
 * function (as listed above)
 */
module_init(tob_init);
module_exit(tob_cleanup);
