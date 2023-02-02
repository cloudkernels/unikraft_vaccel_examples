#/bin/bash

usage()
{ 
	echo -e "Run a Unikraft vAccel example with QEMU/KVM"
	echo -e "usage: $0 <path_to_data> <<unikernel_image> [ARGS]"
	echo -e "where:"
	echo -e "\t path_to_data is the path, where the data for the example are located"
	echo -e "\t unikernel_image is the path, where the unikernel image is located"
	echo -e "\t ARGS any arguments that are needed for the example"
}

QEMU_BINARY="${QEMU_BINARY:-qemu-system-x86_64}"
DATA=$1
UNIKERNEL_IMAGE=$2
shift
shift
APP_ARGS=$@

if [ -z $QEMU_BINARY ] || [ -z $DATA ] || [ -z $UNIKERNEL_IMAGE ]
then
	usage
	exit 1
fi

if [ -z $VACCEL_BACKENDS ]
then
	case $UNIKERNEL_IMAGE in
		*"classify"* | *"detect"* | *"degment"* | *"pose"* | *"depth"*)
			VACCEL_BACKENDS=/usr/local/lib/libvaccel-jetson.so
			if [ -z $APP_ARGS ]
			then
				APP_ARGS="dog_0.jpg 1"
			fi
			;;
		*)
			VACCEL_BACKENDS=/usr/local/lib/libvaccel-noop.so
			;;
	esac
fi

export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
export VACCEL_BACKENDS=$VACCEL_BACKENDS
export VACCEL_DEBUG_LEVEL=4

$QEMU_BINARY \
	-cpu host -m 512 -enable-kvm -nographic -vga none \
	-fsdev local,id=myid,path=${DATA},security_model=none \
	-device virtio-9p-pci,fsdev=myid,mount_tag=data,disable-modern=on,disable-legacy=off \
	-object acceldev-backend-vaccelrt,id=gen0 -device virtio-accel-pci,id=accl0,runtime=gen0,disable-legacy=off,disable-modern=on \
	-kernel ${UNIKERNEL_IMAGE} -append "vfs.rootdev=data -- ${APP_ARGS}"
