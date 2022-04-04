#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x9de7765d, "module_layout" },
	{ 0x8be6f4b1, "seq_release" },
	{ 0xcf379fb1, "seq_lseek" },
	{ 0x4ee79e32, "seq_read" },
	{ 0x67d54299, "remove_proc_entry" },
	{ 0x2b68bd2f, "del_timer" },
	{ 0x4aba4bc5, "proc_create" },
	{ 0x977f511b, "__mutex_init" },
	{ 0xdf9208c0, "alloc_workqueue" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x813e9f89, "proc_mkdir" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x37a0cba, "kfree" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x754d539c, "strlen" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xebbe12f0, "current_task" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xb5f17439, "kmem_cache_alloc_trace" },
	{ 0xcbf895e0, "kmalloc_caches" },
	{ 0xc5850110, "printk" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0xff1e9dd8, "seq_list_start" },
	{ 0xe7d4daac, "seq_list_next" },
	{ 0xd1f85b73, "seq_printf" },
	{ 0x2f495e48, "seq_open" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x409bcb62, "mutex_unlock" },
	{ 0x2db3d320, "mutex_lock_interruptible" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "59450D7BD3D1DAF47B7FB2A");
