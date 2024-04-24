main.exe: main.obj player.obj grenade.obj network_client.obj packets.obj main.res
	link main.obj player.obj grenade.obj network_client.obj packets.obj main.res raylib.lib enet64.lib opengl32.lib gdi32.lib winmm.lib User32.lib Shell32.lib ws2_32.lib /LIBPATH:.\raylib-5.0_win64_msvc16\lib /LIBPATH:.\enet\lib /NODEFAULTLIB:libcmt

main.res: main.rc
	rc main.rc

main.obj: main.c entities.h network.h packets.h
	cl /GA /c main.c /I.\raylib-5.0_win64_msvc16\include /I.\enet\include

player.obj: player.c entities.h
	cl /GA /c player.c /I.\raylib-5.0_win64_msvc16\include

grenade.obj: grenade.c entities.h
	cl /GA /c grenade.c /I.\raylib-5.0_win64_msvc16\include

network_client.obj: network_client.c network_client.h
	cl /GA /c network_client.c /I.\enet\include

packets.obj: packets.c packets.h
	cl /GA /c packets.c

server_debug.exe: server.c network.c network.h packets.c packets.h server.res
	cl /Zi /MT /EHsc /Oy- /Ob0 server.c network.c packets.c /Feserver_debug.exe /I.\enet\include /link server.res enet64.lib Kernel32.lib ws2_32.lib winmm.lib /LIBPATH:.\enet\lib

server.exe: server.c network.c network.h packets.c packets.h server.res
	cl /GA server.c network.c packets.c /I.\enet\include /link server.res enet64.lib Kernel32.lib ws2_32.lib winmm.lib /LIBPATH:.\enet\lib

server.res: server.rc
	rc server.rc

client.exe: client.c packets.c packets.h
	cl /GA client.c packets.c /I.\enet\include /link enet64.lib ws2_32.lib winmm.lib /LIBPATH:.\enet\lib

.PHONY: clean

clean:
	Remove-Item *.obj *.res