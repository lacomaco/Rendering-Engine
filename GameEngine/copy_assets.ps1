# PowerShell ��ũ��Ʈ ����: ���� ���� ���ο� ���� ���� ����

# assets ������ ������ ��� ����
$targetDir = $args[0].Trim('"').TrimEnd('\') # ��� ���� �齽���ø� �����ϰ�, ������ ����ǥ�� ����
Write-Output "Target Directory: $targetDir"

# assets ������ shader ������ ���
$assetsPath = "assets"
$shaderPath = "shader"

# assets ���� ���� �Լ� ����
function Copy-Assets {
    param (
        [string]$source,
        [string]$destination
    )
    
    Write-Output "Copying assets from $source to $destination"
    robocopy $source $destination /E /MT:8 /XD .git /XO # /XO �ɼ��� ����Ͽ� �̹� �����ϴ� ���� ����
}

# shader ���� ����� ���� �Լ� ����
function Overwrite-Shader {
    param (
        [string]$source,
        [string]$destination
    )
    Write-Output "Overwriting shader from $source to $destination"
    robocopy $source $destination /E /MT:8 /XD .git /IS # /IS �ɼ��� ����Ͽ� �̹� �����ϴ� ���� �����
}

# assets ���� ���� ����
Copy-Assets -source $assetsPath -destination "$targetDir\assets"

# shader ���� ����� ���� ����
Overwrite-Shader -source $shaderPath -destination "$targetDir\shader"

Write-Output "Assets copied successfully."