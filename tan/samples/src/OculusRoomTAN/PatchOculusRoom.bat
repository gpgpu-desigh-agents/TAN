@echo off
xcopy /Y /R "..\..\..\..\Thirdparty\OculusSDK1.17\Samples\OculusRoomTiny\OculusRoomTiny (DX11)\main.cpp" .
..\..\..\..\Thirdparty\patchagent\patch.exe -R main.cpp main.cpp.patch
xcopy /Y /R  "..\..\..\..\Thirdparty\OculusSDK1.17\Samples\OculusRoomTiny_Advanced\Common\Win32_DirectXAppUtil.h" .
..\..\..\..\Thirdparty\patchagent\patch.exe -R Win32_DirectXAppUtil.h Win32_DirectXAppUtil.h.patch
