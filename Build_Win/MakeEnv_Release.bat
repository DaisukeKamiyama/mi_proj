mkdir Release
mkdir Release\jp
mkdir Release\jp\TestPatterns
mkdir Release\en
mkdir Release\sdf
mkdir Release\FuzzySearch
mkdir Release\help
mkdir Release\help\ja
mkdir Release\diff
copy ..\App\Localize\Strings_Japanese.txt Release\jp\Strings.txt
copy ..\App\Localize\Strings_English.txt Release\en\Strings.txt
copy ..\App\sdf\*.* Release\sdf
copy ..\AbsFramework\AbsFrame\UnicodeData.txt Release\
copy ..\AbsFramework\AbsFrame\EastAsianWidth.txt Release\
copy ..\libcurl\lib\DLL-Release\libcurl.dll Release\
copy ..\App\FuzzySearch\KanaYuragi.txt Release\FuzzySearch
copy ..\help\ja\*.* Release\help\ja
copy ..\diff\*.* Release\diff
copy ..\App\lua\mi.lua Release\mi.lua
copy ..\App\Resource_Mac\TestPatterns\SJISChars Release\jp\TestPatterns\SJISChars
