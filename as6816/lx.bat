cls
rem Internal Errors
rem as6816 -gloxffr -i ".define INTERNAL" l6816
rem aslink -f l6816
rem asxscn -3 -i l6816.rst
rem External Errors
as6816 -gloxffr l6816
aslink -f l6816
asxscn -3 -i l6816.rst
