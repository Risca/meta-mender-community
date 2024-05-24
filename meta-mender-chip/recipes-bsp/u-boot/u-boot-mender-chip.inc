FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "\
        file://0001-kbuild-add-include-linux-kconfig.h-in-config.h.patch \
        file://0002-Allow-providing-default-environment-from-file.patch \
        file://0003-mender_chip_config.patch \
        file://fw_env.config.default \
"

MENDER_UBOOT_AUTO_CONFIGURE = "0"
BOOTENV_SIZE = "0x400000"

MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_1 = "0x0C00000"
MENDER_UBOOT_ENV_STORAGE_DEVICE_OFFSET_2 = "0x1000000"
