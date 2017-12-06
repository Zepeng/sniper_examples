# echo "setup tutorials v0 in /home/hep/david_moore/zl423/NEXOTOP"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /home/hep/david_moore/zl423/NEXOTOP/ExternalLibs/CMT/v1r26
endif
source ${CMTROOT}/mgr/setup.csh
set cmttutorialstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if $status != 0 then
  set cmttutorialstempfile=/tmp/cmt.$$
endif
${CMTROOT}/mgr/cmt setup -csh -pack=tutorials -version=v0 -path=/home/hep/david_moore/zl423/NEXOTOP  -no_cleanup $* >${cmttutorialstempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/mgr/cmt setup -csh -pack=tutorials -version=v0 -path=/home/hep/david_moore/zl423/NEXOTOP  -no_cleanup $* >${cmttutorialstempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmttutorialstempfile}
  unset cmttutorialstempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmttutorialstempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmttutorialstempfile}
unset cmttutorialstempfile
exit $cmtsetupstatus

