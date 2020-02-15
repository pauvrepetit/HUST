make -j4
sudo make modules_install -j4
sudo cp -v arch/x86_64/boot/bzImage /boot/vmlinuz-linux-5.4.19
sudo mkinitcpio -p linux-5.4.19

