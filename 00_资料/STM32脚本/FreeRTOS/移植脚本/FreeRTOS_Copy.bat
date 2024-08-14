::关掉回显
@echo off

::设置 FreeRTOS 源代码路径
set FreeRTOS_Path=C:\Users\wangq\Desktop\FreeRTOSv9.0.0\FreeRTOSv9.0.0
::echo %FreeRTOS_Path%

::设置在哪个位置移植 FreeRTOS 源代码
set Create_Path=.\FreeRTOS

::创建 FreeRTOS 文件夹
IF EXIST %Create_Path% (
    echo "当前路径下已存在 FreeRTOS 文件夹"
    
    ::删除已存在的 FreeRTOS 文件夹
    ::rd %Create_Path% /s /q
    
    ::结束
    goto :end
) ELSE (
    echo "创建 FreeRTOS 文件夹"
    
    :: 创建相关文件夹
    md %Create_Path%\Core\Source
    md %Create_Path%\Core\Include
    
    md %Create_Path%\portable\Keil
    md %Create_Path%\portable\MemMang
    md %Create_Path%\portable\RVDS
)

::复制 FreeRTOS 源代码
xcopy %FreeRTOS_Path%\FreeRTOS\Source\*.c %Create_Path%\Core\Source
xcopy %FreeRTOS_Path%\FreeRTOS\Source\include\*.h %Create_Path%\Core\Include
xcopy %FreeRTOS_Path%\FreeRTOS\Source\portable\Keil %Create_Path%\portable\Keil /e

xcopy %FreeRTOS_Path%\FreeRTOS\Source\portable\MemMang %Create_Path%\portable\MemMang /e
xcopy %FreeRTOS_Path%\FreeRTOS\Source\portable\RVDS %Create_Path%\portable\RVDS /e

:end
pause
