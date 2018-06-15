#1/bin/bash
# Sudo need to be installed and user should be on the sudoers list
sudo apt-get install -y g++
sudo apt-get install -y git
sudo apt-get install -y scons
sudo apt-get install -y libboost-dev
sudo apt-get install -y libboost-system-dev
sudo apt-get install -y libboost-thread-dev
sudo apt-get install -y python-pip
sudo pip install -r requirments.txt
scons
