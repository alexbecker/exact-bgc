#!/bin/bash

# download necessary libraries
sudo apt-get update
sudo apt-get -y install nfs-kernel-server

# set up shared filesystem
mkdir /home/ubuntu/shared
mkdir /home/ubuntu/shared/handshake
sudo cp /etc/exports /home/ubuntu/temp
echo "/home/ubuntu/shared *(rw,async,insecure,no_root_squash,no_subtree_check)" >> /home/ubuntu/temp
sudo mv /home/ubuntu/temp /etc/exports
sudo service nfs-kernel-server reload
sudo exportfs -a

# donwload executable
cd /home/ubuntu/shared
wget http://alexcbecker.net/files/character_table
chmod a+x character_table
