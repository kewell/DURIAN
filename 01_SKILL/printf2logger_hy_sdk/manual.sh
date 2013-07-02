

#'cp' ../06081800_orig/*.c . -a

for i in `cat list`;do FILE="$i";iconv -f GB2312 -t UTF8 < ${FILE} > ${FILE}.new;sh SDKERROR_2_logger_log.awk ${FILE}.new SDKERROR > ${FILE};iconv -t GB2312 -f UTF8 < ${FILE} > ${FILE}.new;'mv' ${FILE}.new ${FILE};done

for i in `cat list`;do FILE="$i";iconv -f GB2312 -t UTF8 < ${FILE} > ${FILE}.new;sh SDKINFO_2_logger_log.awk ${FILE}.new SDKINFO > ${FILE};iconv -t GB2312 -f UTF8 < ${FILE} > ${FILE}.new;'mv' ${FILE}.new ${FILE};done

