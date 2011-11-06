Summary: vacation, an automatic mail answering program
Name: vacation
Version: 1.2.7.1
Release: 1
Source: http://download.sourceforge.net/vacation/%{version}/vacation-%{version}.tar.gz
License: GPL
Group: Applications/Internet
BuildRoot: %{_tmppath}/%{name}-buildroot
Requires: perl >= 5

%description
Vacation is the automatic mail answering program
found on many Unix systems.


%prep
%setup


%build
make


%install
rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT%{_bindir}
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1

install -s -m 755 vacation     $RPM_BUILD_ROOT%{_bindir}/vacation
install -m 755 vaclook         $RPM_BUILD_ROOT%{_bindir}/vaclook
install -m 444 vacation.man    $RPM_BUILD_ROOT%{_mandir}/man1/vacation.1
install -m 444 vaclook.man     $RPM_BUILD_ROOT%{_mandir}/man1/vaclook.1


%clean
rm -rf $RPM_BUILD_ROOT


%files
%attr (-,root,root) %{_bindir}/vacation
%attr (-,root,root) %{_bindir}/vaclook
%attr (-,root,root) %{_mandir}/*/*
%doc %attr (-,root,root) COPYING README README.smrsh ChangeLog


%changelog
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
