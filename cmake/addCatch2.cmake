# 添加第三方依赖包
include(FetchContent)

set(CATCH_GIT_TAG  v2.13.10)  # 指定版本
set(CATCH_GIT_URL  https://github.com/catchorg/Catch2.git)  # 指定git仓库地址

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY    ${CATCH_GIT_URL}
        GIT_TAG           ${CATCH_GIT_TAG}
)

FetchContent_MakeAvailable(Catch2)