#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf63d6aa7, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xf22abc34, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0x2f266a81, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x83e7dc26, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x27cd5130, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0xa05f372e, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xe914e41e, __VMLINUX_SYMBOL_STR(strcpy) },
	{ 0x6213ce37, __VMLINUX_SYMBOL_STR(init_task) },
	{ 0x37befc70, __VMLINUX_SYMBOL_STR(jiffies_to_msecs) },
	{ 0x7d11c268, __VMLINUX_SYMBOL_STR(jiffies) },
	{ 0xf21ab60f, __VMLINUX_SYMBOL_STR(seq_open) },
	{ 0xcfb4262, __VMLINUX_SYMBOL_STR(seq_release) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DC0B9CCC5FD1A9E4BEE9F25");
