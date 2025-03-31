cmake -B build_64 -DCMAKE_BUILD_TYPE=Debug
cmake -B build_64rel -DCMAKE_BUILD_TYPE=Release
cmake -B build_64relwithdebinfo -DCMAKE_BUILD_TYPE=RelWithDebInfo

Write-Host "Build directories created successfully!"
