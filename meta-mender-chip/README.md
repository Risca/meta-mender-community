# meta-mender-chip

Mender integration for NextThingCo's C.H.I.P. boards

The supported and tested on:

 - [C.H.I.P.](https://docs.getchip.cc/chip/)

Possibly working, but untested on:

 - [C.H.I.P. Pro](https://docs.getchip.cc/chip_pro/)

## Dependencies

This layer depends on:

```
URI: https://github.com/Risca/meta-chip
branch: kirkstone
revision: HEAD
```

```
URI: https://github.com/mendersoftware/meta-mender.git
layers: meta-mender-core
branch: kirkstone
revision: HEAD
```

## Quick start

Follow the instruction on at the link below to build images with Mender integrated. Use the `kas/chip.yml` file.

 - [meta-mender-community Quick start](https://github.com/mendersoftware/meta-mender-community?tab=readme-ov-file#quick-start)

While it builds, follow the instructions to download and build the "flashing" and `chip-tools` from here:

 - [meta-chip flash procedure](https://github.com/Risca/meta-chip/tree/kirkstone?tab=readme-ov-file#using)

You don't have to follow the Yocto instructions beyond step 3.

When the build has finished, copy `sunxi-spl.bin`, `u-boot-dtb.bin `, and `core-image-minimal-chip.ubimg` to a separate folder.
Rename `core-image-minimal-chip.ubimg` to `rootfs.ubi` and flash using the following command:

	./chip-flash-chip.sh <folder>

## Hardware

The C.H.I.P. single board computer by NextThingCo consists of, among other things:

* Allwinner R8 SoC ARM processor
* 4-8 GB of onboard MLC NAND
* Realtek RTL8723BS Wi-Fi

The above is of most interest for integrating Mender.

The Allwinner R8 also goes under the name sun5i or sunxi.

The NAND chip is a multi-level cell (MLC) type NAND, which is quite unusual to find on a board.
Typically, they are found in SSDs with a dedicated controller.

A built-in wifi chip provide for connectivity to the outside world.

## Software considerations

There are mainly two pieces of software of interest for booting and running the C.H.I.P.:

* The u-boot bootloader
* The Linux kernel

Both of the above got frozen in time when NextThingCo disappeared.
The last known working version of the above seem to be u-boot 2016.1 and Linux-4.4.30.
Copies of these repos can be found at multiple places online.

This integration uses the [meta-chip](https://github.com/Risca/meta-chip) yocto layer to provide the board support.
It's a rather small layer that mainly provide u-boot, Linux, and wifi drivers.

There are a few decisions points along the way of supporting the C.H.I.P. discusses in further detail below.

### u-boot version

The last known version of u-boot for the C.H.I.P. is rather old.
However, switching to a more modern version of u-boot would require porting the board support that has been added specifically for the C.H.I.P.
Based on the amount of commits, this was deemed non-trivial and the old u-boot version was kept as is.

Some backporting was done to make the Mender patches apply, albeit with a little bit of fuzz.
Lastly, a patch is added for integrating the Mender boot flow.

### kernel version

As with the C.H.I.P.'s bootloader, the kernel is also quite old.
While switching to a more modern kernel would certainly be feasable, there is one rather big obstacle with this: starting with Linux-4.17 the ubi layer was made incompatible with MLC NAND.
In Linux-5.8, a workaround was added that allow MLC NAND, emulated as SLC NAND, to be used with ubi but that would leave at least half the storage unavailable.
As a side note, the mail that introduced this change mentions that it was tested with a C.H.I.P.
It's unclear whether this would also require changes to u-boot, but it's likely.

### Flash layout

The default u-boot and Linux kernel use the following flash layout

    +---------+---------+---------+---------+---...---+
    |  SPL-1  |  SPL-2  | U-BOOT  |   ENV   |   UBI   |
    +---------+---------+---------+---------+---...---+
    0        4MB       8MB      12MB      16MB       4GB


Where
* `SPL-1` is a first stage bootloader
* `SPL-2` is a backup of `SPL1-`
* `U-BOOT` is a second stage bootloader
* `ENV` is where u-boot keep its environment variables
* `UBI` contains the kernel and root file system

Mender uses the u-boot environment as a way to communicate with the bootloader during an system update.
As such, it is crucial that it can be trusted.
However, there is always a risk that the environment gets corrupted during a sudden power loss.
Therefore, u-boot can be configured to use dual environment blocks so that there's always a working fallback.

This prompted the following change in flash layout:

    +---------+---------+---------+---------+---------+---...---+
    |  SPL-1  |  SPL-2  | U-BOOT  |  ENV-1  |  ENV-2  |   UBI   |
    +---------+---------+---------+---------+---------+---...---+
    0        4MB       8MB      12MB      16MB      20MB       4GB

Now, the Mender documentation recommends keeping dual environments as two separate volumes in the UBI partition.
However, this use case is not supported by the u-boot version available and would require additional work on the bootloader.
The Yocto support for Mender currently adds these two volumes to the UBI partition during build, but they are unused on the C.H.I.P.
Future improvements could fix the bootloader and instead ignore the ENV partition(s).

### Bootstrapping

Before Mender can be used to update the board, it needs to get an initial software flashed somehow.
This is done with the help of a couple of scripts and the sunxi-tools, but the device needs to be in a special mode for this to work.
By connecting GND to the FEL pin, a special boot mode i entered and software can be side loaded.

The chip-tools repo contain a small script that transfers spl, u-boot, a tiny u-boot script, and the rootfs image to RAM.
It then jumps to u-boot, which runs the script and commits the rootfs image from RAM to flash.
This bootstrap script also resets the environment and finally runs `boot`.

The original version of this script would overwrite a few of the bootloader environments during flashing and had to be modified.
It was also updated so it writes the ubi image to the correct offset in flash.
If the bootloader would be updated so it can read/write its environment from a ubi volume instead, the offset can be reverted to the original.
