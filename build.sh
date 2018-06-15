#1/bin/bash
# Sudo need to be installed and user should be on the sudoers list
if [[ $1 == "deploy" ]]
then
sudo apt-get install -y g++ git scons libboost-dev libboost-system-dev libboost-thread-dev python-pip
sudo pip install -r requirments.txt
fi

git submodule update
scons
