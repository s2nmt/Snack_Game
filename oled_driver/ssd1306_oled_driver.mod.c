#include <linux/build-salt.h>
#include <linux/module.h>
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
	{ 0x6b05880a, "module_layout" },
	{ 0x96672421, "cdev_del" },
	{ 0x1265642, "kmalloc_caches" },
	{ 0x9ac6e719, "cdev_init" },
	{ 0xf9a482f9, "msleep" },
	{ 0xfd43d459, "latent_entropy" },
	{ 0xc6a08918, "i2c_del_driver" },
	{ 0xf0364eaf, "device_destroy" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x5f754e5a, "memset" },
	{ 0xc5850110, "printk" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x84b183ae, "strncmp" },
	{ 0xe506116a, "device_create" },
	{ 0xe91a4f4e, "i2c_register_driver" },
	{ 0x11770fa4, "cdev_add" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0xc30e95b, "kmem_cache_alloc_trace" },
	{ 0xe0d27762, "i2c_transfer_buffer_flags" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x37a0cba, "kfree" },
	{ 0xd508c36f, "class_destroy" },
	{ 0x1b0dae34, "__class_create" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
};

MODULE_INFO(depends, "");

