
import os
import sys
import time

# Combine files into a single file with "const char*" data blocks.
crnt_dir = sys.argv[1]+"/"
src_dir = crnt_dir+"templates/"

if(len(sys.argv) < 3):
    print("use %s dir file ..."%(sys.argv[0]));

header = '''
/*
    %s
    THIS IS A GENERATED FILE
    %s
 */

'''


def emit_render(ofp, obj_list):
    ofp.write('''
#include "render.h"
Render* make_renderer() {

''')
    ofp.write("\n    Render* rend = new Render();\n\n")
    for item in obj_list:
        ofp.write("    rend->add(\"%s\", %s);\n"%(item, item))

    ofp.write("\n    return rend;\n}\n\n")

obj_list = []
for name in sys.argv[2:]:
    obj_list.append(os.path.basename(name).replace(".txt", ""))


#print("create file %s"%(crnt_dir+"templates.h"))
with open(crnt_dir+"templates.h", "w") as of:
    of.write(header%("templates.h", time.ctime()))
    of.write("#pragma once\n")
    #of.write("using namespace std;\n\n")
    of.write("//list the template strings\n")
    for name in obj_list:
        of.write("extern const char* %s"%(name+";\n"))
    of.write("\n")

#print("create file %s"%(crnt_dir+"templates.cpp"))
with open(crnt_dir+"templates.cpp", "w") as ofp:
    ofp.write(header%("templates.cpp", time.ctime()))
    for name in sys.argv[2:]:
        str = os.path.basename(name)
        ofp.write("const char* %s"%(str.replace(".txt", " = \n")))
        with open(name, "r") as ifp:
            for line in ifp:
                ofp.write("    \"%s\\n\"\n"%(line.strip()))
        ofp.write(";\n\n")

    emit_render(ofp, obj_list)



#print(sys.argv[2:])