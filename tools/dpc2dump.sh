#!/bin/bash
# This script dumps the DPC2 registers to a new script to write them

echo "#!/bin/bash"
echo "# DPC2 dump on `date`"
echo ""
echo "./iocwritereg 10180 0"
for f in `seq 0 1023`; do 
	HEXVAL=`printf "%03x" $f`
	./iocwritereg 10180 $HEXVAL > /dev/null
	./iocreadreg 10184 | sed -e "s/R.*is/.\/iocwritereg $HEXVAL/"
done
	
