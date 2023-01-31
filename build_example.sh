#!/bin/bash

list()
{
	echo -e "Available examples:"
	echo -e "\tnoop\t\t A simple noop example"
	echo -e "\tsgemm\t\t An example of Single precision GEneral Matrix Multiply"
	echo -e "\tsgemm_genop\t An example of Single precision GEneral Matrix Multiply, over vaccel's genop"
	echo -e "\tclassify\t An example of image classification"
	echo -e "\tclassify_genop\t An example of image classification, over vaccel's genop"
	echo -e "\tdetect\t\t An example of image detection"
	echo -e "\tdetect_genop\t An example of image detection, over vaccel's genop"
	echo -e "\tsegment\t\t An example of image segmenetation"
	echo -e "\tsgement_genop\t An example of image segmenetation, over vaccel's genop"
	echo -e "\tpose\t\t An example of image pose"
	echo -e "\tpose_genop\t An example of image pose, over vaccel's genop"
	echo -e "\tdepth\t\t An example of image depth"
	echo -e "\tdepth_genop\t An example of image depth, over vaccel's genop"
	echo -e "\texec\t\t An example of exec operation"
	echo -e "\texec_genop\t An example of exec operation, over vaccel's genop"
	echo -e "\tminmax\t\t An example of minmax operation"
	echo -e "\tminmax_genop\t An example of minmax operation, over vaccel's genop"
	echo -e "\tlenet\t\t An example of LeNet operation"
	echo -e "\tblackscholes\t An example of Blackscholes operation"
	exit 0
}

usage()
{ 
	echo -e "Build a Unikraft vAccel example image for KVM"
	echo -e "usage: $0 <example_name>"
	echo -e "where, <example_name>can be any of the available examples:"
	list
}

EXAMPLE_CONFIG=""

case $1 in
	noop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_NOOP"
		;;
	sgemm)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_SGEMM"
		;;
	sgemm_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_SGEMM_GOP"
		;;
	classify)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_CLASS"
		;;
	classify_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_CLASS_GOP"
		;;
	detect)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_DTCT"
		;;
	detect_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_DTCT_GOP"
		;;
	segment)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_SEGM"
		;;
	sgement_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_SEGM_GOP"
		;;
	pose)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_POSE"
		;;
	pose_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_POSE_GOP"
		;;
	depth)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_DPTH"
		;;
	depth_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_IMG_DPTH_GOP"
		;;
	exec)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_EXEC"
		;;
	exec_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_EXEC_GOP"
		;;
	minmax)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_MINMAX"
		;;
	minmax_genop)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_MINMAX_GOP"
		;;
	lenet)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_LENET"
		;;
	blackscholes)
		EXAMPLE_CONFIG="CONFIG_APPVACCELTEST_BSCHOLES"
		;;
	list)
		list
		;;
	help)
		usage
		;;
	*)
		echo -e "Error: the specified example does not exist"
		echo -e "\tUse: $0 help"
		echo -e "\tand check how this script works"
		exit 1
		;;
esac

cp vaccel_config .config
sed -i "s:# $EXAMPLE_CONFIG is not set:${EXAMPLE_CONFIG}=y:g" .config
make olddefconfig
make
