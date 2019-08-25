s="[Desktop Entry]\nName=海天鹰时钟\nComment=HTY Clock\nExec=`pwd`/HTYClock\nIcon=`pwd`/HTYClock.png\nPath=`pwd`\nTerminal=false\nCategories=System;"
echo -e $s > HTYClock.desktop
cp `pwd`/HTYClock.desktop ~/.local/share/applications/HTYClock.desktop