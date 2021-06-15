
New-Item -ItemType Directory -Force -Path ./atsfooters/build

Push-Location atsfooters/build
Invoke-Expression "cmake .."
Invoke-Expression "cmake --build . --config Release"
Invoke-Expression "ctest"
Pop-Location

Copy-Item "atsfooters/build/Release/atsfooters.dll" -Destination "dotnet-example"

Push-Location dotnet-example
Invoke-Expression "dotnet run"
Pop-Location
