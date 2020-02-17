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
