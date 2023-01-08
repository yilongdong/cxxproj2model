# 添加第三方依赖包
include(FetchContent)

set(SPDLOG_GIT_TAG  v1.11.0)  # 指定版本
set(SPDLOG_GIT_URL  https://github.com/gabime/spdlog.git)  # 指定git仓库地址

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY    ${SPDLOG_GIT_URL}
        GIT_TAG           ${SPDLOG_GIT_TAG}
)

FetchContent_MakeAvailable(spdlog)