include("/Users/mostafaamin/smartbudget/build-tests/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/SmartBudget-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "SmartBudget.app"
)
