A  kernel module imitating a firewall

For user-space interaction:

display: Sends to the user statistics of the firewall
modify: Reset all statistics

Initialization at fw_init:
  Registering 3 netfilter phases (In, Out, Forward) with a single hook function that handles them.
  The init function handles errors in regestration by returning a proper value and unregestiring the hooks

Exit at fw_exit:
  Unregisters registered hooks

Hook module_hook:
  Simply rejects packets that were routed to LOCAL_IN and LOCAL_OUT (i.e packets coming to and from the firewall)
  Accepts packets routed to INET_FORWARD (i.e. packets destined for another host through the firewall)
  In case the state->hook field is nullish accepts (for protocols other than IP)


Resources:
  Module boilerplate - ChatGPT
  Netfilter documentations - ChatGPT
  Sysfs - Moodle