# echo "cleanup tutorials v0 in /home/hep/david_moore/zl423/NEXOTOP"

if test "${CMTROOT}" = ""; then
  CMTROOT=/home/hep/david_moore/zl423/NEXOTOP/ExternalLibs/CMT/v1r26; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmttutorialstempfile=`${CMTROOT}/mgr/cmt -quiet build temporary_name`
if test ! $? = 0 ; then cmttutorialstempfile=/tmp/cmt.$$; fi
${CMTROOT}/mgr/cmt cleanup -sh -pack=tutorials -version=v0 -path=/home/hep/david_moore/zl423/NEXOTOP  $* >${cmttutorialstempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/mgr/cmt cleanup -sh -pack=tutorials -version=v0 -path=/home/hep/david_moore/zl423/NEXOTOP  $* >${cmttutorialstempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmttutorialstempfile}
  unset cmttutorialstempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmttutorialstempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmttutorialstempfile}
unset cmttutorialstempfile
return $cmtcleanupstatus

