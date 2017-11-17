#!/bin/sh
export PATH=/home/alex/Downloads/build-llvm/bin:$PATH
JSON_PATH="./"
PROJECT_PATH="/home/alex/Downloads/llvm/tools/clang/tools/example/test"
SRC_DIR="./src ./include"
rm -rf replacements.txt
rm -rf processed_files.txt
#find $SRC_DIR -iname "*.c" > files.txt
#find $SRC_DIR -iname "*.cpp" >> files.txt
getJsonFiles ./
cp files.txt run.sh
cat files.txt | xargs sed -i '1s/^/#include <stdio.h>\n/'
sed -i "s|^|example |" run.sh
sed -i "s|$| $JSON_PATH $PROJECT_PATH r|" run.sh
chmod +x run.sh
./run.sh

# cat files.txt | xargs sed -i '1s/^/#define DRS7 { DEFEND_777 \n/'
# cat files.txt | xargs sed -i '1s/^/#define DRE7 } \n/'
# cat files.txt | xargs sed -i '1s/^/#define DEFEND_777 printf ("end %s \\n",__func__);\n/'
# cat files.txt | xargs sed -i '1s/^/#define DEFBEG_777 printf ("start %s \\n",__func__);\n/'


#make
