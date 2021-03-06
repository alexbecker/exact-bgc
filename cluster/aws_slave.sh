#!/bin/bash

# download necessary libraries
sudo apt-get update
sudo apt-get -y install nfs-common
sudo apt-get -y install libgmp10

# connect to master
sudo mkdir /mnt/master
sudo mount -t nfs %ipaddr%:%dir% /mnt/master

# perform handshake proceedure to get unique id
cd /mnt/master/handshake
touch "$HOSTNAME"
while [ $(ls -1 | wc -l) -lt %num_nodes% ]; do
:
done
id=$(ls -1 | grep -n "$HOSTNAME" | sed "s/:.*//")
let id=id-1

# run for n in [min_n, max_n]
cd /mnt/master
n=%min_n%
while [ $n -le %max_n% ]; do
	./character_table $n %num_nodes% $id %threads%
	n=$(expr $n + 1)
done
