## Examples of vAccel in Unikraft

A set of examples to test vAccel in Unikraft.

The vaccel\_config file contains a config for KVM platform which enables the following libraries:
- vAccelrt (the virtio accel driver gets enabled too)
- 9pfs, so we can pass the image to the unikernel
- vfscore with 9pfs as root filesystem
- devfs which gets mounted during boot , for /dev/accel

### Building using Docker

The easiest way to build a Vaccel example over Unikraft is to use docker:
```
DOCKER_BUILDKIT=1 docker build --network=host -f Dockerfile -t unikraft_vaccel_example --build-arg "EXAMPLE=noop" --target artifacts --output type=local,dest=./unikraft_images .
```

The above command will create a new directory named `unikraft_images`, which will contain the Unikraft image of `EXAMPLE` argument.

The available examples can be found using the following command:
```
$ bash buil_example.sh list
```

### Building from scratch

Let's assume, that a Unikraft workspace has been created, using [our Unikraft fork](https://github.com/cloudkernels/unikraft/tree/vaccelrt_rel010) and specifically `vaccel_rel010` branch.

The directory tree should look like that:
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
In order to build an application you can use the `build_example.sh script:
```
$ bash build_example.sh <example_name>
```

The user can get the list with the available examples, with the following command:
```
$ bash buil_example.sh list
```

Otherwise, the user can build an application following the below instructions:
```
$ cp vaccel_config .config
$ make olddefconfig
$ make menuconfig
```

In the menuconfig, under the `Application_Options`, the user can shoose one of the available examples.

NOTE: Only one example can be used. In case that, more than one examples are slected the first one in the list will be built.

After selecting the example, the user can simply build the unikernel, with the `make` command.

### Executing

Let's assume that a successful installation of vAccel has taken place, following the steps in vAccel's [docs](https://docs.vaccel.org/binaries/#install-vaccelrt-core-library).

Afer building a Unikraft example you can run it using the `qemu_run.sh` script.

```
bash QEMU_BINARY=<path_to_qemu_binary> qemu_run.sh <DATA> <UNIKERNEL_IMAGE> <APP_ARGS>
```

The `qemu_run.sh` script simply executes the following command:

```
$QEMU_BINARY \
-cpu host -m 512 -enable-kvm -nographic -vga none \
-fsdev local,id=myid,path=$DATA,security_model=none -device virtio-9p-pci,fsdev=myid,mount_tag=data,disable-modern=on,disable-legacy=off \
-object acceldev-backend-vaccelrt,id=gen0 -device virtio-accel-pci,id=accl0,runtime=gen0,disable-legacy=off,disable-modern=on \
-kernel $UNIKERNEL_IMAGE -append "vfs.rootdev=data -- $APP_ARGS"
```

where:
- QEMU\_BINARY is the full path of a QEMU binary, built with the vAccel bacckend
- DATA is the directory which contains the data for the example
- UNIKERNEL\_IMAGE, is the path to the unikernel image
- $APP\_ARGS, are the arguments for the example application

