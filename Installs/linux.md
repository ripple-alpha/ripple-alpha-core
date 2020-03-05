# How to install ripple alpha core with binary packages
## Debian/Ubuntu
### 1, Install GPG key
```# wget -qO - https://repo.ripplealpha.com/Release.key | sudo apt-key add -```
### 2, Add APT repository on your system
```
# echo "deb http://repo.ripplealpha.com/ bionic main" >> /etc/apt/sources.list.d/ripplealpha.list
# apt update
```
### 3, Install/Upgrade ripple-alpha-core
```# apt install ripplealpha```

If you've already installed older virsion of ripple-alpha-core and desire to upgrade to the latest one, run the following comannd.  
```# apt upgrade ripplealpha```

To enable automatic upgrade, run the following commant.  
```# ln -s /opt/ripple-alpha/etc/auto-upgrade-ripplealpha /etc/cron.d/```

## Fedora/Amazon Linux or other RPM based distributions
NOTE: If you hope to install Ripplealpha on CentOS 7 or earlier, you need to upgrade`glibc`to the version of 2.25 or higher.
### 1, Install YUM repository  on your system
```# yum install -y https://repo.ripplealpha.com/RPM/noarch/Ripplealpha-1-0.noarch.rpm```
### 2, Install ripple-alpha-core
```# yum install -y Ripplealpha.x86_64```
### 3, Upgrade ripple-alpha-core
If you've already installed older virsion of ripple-alpha-core and desire to upgrade to the latest one, run the following comannd.
```# yum upgrade -y Ripplealpha.x86_64```
