
	
xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Library\
xcopy			/y			.\Engine\ThirdPartyLibrary\*.lib	.\Reference\Library\
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Release\
xcopy			/y/s		.\Engine\Public\*.*				.\Reference\Header\