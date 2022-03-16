# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "devnan"

  config.vm.box = "centos/7"

  # NFS version defaults to 3, may not be supported by your host
  config.vm.synced_folder ".", "/src", type: "nfs", nfs_version: 4

  # upgrade the kernel to the latest version
  config.vm.provision "shell", inline: <<-SHELL
    yum install -y kernel
  SHELL

  # reboot into the newly-upgraded kernel
  config.vm.provision :reload

  config.vm.provision "shell", inline: <<-SHELL
    yum install -y kernel kernel-devel-$(uname -r) gcc numpy pytest
    cd /src
    make
    make load
    py.test
  SHELL

end
