# PowerShell 스크립트 예시: 파일 존재 여부에 따른 복사 제어

# assets 폴더를 복사할 대상 폴더
$targetDir = $args[0].Trim('"').TrimEnd('\') # 경로 끝의 백슬래시를 제거하고, 양쪽의 따옴표를 제거
Write-Output "Target Directory: $targetDir"

# assets 폴더와 shader 폴더의 경로
$assetsPath = "assets"
$shaderPath = "shader"

# assets 폴더 복사 함수 정의
function Copy-Assets {
    param (
        [string]$source,
        [string]$destination
    )
    
    Write-Output "Copying assets from $source to $destination"
    robocopy $source $destination /E /MT:8 /XD .git /XO # /XO 옵션을 사용하여 이미 존재하는 파일 제외
}

# shader 폴더 덮어쓰기 복사 함수 정의
function Overwrite-Shader {
    param (
        [string]$source,
        [string]$destination
    )
    Write-Output "Overwriting shader from $source to $destination"
    robocopy $source $destination /E /MT:8 /XD .git /IS # /IS 옵션을 사용하여 이미 존재하는 파일 덮어쓰기
}

# assets 폴더 복사 수행
Copy-Assets -source $assetsPath -destination "$targetDir\assets"

# shader 폴더 덮어쓰기 복사 수행
Overwrite-Shader -source $shaderPath -destination "$targetDir\shader"

Write-Output "Assets copied successfully."