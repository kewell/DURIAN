==============================================================================

find . -type f -name "*.?" > list
#grep "printf" -Ir --exclude-dir="common" * | grep -v "sprintf" | grep -v "fprintf" | grep -v "define" | grep ";" | wc -l

for i in `cat list`;do FILE="$i";iconv -f GB2312 -t UTF8 < ${FILE} > ${FILE}.new;sh ../run.sh ${FILE}.new > ${FILE};iconv -t GB2312 -f UTF8 < ${FILE} > ${FILE}.new;'mv' ${FILE}.new ${FILE};done

for i in `cat list`;do vi -c ":%s/#include\ <stdio.h>/#include\ <stdio.h>\r#include\ \"local.h\"/g" -c ":wq" $i;done

==============================================================================


grep "printf" -Ir --exclude-dir="common" * | grep -v "sprintf" | grep -v "fprintf" | grep -v "define" | grep ";" | grep -v '\\n'

grep "printf" -Ir --exclude-dir="common" * | grep -v "sprintf" | grep -v "fprintf" | grep -v "define" | grep ";" | grep '\\r\\n'
