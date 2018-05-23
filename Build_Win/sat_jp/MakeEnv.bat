mkdir .\Icons
copy ..\..\App\Resource_Win\miedit.rc .\
copy ..\..\App\Resource_Win\Icons\ .\Icons\
copy ..\..\App\Resource_Win\resource.h .\
copy ..\..\App\Resource_Win\miedit.exe.manifest .\
..\localizer.exe .\miedit.rc ..\..\App\Localize\main_Japanese.txt
..\localizer.exe .\miedit.rc ..\..\App\Localize\apppref_Japanese.txt
..\localizer.exe .\miedit.rc ..\..\App\Localize\menu_Japanese.txt
..\localizer.exe .\miedit.rc ..\..\App\Localize\modepref_Japanese.txt
