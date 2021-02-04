### Image classification example

A small example to test vAccel in unikraft.

The vaccel\_config file contains a config for KVM platform which enables the following libraries:
- vAccelrt (the virtio accel driver gets enabled too)
- 9pfs, so we can pass the image to the unikernel
- vfscore with 9pfs as root filesystem
- devfs which gets mounted during boot , for /dev/accel

Afer building unikraft you can run it using the below command.

```
LD_LIBRARY_PATH=$PATH_TO_VACCELRT:/usr/local/lib:/usr/local/nvidia/lib:/usr/local/nvidia/lib64 qemu-system-x86_64 \
-cpu host -m 512 -enable-kvm -nographic -vga none \
-fsdev local,id=myid,path=$PATH_TO_DIRECTORY_CONTAINING_IMAGE,security_model=none -device virtio-9p-pci,fsdev=myid,mount_tag=data,disable-modern=on,disable-legacy=off \
-object acceldev-backend-vaccelrt,id=gen0 -device virtio-accel-pci,id=accl0,runtime=gen0,disable-legacy=off,disable-modern=on \
-kernel $PATH_TO_UNIKRAFT_IMAGE -append "vfs.rootdev=data -- $IMAGE $ITERATIONS"
```

where:
- PATH\_TO\_VACCELRT is the install directory of vAccelrt
- PATH\_TO\_DIRECTORY\_CONTAINING\_IMAGE is the directory which contains $IMAGE
- PATH\_TO\_UNIKRAFT\_IMAGE, under build of the directory we built unikraft
- $IMAGE the image we want to classify
- $ITERATIONS, how many iterations we want to do

