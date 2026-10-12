sudo cp 99* /etc/udev/rules.d/.
sudo cp 51* /etc/udev/rules.d/.
sudo udevadm control --reload-rules
sudo udevadm trigger




sudo apt update

sudo apt install -y \
    git \
    build-essential \
    libtool \
    automake \
    autoconf \
    pkg-config \
    libusb-1.0-0-dev \
    libftdi1-dev libjim-dev
pip install pyftdi
pip install PyD3XX
sudo usermod -aG plugdev $USER


git clone https://github.com/WCHSoftGroup/ch347.git
cd ./ch347/OpenOCD_SourceCode_CH347
bash ./bootstrap
bash ./configure --enable-ch347
chmod +x ./src/helper/bin2char.sh
make -j$(nproc)
sudo make install
cd ..
cd ..
cp CH347DLL* ./ch347/CH347FPGATool/.

#python3 testusb.py

wget https://ftdichip.com/wp-content/uploads/2025/11/libftd3xx-linux-x86_64-1.1.1.tgz
tar xvf libftd3xx-linux-*.tgz
cd linux-x86_64
sudo cp libftd3xx.so libftd3xx.so.1.1.1 /usr/local/lib/
sudo ldconfig
sudo cp *.h /usr/local/include/

ls /usr/local/include | grep ftd3xx

sudo udevadm control --reload-rules
sudo udevadm trigger
cd ..



#cd

#wget -nc https://dl.winehq.org/wine-builds/winehq.key

#sudo apt-key add winehq.key

 #sudo add-apt-repository 'deb https://dl.winehq.org/wine-builds/ubuntu/ jammy main'
#wget -q https://download.opensuse.org/repositories/Emulators:/Wine:/Debian/xUbuntu_22.04/Release.key -O Release.key -O- | sudo apt-key add - 

#sudo apt-add-repository 'deb https://download.opensuse.org/repositories/Emulators:/Wine:/Debian/xUbuntu_22.04/ ./'
#sudo dpkg --add-architecture i386 && sudo apt update
#sudo apt install --install-recommends winehq-stable
