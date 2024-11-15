#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Razaq");
MODULE_DESCRIPTION("Basic Packet Filtering");
MODULE_VERSION("1");

static struct nf_hook_ops netfilter_ops_in;
static struct nf_hook_ops netfilter_ops_out;
static struct nf_hook_ops netfilter_ops_fw;

static unsigned int module_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state) {
    if (state->hook == NF_INET_LOCAL_IN) {
        printk(KERN_INFO "[FW] Incoming packet: Local connection blocked\n");
        return NF_DROP;
    } else if (state->hook == NF_INET_LOCAL_OUT) {
        printk(KERN_INFO "[FW] Outgoing packet: Local connection blocked\n");
        return NF_DROP;
    } else if (state->hook == NF_INET_FORWARD) {
        printk(KERN_INFO "[FW] Forwarding packet: Connection allowed\n");
        return NF_ACCEPT;
    }
    return NF_ACCEPT;
}

static int __init fw_init(void) {
    printk(KERN_INFO "Loading hw1secws module...\n");

    // Set up the Netfilter hook for incoming packets
    netfilter_ops_in.hook = module_hook;
    netfilter_ops_in.pf = PF_INET;
    netfilter_ops_in.hooknum = NF_INET_LOCAL_IN;
    netfilter_ops_in.priority = NF_IP_PRI_FIRST;

    // Set up the Netfilter hook for outgoing packets
    netfilter_ops_out.hook = module_hook;
    netfilter_ops_out.pf = PF_INET;
    netfilter_ops_out.hooknum = NF_INET_LOCAL_OUT;
    netfilter_ops_out.priority = NF_IP_PRI_FIRST;

    // Set up the Netfilter hook for forwarding packets
    netfilter_ops_fw.hook = module_hook;
    netfilter_ops_fw.pf = PF_INET;
    netfilter_ops_fw.hooknum = NF_INET_FORWARD;
    netfilter_ops_fw.priority = NF_IP_PRI_FIRST;

    // Register the hooks
    nf_register_net_hook(&init_net, &netfilter_ops_in);
    nf_register_net_hook(&init_net, &netfilter_ops_out);
    nf_register_net_hook(&init_net, &netfilter_ops_fw);

    return 0;
}

static void __exit fw_exit(void) {
    printk(KERN_INFO "Removing hw1secws module...\n");

    nf_unregister_net_hook(&init_net, &netfilter_ops_in);
    nf_unregister_net_hook(&init_net, &netfilter_ops_out);
    nf_unregister_net_hook(&init_net, &netfilter_ops_fw);
}

module_init(fw_init);
module_exit(fw_exit);
