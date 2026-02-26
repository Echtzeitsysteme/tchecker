#!/bin/bash
# Main idea contributed by [Maximilian Kratz] (@maxkratz)
# Thanks for the inspiration!

set -e

# download Debian image
wget -q https://cloud.debian.org/images/cloud/trixie/latest/debian-13-generic-amd64.qcow2

# Copying Debian image to libvirt image folder.
cp debian-13-generic-amd64.qcow2 /var/lib/libvirt/images/

# create VM and start provisioning
virt-install --name tchecker --memory  16384 --vcpus 4 --disk=size=10,backing_store=/var/lib/libvirt/images/debian-13-generic-amd64.qcow2 --cloud-init user-data=.ci-scripts/cloud-init.yml,disable=on --network bridge=virbr0,mac=52:54:00:fa:58:c8 --osinfo=debian13
# ^the VM terminates itself after provisioning

# create ovf
virt-v2v -i libvirt -o local -os ./ tchecker

# create OVA template
qemu-img convert -p -f qcow2 -O vmdk /var/lib/libvirt/images/tchecker.qcow2 tchecker-disk001.vmdk
tar -cvf tchecker.ova tchecker.ovf tchecker-disk001.vmdk

# fixes permissions
chown -R 1000:1000 debian-13-generic-amd64.qcow2 tchecker-disk001.vmdk tchecker.ova