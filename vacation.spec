Summary: vacation, an automatic mail answering program
Name: vacation
Version: GITVERSION
Release: 1
Source: https://downloads.sourceforge.net/project/vacation/vacation/%{version}/vacation-%{version}.tar.gz
License: GPL
Group: Applications/Internet
BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: perl >= 5
BuildRequires: gdbm-devel

%description
Vacation is the automatic mail answering program
found on many Unix systems.


%prep
%setup


%build
make

%install
make install DESTDIR=${RPM_BUILD_ROOT}

%clean
rm -rf ${RPM_BUILD_ROOT}


%files
%attr (-,root,root) %{_bindir}/vacation
%attr (-,root,root) %{_bindir}/vaclook
%attr (-,root,root) %{_mandir}/man1/*.1
%attr (-,root,root) %{_mandir}/*/man1/*.1
%doc %attr (-,root,root) COPYING README README.smrsh ChangeLog


%changelog
* Tue Jan 21 2014 Chris Samuel <chris@csamuel.org>
- upgrade to version 1.2.8.0-beta1
- version in spec file now automatically populated from git.

* Tue Nov 08 2011 Chris Samuel <chris@csamuel.org>
- Add German translation of manpage to spec file.
- Simplify spec file to just use make install now we support DESTDIR.

* Sun Nov 06 2011 Chris Samuel <chris@csamuel.org>
- upgrade to version 1.2.7.1

* Thu Oct 20 2011 Chris Samuel <chris@csamuel.org>
- upgrade to version 1.2.7.1-beta2

* Tue Jul  8 2008 Magnus Stenman <stone@hkust.se>
- Spec from Guardian Digital, Inc. <http://www.guardiandigital.com>
- Version 1.2.7.0

* Tue Sep 03 2002 Pete O'Hara <pete@guardiandigital.com>
- Version 1.2.7.rc1, Release 1.2.1
  - Upgraded to 1.2.7.rc1

* Mon Aug 26 2002 Pete O'Hara <pete@guardiandigital.com>
- Version 1.2.6.1, Release 1.2.0
  - Initial release for Mail Suite

* Sun Nov 11 2001 Devon <devon@tuxfan.homeip.net>
- upgrade to version 1.2.7.rc1
* Sat Nov 10 2001 Devon <devon@tuxfan.homeip.net>
- upgrade to version 1.2.6
* Wed Sep 19 2001 Devon <devon@tuxfan.homeip.net>
- added %post link /etc/smrsh to /usr/bin/vacation
- added %postun deletion of /etc/smrsh/vacation
- defined a umask of 022 fix permissions on created files.
  $HOME/.forward was created group writable, smrsh refused
  to run in that case. See vacation-1.2.2-permissions.patch

* Mon Aug 07 2000 Than Ngo <than@redhat.de>
- fix specfile and patch file to rebuilt

* Mon Aug 07 2000 Michael Stefaniuc <mstefani@redhat.com>
- upgraded to 1.2.2
- fixed security fix

* Wed Aug 02 2000 Than Ngo <than@redhat.de>
- fix manpath (Bug #15070)

* Mon Jul 24 2000 Prospector <prospector@redhat.com>
- rebuilt

* Sun Jul 16 2000 Florian La Roche <Florian.LaRoche@redhat.de>
- add security fix

* Mon Jul 10 2000 Than Ngo <than@redhat.de>
- fix problem (it won't include the .vacation.msg) (bug #13572)
- use RPM macros

* Mon Jul 03 2000 Prospector <bugzilla@redhat.com>
- automatic rebuild

* Sun May 28 2000 Ngo Than <than@redhat.de>
- rebuild for 7.0
- put man pages in correct place
- cleanup specfile
- fix Summary

* Fri Dec 10 1999 Ngo Than <than@redhat.de>
- initial RPM for powertools-6.2
