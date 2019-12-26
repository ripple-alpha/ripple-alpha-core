%define rippled_version %(echo $RIPPLED_RPM_VERSION)
%define rpm_release %(echo $RPM_RELEASE)
%define rpm_patch %(echo $RPM_PATCH)
%define _prefix /opt/ripple-alpha
Name:           ripple-alpha-core
# Dashes in Version extensions must be converted to underscores
Version:        %{rippled_version}
Release:        %{rpm_release}%{?dist}%{rpm_patch}
Summary:        ripple-alpha-core daemon

License:        MIT
URL:            http://ripple.com/
Source0:        rippled.tar.gz

BuildRequires:  protobuf-static openssl-static cmake zlib-static ninja-build

%description
ripple-alpha-core

%package devel
Summary: Files for development of applications using xrpl core library
Group: Development/Libraries
Requires: openssl-static, zlib-static

%description devel
core library for development of standalone applications that sign transactions.

%prep
%setup -c -n ripple-alpha-core

%build
cd ripple-alpha-core
mkdir -p bld.release
cd bld.release
cmake .. -G Ninja -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_BUILD_TYPE=Release -Dstatic=true -DCMAKE_VERBOSE_MAKEFILE=ON -Dlocal_protobuf=ON -Dvalidator_keys=ON
cmake --build . --parallel --target ripple-alpha-core --target validator-keys -- -v

%pre
test -e /etc/pki/tls || { mkdir -p /etc/pki; ln -s /usr/lib/ssl /etc/pki/tls; }

%install
rm -rf $RPM_BUILD_ROOT
DESTDIR=$RPM_BUILD_ROOT cmake --build rippled/bld.release --target install -- -v
install -d ${RPM_BUILD_ROOT}/etc/opt/ripple-alpha
install -d ${RPM_BUILD_ROOT}/usr/local/bin
ln -s %{_prefix}/etc/ripple-alpha-core.cfg ${RPM_BUILD_ROOT}/etc/opt/ripple-alpha/ripple-alpha-core.cfg
ln -s %{_prefix}/etc/validators.txt ${RPM_BUILD_ROOT}/etc/opt/ripple-alpha/validators.txt
ln -s %{_prefix}/bin/ripple-alpha-core ${RPM_BUILD_ROOT}/usr/local/bin/ripple-alpha-core
install -D ripple-alpha-core/bld.release/validator-keys/validator-keys ${RPM_BUILD_ROOT}%{_bindir}/validator-keys
install -D ./ripple-alpha-core/Builds/containers/shared/ripple-alpha-core.service ${RPM_BUILD_ROOT}/usr/lib/systemd/system/ripple-alpha-core.service
install -D ./ripple-alpha-core/Builds/containers/packaging/rpm/50-rippled.preset ${RPM_BUILD_ROOT}/usr/lib/systemd/system-preset/50-rippled.preset
install -D ./ripple-alpha-core/Builds/containers/shared/update-rippled.sh ${RPM_BUILD_ROOT}%{_bindir}/update-rippled.sh
install -D ./ripple-alpha-core/Builds/containers/shared/update-rippled-cron ${RPM_BUILD_ROOT}%{_prefix}/etc/update-rippled-cron
install -D ./ripple-alpha-core/Builds/containers/shared/rippled-logrotate ${RPM_BUILD_ROOT}/etc/logrotate.d/rippled
install -d $RPM_BUILD_ROOT/var/log/rippled
install -d $RPM_BUILD_ROOT/var/lib/rippled

%post
USER_NAME=ripple-alpha-core
GROUP_NAME=ripple-alpha-core

getent passwd $USER_NAME &>/dev/null || useradd $USER_NAME
getent group $GROUP_NAME &>/dev/null || groupadd $GROUP_NAME

chown -R $USER_NAME:$GROUP_NAME /var/log/rippled/
chown -R $USER_NAME:$GROUP_NAME /var/lib/rippled/
chown -R $USER_NAME:$GROUP_NAME %{_prefix}/

chmod 755 /var/log/rippled/
chmod 755 /var/lib/rippled/

chmod 644 %{_prefix}/etc/update-rippled-cron
chmod 644 /etc/logrotate.d/rippled
chown -R root:$GROUP_NAME %{_prefix}/etc/update-rippled-cron

%files
%doc ripple-alpha-core/README.md ripple-alpha-core/LICENSE
%{_bindir}/ripple-alpha-core
/usr/local/bin/ripple-alpha-core
%{_bindir}/update-rippled.sh
%{_prefix}/etc/update-rippled-cron
%{_bindir}/validator-keys
%config(noreplace) %{_prefix}/etc/ripple-alpha-core.cfg
%config(noreplace) /etc/opt/ripple-alpha/ripple-alpha-core.cfg
%config(noreplace) %{_prefix}/etc/validators.txt
%config(noreplace) /etc/opt/ripple-alpha/validators.txt
%config(noreplace) /etc/logrotate.d/rippled
%config(noreplace) /usr/lib/systemd/system/ripple-alpha-core.service
%config(noreplace) /usr/lib/systemd/system-preset/50-rippled.preset
%dir /var/log/rippled/
%dir /var/lib/rippled/

%files devel
%{_prefix}/include
%{_prefix}/lib/*.a
%{_prefix}/lib/cmake/ripple

%changelog
* Wed Aug 28 2019 Mike Ellery <mellery451@gmail.com>
- Switch to subproject build for validator-keys

* Wed May 15 2019 Mike Ellery <mellery451@gmail.com>
- Make validator-keys use local rippled build for core lib

* Wed Aug 01 2018 Mike Ellery <mellery451@gmail.com>
- add devel package for signing library

* Thu Jun 02 2016 Brandon Wilson <bwilson@ripple.com>
- Install validators.txt

