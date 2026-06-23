cd "C:\Users\cenor\Desktop\quantum"

Remove-Item src\moc_MainWindow.cpp -ErrorAction SilentlyContinue

& "C:\Qt\6.11.1\mingw_64\bin\moc.exe" src\MainWindow.h -o src\moc_MainWindow.cpp

& "C:\Qt\Tools\mingw1310_64\bin\g++.exe" -std=c++17 `
  src\main.cpp src\MainWindow.cpp src\moc_MainWindow.cpp src\SyntaxHighlighter.cpp `
  -o main.exe `
  -IC:\Qt\6.11.1\mingw_64\include `
  -IC:\Qt\6.11.1\mingw_64\include\QtWidgets `
  -IC:\Qt\6.11.1\mingw_64\include\QtCore `
  -IC:\Qt\6.11.1\mingw_64\include\QtGui `
  -Isrc `
  -LC:\Qt\6.11.1\mingw_64\lib `
  -lQt6Widgets -lQt6Core -lQt6Gui

if ($LASTEXITCODE -eq 0) {
    Write-Host "BUILD SUCCEEDED" -ForegroundColor Green
} else {
    Write-Host "BUILD FAILED" -ForegroundColor Red
}
