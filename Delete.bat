rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Bin\Debug
rmdir /s /q .\Client\Bin\ipch
rmdir /s /q .\Client\Bin\Release
rmdir /s /q .\Client\Bin\x64

rmdir /s /q .\Engine\Bin

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb
del /f /q MyD3D.VC.db
