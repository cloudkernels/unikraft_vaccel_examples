FROM debian:stable as builder

# install prerequisites
RUN apt-get update && apt-get install -y build-essential git \
	libz-dev bin86 bison flex wget bc libelf-dev libssl-dev \
	init udev kmod unzip python3 && apt-get clean

# build unikraft
RUN mkdir -p unikraft/libs unikraft/apps && \
	cd unikraft && \
	git clone https://github.com/cloudkernels/unikraft.git -b vaccelrt_rel010 && \
	cd apps && \
	git clone https://github.com/cloudkernels/unikraft_vaccel_examples

WORKDIR unikraft/apps/unikraft_vaccel_examples 

ARG EXAMPLE

RUN /bin/bash build_example.sh ${EXAMPLE}

FROM scratch as artifacts

ARG EXAMPLE

COPY --from=builder /unikraft/apps/unikraft_vaccel_examples/build/unikraft_vaccel_examples_kvm-x86_64 /unikraft_vaccel_${EXAMPLE}_kvm-x86_64
