try { 
    cls

    $CurrentDir = $PSScriptRoot
    $CurrentDir
    
    # Rename Control folder
    "Rename Control folder"
    Rename-Item "$CurrentDir\src\Control" -NewName Control_

    # Check vcpkg install:
    $VcpkgFolder = "$CurrentDir\src\vcpkg"
    "Test to see if folder [$VcpkgFolder]  exists"
    if (Test-Path -Path $VcpkgFolder) {
        "Path exists!"      
    } else {
        "Path doesn't exist. Will try to update submodule"
        git --version
        git submodule update --remote  
    }
    # Test again:
    if (!(Test-Path -Path $VcpkgFolder)) {
        "Can't get the vcpkg submodule. Try to pull the repo again and enable submodules"
        return
    }

    "Check if vcpkg.exe exists:"
    if (!(Test-Path -Path $VcpkgFolder\vcpkg.exe -PathType Leaf))
    {
        "vcpkg.exe does not exists, will try to generate it"
        Start-Process -FilePath $VcpkgFolder\bootstrap-vcpkg.bat -Wait -NoNewWindow
    }else{
        "$VcpkgFolder\vcpkg.exe exists"
    }
    
    # Check again
    if (!(Test-Path -Path $VcpkgFolder\vcpkg.exe -PathType Leaf)){
        "Can't generate vcpkg.exe. Run bootstrap-vcpkg.bat manually"
        return
    }

    "Run vcpkg.exe install"
    Set-Location $CurrentDir\src\  
    # Sadly only one triplet can be installed, so added in the pre-build event as well:  
    Start-Process -FilePath $VcpkgFolder\vcpkg.exe -ArgumentList "Install --triplet=x64-windows" -Wait -NoNewWindow
    Set-Location $CurrentDir
}
catch {
  Write-Host "An error occurred:"
  Write-Host $_.ScriptStackTrace
  Write-Host $_
}
finally {
    # Always rename back:
    "Rename Control folder back"
    Set-Location $CurrentDir
    Rename-Item "$CurrentDir\src\Control_" -NewName Control -ErrorAction SilentlyContinue
}