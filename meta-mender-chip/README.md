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

Once it finishes, follow the instructions here to flash the images:

 - [meta-chip flash procedure](https://github.com/Risca/meta-chip?tab=readme-ov-file#chip)

Use the `core-image-minimal-chip.ubimg` file as `rootfs.ubi`.
