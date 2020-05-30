#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include "button.h"


#define DRV_NAME "bsp_button"

struct imx6ull_gpio {
    volatile unsigned int dr;
    volatile unsigned int gdir;
    volatile unsigned int psr;
    volatile unsigned int icr1;
    volatile unsigned int icr2;
    volatile unsigned int imr;
    volatile unsigned int isr;
    volatile unsigned int edge_sel;
};

static volatile unsigned int *CCM_CCGR3;
static volatile unsigned int *CCM_CCGR1;

static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1;
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B;

static struct imx6ull_gpio *gpio4;
static struct imx6ull_gpio *gpio5;


static void imx6ull_init(int which)
{
    if (!CCM_CCGR1) {
        CCM_CCGR1 = ioremap(0x20C406C, 4);
        CCM_CCGR3 = ioremap(0x20C4074, 4);
        IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 = ioremap(0x229000C, 4);
        IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B        = ioremap(0x20E01B0, 4);
        gpio4 = ioremap(0x020A8000, sizeof(struct imx6ull_gpio));
        gpio5 = ioremap(0x20AC000, sizeof(struct imx6ull_gpio));
    }

    *CCM_CCGR1 |= (3<<30);
    *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 = 5;
    gpio5->gdir &= ~(1<<1);
    *CCM_CCGR3 |= (3<<12);
    *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B = 5;
    gpio4->gdir &= ~(1<<14);
}

static int imx6ull_read(int which)
{
    int key8 = (gpio5->psr & (1<<1));
    int key9 = (gpio4->psr & (1<<14));
    pr_info("imx6ull_read, key8:%d, key9:%d\n", key8, key9);
    return (key8 << 1) | key9;
}

static struct button_operations imx6ull_ops = {
    .count = 2,
    .init = imx6ull_init,
    .read = imx6ull_read,
};

static struct button_operations *button_operations = NULL;
static struct fasync_struct *b_async = NULL;

static int button_open(struct inode *inode, struct file *file)
{
    int minor = iminor(inode);
    if (button_operations && button_operations->init) {
        button_operations->init(minor);
    }
    return 0;
}
static ssize_t button_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
    unsigned int minor = iminor(file_inode(file));
    char level = 0;
    int ret = 0;

    if (button_operations && button_operations->read) {
        level = button_operations->read(minor);
    }
    ret = copy_to_user(buf, &level, 1);
    *ppos += 1;

    kill_fasync(&b_async, SIGIO, POLL_IN);
    kill_fasync(&b_async, SIGIO, POLL_MSG);

    return *ppos == 0 ? 1 :0;
}

static int button_fasync(int fd, struct file *file, int on)
{
    pr_info("FASYNC\n");
    return fasync_helper(fd, file, on, &b_async);
}
#if 0
static unsigned int button_poll(struct file *, struct poll_table_struct *)
{

}
//static //int (*fsync) (struct file *, loff_t, loff_t, int datasync);
#endif

static struct file_operations button_fops = {
    .open = button_open,
    .read = button_read,
    .fasync = button_fasync,
#if 0
    .poll = button_poll,
#endif
};

static struct miscdevice miscdev = {
    .name = DRV_NAME,
    .fops = &button_fops,
};

int button_init(void)
{
    int ret = 0;

    ret = misc_register(&miscdev);
    if (ret) {
        pr_err("[%s] misc_register fail\n", __func__);
        return ret;
    }

    return 0;
}


void button_exit(void)
{
    misc_deregister(&miscdev);
}

module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");







