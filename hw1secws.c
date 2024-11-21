#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>
#include <linux/fs.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Razaq");
MODULE_DESCRIPTION("Basic Packet Filtering");
MODULE_VERSION("1");


// Netfilter hooks for relevant packet phases
static struct nf_hook_ops netfilter_ops_in;
static struct nf_hook_ops netfilter_ops_out;
static struct nf_hook_ops netfilter_ops_fw;


static int major_number;
static struct class* sysfs_class = NULL;
static struct device* sysfs_device = NULL;

static unsigned int sysfs_int = 0;


static struct file_operations fops = {
	.owner = THIS_MODULE
};

ssize_t display(struct device *dev, struct device_attribute *attr, char *buf)	//sysfs show implementation
{
	return scnprintf(buf, PAGE_SIZE, "%u\n", sysfs_int);
}

ssize_t modify(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)	//sysfs store implementation
{
	int temp;
	if (sscanf(buf, "%u", &temp) == 1)
		sysfs_int = temp;
	return count;	
}

static DEVICE_ATTR(sysfs_att, S_IWUSR | S_IRUGO , display, modify);

// A hook function used for the 3 relevan phases (In, Out, Through)
static unsigned int module_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    if (state->hook == NF_INET_LOCAL_IN || state->hook == NF_INET_LOCAL_OUT) {
        printk(KERN_INFO " *** Packet Dropped ***\n");
        return NF_DROP;
    } else if (state->hook == NF_INET_FORWARD) {
        printk(KERN_INFO "*** Packet Accepted ***");
        return NF_ACCEPT;
    }
    return NF_ACCEPT;
}


// Initialization function; handles error registering the hooks with cleanups and an indicative return value
static int __init fw_init(void) {
    int ret;
    printk(KERN_INFO "Loading hw1secws module...\n");

    // Set up the Netfilter hook for incoming packets
    netfilter_ops_in.hook = module_hook;
    netfilter_ops_in.pf = PF_INET;
    netfilter_ops_in.hooknum = NF_INET_LOCAL_IN;
    netfilter_ops_in.priority = NF_IP_PRI_FIRST;

    ret = nf_register_net_hook(&init_net, &netfilter_ops_in);
    if (ret) {
        printk(KERN_ERR "hw1secws: Failed to register incoming hook. Error: %d\n", ret);
        return ret;
    }

    // Set up the Netfilter hook for outgoing packets
    netfilter_ops_out.hook = module_hook;
    netfilter_ops_out.pf = PF_INET;
    netfilter_ops_out.hooknum = NF_INET_LOCAL_OUT;
    netfilter_ops_out.priority = NF_IP_PRI_FIRST;

    ret = nf_register_net_hook(&init_net, &netfilter_ops_out);
    if (ret) {
        printk(KERN_ERR "hw1secws: Failed to register outgoing hook. Error: %d\n", ret);
        nf_unregister_net_hook(&init_net, &netfilter_ops_in); // Cleanup
        return ret;
    }

    // Set up the Netfilter hook for forwarding packets
    netfilter_ops_fw.hook = module_hook;
    netfilter_ops_fw.pf = PF_INET;
    netfilter_ops_fw.hooknum = NF_INET_FORWARD;
    netfilter_ops_fw.priority = NF_IP_PRI_FIRST;

    ret = nf_register_net_hook(&init_net, &netfilter_ops_fw);
    if (ret) {
        printk(KERN_ERR "hw1secws: Failed to register forwarding hook. Error: %d\n", ret);
        nf_unregister_net_hook(&init_net, &netfilter_ops_in); // Cleanup
        nf_unregister_net_hook(&init_net, &netfilter_ops_out); // Cleanup
        return ret;
    }
    

    //create char device
	major_number = register_chrdev(0, "Sysfs_Device", &fops);\
	if (major_number < 0)
		return -1;
		
	//create sysfs class
	sysfs_class = class_create(THIS_MODULE, "Sysfs_class");
	if (IS_ERR(sysfs_class))
	{
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}
	
	//create sysfs device
	sysfs_device = device_create(sysfs_class, NULL, MKDEV(major_number, 0), NULL, "sysfs_class" "_" "sysfs_Device");	
	if (IS_ERR(sysfs_device))
	{
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}
	
	//create sysfs file attributes	
	if (device_create_file(sysfs_device, (const struct device_attribute *)&dev_attr_sysfs_att.attr))
	{
		device_destroy(sysfs_class, MKDEV(major_number, 0));
		class_destroy(sysfs_class);
		unregister_chrdev(major_number, "Sysfs_Device");
		return -1;
	}


    return 0;
}

static void __exit fw_exit(void) {
    printk(KERN_INFO "Removing hw1secws module...\n");

    nf_unregister_net_hook(&init_net, &netfilter_ops_in);
    nf_unregister_net_hook(&init_net, &netfilter_ops_out);
    nf_unregister_net_hook(&init_net, &netfilter_ops_fw);
    device_remove_file(sysfs_device, (const struct device_attribute *)&dev_attr_sysfs_att.attr);
	device_destroy(sysfs_class, MKDEV(major_number, 0));
	class_destroy(sysfs_class);
	unregister_chrdev(major_number, "Sysfs_Device");
}

module_init(fw_init);
module_exit(fw_exit);
