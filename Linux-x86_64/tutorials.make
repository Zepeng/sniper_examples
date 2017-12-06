#-- start of make_header -----------------

#====================================
#  Library tutorials
#
#   Generated Wed Dec  6 12:12:04 2017  by zl423
#
#====================================

include ${CMTROOT}/src/Makefile.core

ifdef tag
CMTEXTRATAGS = $(tag)
else
tag       = $(CMTCONFIG)
endif

cmt_tutorials_has_no_target_tag = 1

#--------------------------------------------------------

ifdef cmt_tutorials_has_target_tag

tags      = $(tag),$(CMTEXTRATAGS),target_tutorials

tutorials_tag = $(tag)

#cmt_local_tagfile_tutorials = $(tutorials_tag)_tutorials.make
cmt_local_tagfile_tutorials = $(bin)$(tutorials_tag)_tutorials.make

else

tags      = $(tag),$(CMTEXTRATAGS)

tutorials_tag = $(tag)

#cmt_local_tagfile_tutorials = $(tutorials_tag).make
cmt_local_tagfile_tutorials = $(bin)$(tutorials_tag).make

endif

include $(cmt_local_tagfile_tutorials)
#-include $(cmt_local_tagfile_tutorials)

ifdef cmt_tutorials_has_target_tag

cmt_final_setup_tutorials = $(bin)setup_tutorials.make
cmt_dependencies_in_tutorials = $(bin)dependencies_tutorials.in
#cmt_final_setup_tutorials = $(bin)tutorials_tutorialssetup.make
cmt_local_tutorials_makefile = $(bin)tutorials.make

else

cmt_final_setup_tutorials = $(bin)setup.make
cmt_dependencies_in_tutorials = $(bin)dependencies.in
#cmt_final_setup_tutorials = $(bin)tutorialssetup.make
cmt_local_tutorials_makefile = $(bin)tutorials.make

endif

#cmt_final_setup = $(bin)setup.make
#cmt_final_setup = $(bin)tutorialssetup.make

#tutorials :: ;

dirs ::
	@if test ! -r requirements ; then echo "No requirements file" ; fi; \
	  if test ! -d $(bin) ; then $(mkdir) -p $(bin) ; fi

javadirs ::
	@if test ! -d $(javabin) ; then $(mkdir) -p $(javabin) ; fi

srcdirs ::
	@if test ! -d $(src) ; then $(mkdir) -p $(src) ; fi

help ::
	$(echo) 'tutorials'

binobj = 
ifdef STRUCTURED_OUTPUT
binobj = tutorials/
#tutorials::
#	@if test ! -d $(bin)$(binobj) ; then $(mkdir) -p $(bin)$(binobj) ; fi
#	$(echo) "STRUCTURED_OUTPUT="$(bin)$(binobj)
endif

${CMTROOT}/src/Makefile.core : ;
ifdef use_requirements
$(use_requirements) : ;
endif

#-- end of make_header ------------------
#-- start of libary_header ---------------

tutorialslibname   = $(bin)$(library_prefix)tutorials$(library_suffix)
tutorialslib       = $(tutorialslibname).a
tutorialsstamp     = $(bin)tutorials.stamp
tutorialsshstamp   = $(bin)tutorials.shstamp

tutorials :: dirs  tutorialsLIB
	$(echo) "tutorials ok"

cmt_tutorials_has_prototypes = 1

#--------------------------------------

ifdef cmt_tutorials_has_prototypes

tutorialsprototype :  ;

endif

tutorialscompile : $(bin)ChargeReconAlg.o $(bin)MC_change.o ;

#-- end of libary_header ----------------
#-- start of libary ----------------------

tutorialsLIB :: $(tutorialslib) $(tutorialsshstamp)
	$(echo) "tutorials : library ok"

$(tutorialslib) :: $(bin)ChargeReconAlg.o $(bin)MC_change.o
	$(lib_echo) "static library $@"
	$(lib_silent) [ ! -f $@ ] || \rm -f $@
	$(lib_silent) $(ar) $(tutorialslib) $(bin)ChargeReconAlg.o $(bin)MC_change.o
	$(lib_silent) $(ranlib) $(tutorialslib)
	$(lib_silent) cat /dev/null >$(tutorialsstamp)

#------------------------------------------------------------------
#  Future improvement? to empty the object files after
#  storing in the library
#
##	  for f in $?; do \
##	    rm $${f}; touch $${f}; \
##	  done
#------------------------------------------------------------------

#
# We add one level of dependency upon the true shared library 
# (rather than simply upon the stamp file)
# this is for cases where the shared library has not been built
# while the stamp was created (error??) 
#

$(tutorialslibname).$(shlibsuffix) :: $(tutorialslib) requirements $(use_requirements) $(tutorialsstamps)
	$(lib_echo) "shared library $@"
	$(lib_silent) if test "$(makecmd)"; then QUIET=; else QUIET=1; fi; QUIET=$${QUIET} bin="$(bin)" ld="$(shlibbuilder)" ldflags="$(shlibflags)" suffix=$(shlibsuffix) libprefix=$(library_prefix) libsuffix=$(library_suffix) $(make_shlib) "$(tags)" tutorials $(tutorials_shlibflags)
	$(lib_silent) cat /dev/null >$(tutorialsshstamp)

$(tutorialsshstamp) :: $(tutorialslibname).$(shlibsuffix)
	$(lib_silent) if test -f $(tutorialslibname).$(shlibsuffix) ; then cat /dev/null >$(tutorialsshstamp) ; fi

tutorialsclean ::
	$(cleanup_echo) objects tutorials
	$(cleanup_silent) /bin/rm -f $(bin)ChargeReconAlg.o $(bin)MC_change.o
	$(cleanup_silent) /bin/rm -f $(patsubst %.o,%.d,$(bin)ChargeReconAlg.o $(bin)MC_change.o) $(patsubst %.o,%.dep,$(bin)ChargeReconAlg.o $(bin)MC_change.o) $(patsubst %.o,%.d.stamp,$(bin)ChargeReconAlg.o $(bin)MC_change.o)
	$(cleanup_silent) cd $(bin); /bin/rm -rf tutorials_deps tutorials_dependencies.make

#-----------------------------------------------------------------
#
#  New section for automatic installation
#
#-----------------------------------------------------------------

install_dir = ${CMTINSTALLAREA}/$(tag)/lib
tutorialsinstallname = $(library_prefix)tutorials$(library_suffix).$(shlibsuffix)

tutorials :: tutorialsinstall ;

install :: tutorialsinstall ;

tutorialsinstall :: $(install_dir)/$(tutorialsinstallname)
ifdef CMTINSTALLAREA
	$(echo) "installation done"
endif

$(install_dir)/$(tutorialsinstallname) :: $(bin)$(tutorialsinstallname)
ifdef CMTINSTALLAREA
	$(install_silent) $(cmt_install_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(tutorialsinstallname)" \
	    -out "$(install_dir)" \
	    -cmd "$(cmt_installarea_command)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

##tutorialsclean :: tutorialsuninstall

uninstall :: tutorialsuninstall ;

tutorialsuninstall ::
ifdef CMTINSTALLAREA
	$(cleanup_silent) $(cmt_uninstall_action) \
	    -source "`(cd $(bin); pwd)`" \
	    -name "$(tutorialsinstallname)" \
	    -out "$(install_dir)" \
	    -cmtpath "$($(package)_cmtpath)"
endif

#-- end of libary -----------------------
#-- start of dependencies ------------------
ifneq ($(MAKECMDGOALS),tutorialsclean)
ifneq ($(MAKECMDGOALS),uninstall)
ifneq ($(MAKECMDGOALS),tutorialsprototype)

$(bin)tutorials_dependencies.make : $(use_requirements) $(cmt_final_setup_tutorials)
	$(echo) "(tutorials.make) Rebuilding $@"; \
	  $(build_dependencies) -out=$@ -start_all $(src)ChargeReconAlg.cxx $(src)MC_change.cxx -end_all $(includes) $(app_tutorials_cppflags) $(lib_tutorials_cppflags) -name=tutorials $? -f=$(cmt_dependencies_in_tutorials) -without_cmt

-include $(bin)tutorials_dependencies.make

endif
endif
endif

tutorialsclean ::
	$(cleanup_silent) \rm -rf $(bin)tutorials_deps $(bin)tutorials_dependencies.make
#-- end of dependencies -------------------
#-- start of cpp_library -----------------

ifneq (,)

ifneq ($(MAKECMDGOALS),tutorialsclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)ChargeReconAlg.d

$(bin)$(binobj)ChargeReconAlg.d :

$(bin)$(binobj)ChargeReconAlg.o : $(cmt_final_setup_tutorials)

$(bin)$(binobj)ChargeReconAlg.o : $(src)ChargeReconAlg.cxx
	$(cpp_echo) $(src)ChargeReconAlg.cxx
	$(cpp_silent) $(cppcomp)  -o $@ $(use_pp_cppflags) $(tutorials_pp_cppflags) $(lib_tutorials_pp_cppflags) $(ChargeReconAlg_pp_cppflags) $(use_cppflags) $(tutorials_cppflags) $(lib_tutorials_cppflags) $(ChargeReconAlg_cppflags) $(ChargeReconAlg_cxx_cppflags)  $(src)ChargeReconAlg.cxx
endif
endif

else
$(bin)tutorials_dependencies.make : $(ChargeReconAlg_cxx_dependencies)

$(bin)tutorials_dependencies.make : $(src)ChargeReconAlg.cxx

$(bin)$(binobj)ChargeReconAlg.o : $(ChargeReconAlg_cxx_dependencies)
	$(cpp_echo) $(src)ChargeReconAlg.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(tutorials_pp_cppflags) $(lib_tutorials_pp_cppflags) $(ChargeReconAlg_pp_cppflags) $(use_cppflags) $(tutorials_cppflags) $(lib_tutorials_cppflags) $(ChargeReconAlg_cppflags) $(ChargeReconAlg_cxx_cppflags)  $(src)ChargeReconAlg.cxx

endif

#-- end of cpp_library ------------------
#-- start of cpp_library -----------------

ifneq (,)

ifneq ($(MAKECMDGOALS),tutorialsclean)
ifneq ($(MAKECMDGOALS),uninstall)
-include $(bin)$(binobj)MC_change.d

$(bin)$(binobj)MC_change.d :

$(bin)$(binobj)MC_change.o : $(cmt_final_setup_tutorials)

$(bin)$(binobj)MC_change.o : $(src)MC_change.cxx
	$(cpp_echo) $(src)MC_change.cxx
	$(cpp_silent) $(cppcomp)  -o $@ $(use_pp_cppflags) $(tutorials_pp_cppflags) $(lib_tutorials_pp_cppflags) $(MC_change_pp_cppflags) $(use_cppflags) $(tutorials_cppflags) $(lib_tutorials_cppflags) $(MC_change_cppflags) $(MC_change_cxx_cppflags)  $(src)MC_change.cxx
endif
endif

else
$(bin)tutorials_dependencies.make : $(MC_change_cxx_dependencies)

$(bin)tutorials_dependencies.make : $(src)MC_change.cxx

$(bin)$(binobj)MC_change.o : $(MC_change_cxx_dependencies)
	$(cpp_echo) $(src)MC_change.cxx
	$(cpp_silent) $(cppcomp) -o $@ $(use_pp_cppflags) $(tutorials_pp_cppflags) $(lib_tutorials_pp_cppflags) $(MC_change_pp_cppflags) $(use_cppflags) $(tutorials_cppflags) $(lib_tutorials_cppflags) $(MC_change_cppflags) $(MC_change_cxx_cppflags)  $(src)MC_change.cxx

endif

#-- end of cpp_library ------------------
#-- start of cleanup_header --------------

clean :: tutorialsclean ;
#	@cd .

ifndef PEDANTIC
.DEFAULT::
	$(echo) "(tutorials.make) $@: No rule for such target" >&2
else
.DEFAULT::
	$(error PEDANTIC: $@: No rule for such target)
endif

tutorialsclean ::
#-- end of cleanup_header ---------------
#-- start of cleanup_library -------------
	$(cleanup_echo) library tutorials
	-$(cleanup_silent) cd $(bin) && \rm -f $(library_prefix)tutorials$(library_suffix).a $(library_prefix)tutorials$(library_suffix).$(shlibsuffix) tutorials.stamp tutorials.shstamp
#-- end of cleanup_library ---------------