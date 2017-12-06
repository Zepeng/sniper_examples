----------> uses
# use SniperRelease v*  (no_version_directory)
#   use SniperPolicy v*  (no_version_directory)
#   use SniperKernel v*  (no_version_directory)
#     use SniperPolicy v*  (no_version_directory)
#     use Boost v* Externals (no_version_directory)
#       use Python v* Externals (no_version_directory)
#   use DataBuffer v* SniperUtil (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
#   use HelloWorld v* Examples (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
#   use RootWriter v* SniperSvc (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
#     use ROOT v* Externals (no_version_directory)
#     use Boost v* Externals (no_version_directory)
#   use DummyAlg v* Examples (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
#     use RootWriter v* SniperSvc (no_version_directory)
# use EvtNavigator v* DataModel (no_version_directory)
#   use ROOT v* Externals (no_version_directory)
#   use BaseEvent v* DataModel (no_version_directory)
#     use XmlObjDesc v*  (no_auto_imports) (no_version_directory)
#       use SniperPolicy v*  (no_version_directory)
#       use ROOT v* Externals (no_version_directory)
#     use ROOT v* Externals (no_version_directory)
#   use EDMUtil v* DataModel (no_version_directory)
#     use ROOT v* Externals (no_version_directory)
#     use BaseEvent v* DataModel (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
# use SimEvent v* DataModel (no_version_directory)
#   use ROOT v* Externals (no_version_directory)
#   use BaseEvent v* DataModel (no_version_directory)
#   use EDMUtil v* DataModel (no_version_directory)
# use ElecEvent v* DataModel (no_version_directory)
#   use ROOT v* Externals (no_version_directory)
#   use BaseEvent v* DataModel (no_version_directory)
#   use EDMUtil v* DataModel (no_version_directory)
# use PidTmvaEvent v* DataModel (no_version_directory)
#   use ROOT v* Externals (no_version_directory)
#   use BaseEvent v* DataModel (no_version_directory)
#   use EDMUtil v* DataModel (no_version_directory)
# use BufferMemMgr v* CommonSvc (no_version_directory)
#   use SniperKernel v*  (no_version_directory)
#   use EvtNavigator v* DataModel (no_version_directory)
#   use RootIOSvc v* RootIO (no_version_directory)
#     use SniperKernel v*  (no_version_directory)
#     use Boost v* Externals (no_version_directory)
#     use RootIOUtil v* RootIO (no_version_directory)
#       use SniperKernel v*  (no_version_directory)
#       use BaseEvent v* DataModel (no_version_directory)
#       use EDMUtil v* DataModel (no_version_directory)
#       use EvtNavigator v* DataModel (no_version_directory)
#       use ROOT v* Externals (no_version_directory)
#     use ROOT v* Externals (no_version_directory)
#     use EDMUtil v* DataModel (no_version_directory)
# use RootIOSvc v* RootIO (no_version_directory)
#
# Selection :
use CMT v1r26 (/home/hep/david_moore/zl423/NEXOTOP/ExternalLibs)
use ROOT v0 Externals (/home/hep/david_moore/zl423/NEXOTOP/nexo-ei)
use Python v0 Externals (/home/hep/david_moore/zl423/NEXOTOP/nexo-ei)
use Boost v0 Externals (/home/hep/david_moore/zl423/NEXOTOP/nexo-ei)
use SniperPolicy v0  (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use XmlObjDesc v0  (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline) (no_auto_imports)
use BaseEvent v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use SniperKernel v2  (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use EDMUtil v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use PidTmvaEvent v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use ElecEvent v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use SimEvent v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use EvtNavigator v0 DataModel (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use RootIOUtil v0 RootIO (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use RootIOSvc v0 RootIO (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use BufferMemMgr v0 CommonSvc (/home/hep/david_moore/zl423/NEXOTOP/nexo-offline)
use RootWriter v0 SniperSvc (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use DummyAlg v0 Examples (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use HelloWorld v1 Examples (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use DataBuffer v0 SniperUtil (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
use SniperRelease v2  (/home/hep/david_moore/zl423/NEXOTOP/nexo-sniper)
----------> tags
CMTv1 (from CMTVERSION)
CMTr26 (from CMTVERSION)
CMTp0 (from CMTVERSION)
Linux (from uname) package [CMT] implies [Unix]
Linux-x86_64 (from CMTCONFIG)
nexo-offline_no_config (from PROJECT) excludes [nexo-offline_config]
nexo-offline_root (from PROJECT) excludes [nexo-offline_no_root]
nexo-offline_cleanup (from PROJECT) excludes [nexo-offline_no_cleanup]
nexo-offline_scripts (from PROJECT) excludes [nexo-offline_no_scripts]
nexo-offline_prototypes (from PROJECT) excludes [nexo-offline_no_prototypes]
nexo-offline_with_installarea (from PROJECT) excludes [nexo-offline_without_installarea]
nexo-offline_without_version_directory (from PROJECT) excludes [nexo-offline_with_version_directory]
nexo-sniper_no_config (from PROJECT) excludes [nexo-sniper_config]
nexo-sniper_root (from PROJECT) excludes [nexo-sniper_no_root]
nexo-sniper_cleanup (from PROJECT) excludes [nexo-sniper_no_cleanup]
nexo-sniper_scripts (from PROJECT) excludes [nexo-sniper_no_scripts]
nexo-sniper_prototypes (from PROJECT) excludes [nexo-sniper_no_prototypes]
nexo-sniper_with_installarea (from PROJECT) excludes [nexo-sniper_without_installarea]
nexo-sniper_without_version_directory (from PROJECT) excludes [nexo-sniper_with_version_directory]
nexo-ei_no_config (from PROJECT) excludes [nexo-ei_config]
nexo-ei_no_root (from PROJECT) excludes [nexo-ei_root]
nexo-ei_cleanup (from PROJECT) excludes [nexo-ei_no_cleanup]
nexo-ei_scripts (from PROJECT) excludes [nexo-ei_no_scripts]
nexo-ei_prototypes (from PROJECT) excludes [nexo-ei_no_prototypes]
nexo-ei_without_installarea (from PROJECT) excludes [nexo-ei_with_installarea]
nexo-ei_without_version_directory (from PROJECT) excludes [nexo-ei_with_version_directory]
x86_64 (from package CMT)
rhel72 (from package CMT)
gcc485 (from package CMT)
Unix (from package CMT) excludes [WIN32 Win32]
----------> CMTPATH
# Add path /home/hep/david_moore/zl423/NEXOTOP/nexo-offline from initialization
# Add path /home/hep/david_moore/zl423/NEXOTOP/nexo-sniper from ProjectPath
# Add path /home/hep/david_moore/zl423/NEXOTOP/nexo-ei from ProjectPath
