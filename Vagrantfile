# -*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant.configure("2") do |config|
  config.vm.define "devnan"

  config.vm.box = "centos/7"

  config.vm.synced_folder ".", "/src"

  config.vm.provider :libvirt do |libvirt|
    libvirt.storage_pool_name = "images"
  end

  config.vm.provision "shell", inline: <<-SHELL
    yum install -y kernel
  SHELL

  config.vm.provision :reload

  config.vm.provision "shell", inline: <<-SHELL
    yum install -y kernel kernel-devel-$(uname -r) gcc numpy pytest
    cd /src
    make
    make load
    py.test
  SHELL

end
