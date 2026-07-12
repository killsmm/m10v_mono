FROM ubuntu:16.04


RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends tzdata

RUN apt-get update && apt-get install -y openssh-server \
	language-pack-en \
	parted \
	mtools \
	dosfstools \
	gawk wget git diffstat unzip texinfo gcc build-essential chrpath socat cpio \
	python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping \
	python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm \ 
	python3-subunit mesa-common-dev zstd liblz4-tool sudo libssl-dev libc6-i386 jq bc genromfs kmod software-properties-common

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y locales

RUN wget http://releases.linaro.org/components/toolchain/binaries/5.3-2016.05/arm-linux-gnueabihf/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf.tar.xz

RUN sudo cp gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf.tar.xz /opt && cd /opt && sudo tar xf gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf.tar.xz && sudo rm gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf.tar.xz

RUN wget https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update/+download/gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2 && mkdir -p /home/mptk && sudo tar xf gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2 -C /home/mptk && mv /home/mptk/gcc-arm-none-eabi-4_9-2015q3 /home/mptk/prefix_for_bin && sudo rm gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2

RUN rm -rf /var/lib/apt/lists/*

RUN wget https://go.dev/dl/go1.20.1.linux-amd64.tar.gz

RUN sudo cp go1.20.1.linux-amd64.tar.gz /usr/local/ && cd /usr/local && tar xvf go1.20.1.linux-amd64.tar.gz && rm go1.20.1.linux-amd64.tar.gz

RUN PB_REL="https://github.com/protocolbuffers/protobuf/releases" && wget $PB_REL/download/v3.15.8/protoc-3.15.8-linux-x86_64.zip 

RUN unzip protoc-3.15.8-linux-x86_64.zip -d /usr/local && rm protoc-3.15.8-linux-x86_64.zip && chmod -R 777 /usr/local/

RUN useradd -g sudo -d /home/m10v -s /bin/bash m10v

RUN echo 'm10v:m10v' | chpasswd


ENV GOPATH=/home/m10v/go

ENV PATH=$PATH:/opt/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin:/usr/local/go/bin:$GOPATH/bin

RUN go install google.golang.org/protobuf/cmd/protoc-gen-go@latest

RUN go install google.golang.org/grpc/cmd/protoc-gen-go-grpc@latest

# RUN go install github.com/grpc-ecosystem/grpc-gateway/protoc-gen-openapi@latest

RUN go install github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway@latest

RUN chown -R m10v /home/m10v


# CMD export GOPATH=/home/m10v/go

# CMD export PATH=$PATH:/usr/local/go/bin


# CMD export LC_ALL="en_US.UTF-8" && export LC_CTYPE="en_US.UTF-8" && sudo dpkg-reconfigure locales

RUN sed -i '/en_US.UTF-8/s/^# //g' /etc/locale.gen && \
    locale-gen
ENV LANG en_US.UTF-8  
ENV LANGUAGE en_US:en  
ENV LC_ALL en_US.UTF-8

# RUN wget https://deb.nodesource.com/setup_16.x && sudo bash setup_16.x && sudo apt-get install -y nodejs

# RUN sh -c "echo deb https://deb.nodesource.com/node_16.x lunar main \
# > /etc/apt/sources.list.d/nodesource.list"
# RUN apt-get update
# RUN apt-get install nodejs

RUN wget https://deb.nodesource.com/node_16.x/pool/main/n/nodejs/nodejs_16.20.2-deb-1nodesource1_amd64.deb
RUN dpkg -i nodejs_16.20.2-deb-1nodesource1_amd64.deb

COPY poky-glibc-x86_64-meta-toolchain-cortexa7-toolchain-2.2.sh /opt

RUN chmod +x /opt/poky-glibc-x86_64-meta-toolchain-cortexa7-toolchain-2.2.sh

RUN sh /opt/poky-glibc-x86_64-meta-toolchain-cortexa7-toolchain-2.2.sh
