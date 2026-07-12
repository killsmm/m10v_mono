#!/bin/bash

script_dir=$(dirname $(realpath $0))

if [[ $(uname -a) =~ "WSL" ]]
then
    echo "WSL system"
    sudo docker version > /dev/null
    if [ $? -ne 0 ]
    then
        echo "Please make sure that you have the windows docker application: "
        echo "https://docs.docker.com/desktop/install/windows-install/"
        exit 1
    fi
else
    echo "None-WSL system"
    sudo apt install docker.io -y
    if [ $? -ne 0 ]
    then
	echo "install docker failed"
	exit 1
    fi
fi
sudo docker build -t m10v-compiler ${script_dir}