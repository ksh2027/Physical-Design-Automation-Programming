python3 draw.py [INPUT] [OUTPUT] [PICTURE_NAME] (-i) (-p)
#-i would also draw initial graph
#-p would also draw partition graph (the cell distribution on 2 chips before doing legalization ) 



ex:
python3 draw.py Case/case0.txt output.txt drawing/layout0
python3 draw.py Case/case0.txt output.txt drawing/layout0 -i
python3 draw.py Case/case0.txt output.txt drawing/layout0 -i -p
python3 draw.py Case/case0.txt output.txt drawing/layout0 -p