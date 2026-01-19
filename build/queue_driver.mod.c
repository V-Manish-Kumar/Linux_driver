#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x82c6f73b, "proc_create" },
	{ 0xe8213e80, "_printk" },
	{ 0x003b23f9, "single_open" },
	{ 0xf2c4f3f1, "seq_printf" },
	{ 0xf46d5bf3, "mutex_lock" },
	{ 0xcb8b6ec6, "kfree" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xd710adbf, "__kmalloc_noprof" },
	{ 0xf46d5bf3, "mutex_unlock" },
	{ 0x7851be11, "__SCT__might_resched" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x7a5ffe84, "init_wait_entry" },
	{ 0xd272d446, "schedule" },
	{ 0x0db8d68d, "prepare_to_wait_event" },
	{ 0xc87f4bab, "finish_wait" },
	{ 0x16ab4215, "__wake_up" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x33c78c8a, "remove_proc_entry" },
	{ 0x6fdeeff0, "device_destroy" },
	{ 0x14fcde53, "class_destroy" },
	{ 0x0c72f9ad, "cdev_del" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0xbd4e501f, "seq_read" },
	{ 0xfc8fa4ce, "seq_lseek" },
	{ 0xcb077514, "single_release" },
	{ 0xd272d446, "__fentry__" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xefd5d5d8, "cdev_init" },
	{ 0x4c1dbbd9, "cdev_add" },
	{ 0xea5ac1d9, "class_create" },
	{ 0xf98f93a7, "device_create" },
	{ 0xba157484, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0x82c6f73b,
	0xe8213e80,
	0x003b23f9,
	0xf2c4f3f1,
	0xf46d5bf3,
	0xcb8b6ec6,
	0x092a35a2,
	0xd710adbf,
	0xf46d5bf3,
	0x7851be11,
	0x092a35a2,
	0x7a5ffe84,
	0xd272d446,
	0x0db8d68d,
	0xc87f4bab,
	0x16ab4215,
	0xd272d446,
	0x33c78c8a,
	0x6fdeeff0,
	0x14fcde53,
	0x0c72f9ad,
	0x0bc5fb0d,
	0xbd4e501f,
	0xfc8fa4ce,
	0xcb077514,
	0xd272d446,
	0xd272d446,
	0x9f222e1e,
	0xefd5d5d8,
	0x4c1dbbd9,
	0xea5ac1d9,
	0xf98f93a7,
	0xba157484,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"proc_create\0"
	"_printk\0"
	"single_open\0"
	"seq_printf\0"
	"mutex_lock\0"
	"kfree\0"
	"_copy_from_user\0"
	"__kmalloc_noprof\0"
	"mutex_unlock\0"
	"__SCT__might_resched\0"
	"_copy_to_user\0"
	"init_wait_entry\0"
	"schedule\0"
	"prepare_to_wait_event\0"
	"finish_wait\0"
	"__wake_up\0"
	"__stack_chk_fail\0"
	"remove_proc_entry\0"
	"device_destroy\0"
	"class_destroy\0"
	"cdev_del\0"
	"unregister_chrdev_region\0"
	"seq_read\0"
	"seq_lseek\0"
	"single_release\0"
	"__fentry__\0"
	"__x86_return_thunk\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"cdev_add\0"
	"class_create\0"
	"device_create\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C1A295A33D01A16EA5059A9");
