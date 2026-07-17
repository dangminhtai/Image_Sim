$target = "23127523_DOAN_FINAL"

# Clean if exists
if (Test-Path $target) {
    Remove-Item -Recurse -Force $target
}

# Create folders
New-Item -ItemType Directory -Force -Path "$target"
New-Item -ItemType Directory -Force -Path "$target\Source"
New-Item -ItemType Directory -Force -Path "$target\Release"
New-Item -ItemType Directory -Force -Path "$target\Docs"

# Copy Docs (only the essential latex source and final PDF)
Write-Host "Copying Docs..."
Copy-Item -Path "Docs\latex\main.pdf" -Destination "$target\23127523_BaoCao.pdf" -Force
Copy-Item -Path "Docs\latex" -Destination "$target\Docs\latex" -Recurse -Force
# Remove intermediate minted files to save space
if (Test-Path "$target\Docs\latex\_minted-main") {
    Remove-Item -Recurse -Force "$target\Docs\latex\_minted-main"
}
if (Test-Path "$target\Docs\latex\main.pdf") {
    Remove-Item -Force "$target\Docs\latex\main.pdf"
}

# Copy Source
Write-Host "Copying Source Code..."
Copy-Item -Path "src" -Destination "$target\Source\src" -Recurse -Force
Copy-Item -Path "include" -Destination "$target\Source\include" -Recurse -Force
Copy-Item -Path "CMakeLists.txt" -Destination "$target\Source" -Force

# Copy Data (only the necessary ones to avoid bloat, e.g. data/CD)
Write-Host "Copying Data..."
if (Test-Path "data\CD") {
    Copy-Item -Path "data\CD" -Destination "$target\data\CD" -Recurse -Force
}
if (Test-Path "data\features_CD.yml") {
    Copy-Item -Path "data\features_CD.yml" -Destination "$target\data" -Force
}
if (Test-Path "data\features_TMBuD.yml") {
    Copy-Item -Path "data\features_TMBuD.yml" -Destination "$target\data" -Force
}
if (Test-Path "data\vocab_CD") {
    Copy-Item -Path "data\vocab_CD" -Destination "$target\data\vocab_CD" -Recurse -Force
}
if (Test-Path "data\vocab_TMBuD") {
    Copy-Item -Path "data\vocab_TMBuD" -Destination "$target\data\vocab_TMBuD" -Recurse -Force
}
if (Test-Path "data\TMBuD\images") {
    Copy-Item -Path "data\TMBuD\images" -Destination "$target\data\TMBuD\images" -Recurse -Force
}

# Copy Release (exe + dll)
Write-Host "Copying Binaries..."
if (Test-Path "build\Release") {
    Copy-Item -Path "build\Release\*.exe" -Destination "$target\Release" -Force
}
if (Test-Path "C:\opencv\build\x64\vc16\bin\opencv_world4110.dll") {
    Copy-Item -Path "C:\opencv\build\x64\vc16\bin\opencv_world4110.dll" -Destination "$target\Release" -Force
}

Write-Host "Done packaging into folder '$target'."
