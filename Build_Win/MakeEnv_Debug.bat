mkdir Debug
mkdir Debug\jp
mkdir Debug\jp\TestPatterns
mkdir Debug\en
mkdir Debug\sdf
mkdir Debug\FuzzySearch
mkdir Debug\help
mkdir Debug\help\ja
mkdir Debug\diff
copy ..\App\Localize\Strings_Japanese.txt Debug\jp\Strings.txt
copy ..\App\Localize\Strings_English.txt Debug\en\Strings.txt
copy ..\App\sdf\*.* Debug\sdf
copy ..\AbsFramework\AbsFrame\UnicodeData.txt Debug\
copy ..\AbsFramework\AbsFrame\EastAsianWidth.txt Debug\
copy ..\libcurl\lib\DLL-Release\libcurl.dll Debug\
copy ..\App\FuzzySearch\KanaYuragi.txt Debug\FuzzySearch
copy ..\help\ja\*.* Debug\help\ja
copy ..\diff\*.* Debug\diff
copy ..\App\lua\mi.lua Debug\mi.lua
copy ..\App\Resource_Mac\TestPatterns\SJISChars Debug\jp\TestPatterns\SJISChars
