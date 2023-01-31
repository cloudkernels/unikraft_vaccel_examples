## Examples of vAccel in Unikrafy

A set of examples to test vAccel in Unikraft.

The vaccel\_config file contains a config for KVM platform which enables the following libraries:
- vAccelrt (the virtio accel driver gets enabled too)
- 9pfs, so we can pass the image to the unikernel
- vfscore with 9pfs as root filesystem
- devfs which gets mounted during boot , for /dev/accel

### Building

Let's assume, that we follow the typical paradigm in a Unikraft workspace, where the directory tree looks like the following one
```
$ tree -L 2
.
├── apps
│   └── unikraft_vaccel_examples
├── libs
│   └── lib-newlib
└── unikraft
    ├── arch
    ├── Config.uk
    ├── CONTRIBUTING.md
    ├── COPYING.md
    ├── include
    ├── lib
    ├── Makefile
    ├── Makefile.uk
    ├── plat
    ├── README.md
    ├── support
    └── version.mk

10 directories, 8 files
```
In order to build an application you can follow the below instructions:
```
$ cp vaccel_config .config
$ make olddefconfig
$ make menuconfig
```

In the menuconfig, under the `Application_Options`, the user can shoose one of the available examples.

NOTE: Only one example can be used. In case that, more than one examples are slected the first one in the list will be built.

After selecting the example, the user can simply build the unikernel, with the `make` command.

### Executing

Afer building a Unikraft example you can run it using the following command.

```
LD_LIBRARY_PATH=$PATH_TO_VACCELRT:$PATH_TO_EXTERNAL_LIBRARIES VACCEL_BACKENDS=$PATH_TO_VACCEL_PLUGIN qemu-system-x86_64 \
-cpu host -m 512 -enable-kvm -nographic -vga none \
-fsdev local,id=myid,path=$PATH_TO_DIRECTORY_CONTAINING_DATA,security_model=none -device virtio-9p-pci,fsdev=myid,mount_tag=data,disable-modern=on,disable-legacy=off \
-object acceldev-backend-vaccelrt,id=gen0 -device virtio-accel-pci,id=accl0,runtime=gen0,disable-legacy=off,disable-modern=on \
-kernel $PATH_TO_UNIKRAFT_IMAGE -append "vfs.rootdev=data -- $EXAMPLE_ARGS"
```

where:
- PATH\_TO\_VACCELRT is the directory where vAccel has been installed
- PATH\_TO\_EXTERNAL\_LIBRARIES is the directory where the external libraries have been installed (e.g. `/usr/local/lib:/usr/local/nvidia/lib`)
- PATH\_TO\_VACCEL\_PLUGIN is the directory where the vAccel plugin has been installed.
- PATH\_TO\_DIRECTORY\_CONTAINING\_DATA is the directory which contains the data for the example
- PATH\_TO\_UNIKRAFT\_IMAGE, is the path to the unikernel image
- $EXAMPLE\_ARGS, The arguments for the example application

