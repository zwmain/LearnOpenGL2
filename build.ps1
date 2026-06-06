$Arguments = $args
$BUILD_TYPE = ""
$CLEAN = $false

foreach ($arg in $Arguments) {
    if ($arg -eq 'debug') {
        $BUILD_TYPE = "Debug"
    } elseif ($arg -eq 'release') {
        $BUILD_TYPE = "Release"
    } elseif ($arg -eq 'clean') {
        Remove-Item -Path "build" -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "已清理 build 文件夹"
        exit
    } else {
        Write-Host "不支持的参数：$arg" -ForegroundColor Red
        exit
    }
}

if ([string]::IsNullOrEmpty($BUILD_TYPE)) {
    $BUILD_TYPE = "Debug"
}

Write-Host "开始构建，类型：$BUILD_TYPE"

# 除非已经执行过 clean，否则清理 build 文件夹
# if ($CLEAN -eq $false) {
    # if (Test-Path "build") {
        # Remove-Item -Path "build" -Recurse -Force
        # Write-Host "已清理旧的 build 文件夹"
    # }
# }

# 配置和构建
Write-Host "生成项目..."
cmake -S . -B build

Write-Host "编译中..."
cmake --build build --config $BUILD_TYPE
