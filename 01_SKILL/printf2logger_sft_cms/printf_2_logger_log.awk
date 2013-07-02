cat $1 | awk -F'[()"]'     \
'{                          \
    if ($0 ~ /printf/ && $0 !~ /sprintf/ && $0 !~ /fprintf/ && $0 ~ /;/ && $0 !~ /#define/) \
    {                       \
        if($0 ~ /\/\/printf/)                           \
        {   \
            TAB=match($0, "\t")     - 1;for (I=0; I<TAB; I++)printf"\t";               \
            BLANK=match($0, "/")     - 1;for (I=0; I<BLANK; I++)printf" ";                \
            printf "//";                                \
        }   \
        else    \
        {   \
            BLANK=match($0, "[a-z]") - 1;for (I=0; I<BLANK; I++)printf" ";                \
        } \

        if($3 ~ /\\n/)       \
        {                   \
            print "logger_log(&g_stLog, LOG_ALL, \"@%04d %s "substr($3, 0, index($3, "\\") - 1) "\", __LINE__, SYS_NAME_NAME"$4");"        \
        }                   \
        else                \
        {                   \
            print "logger_log(&g_stLog, LOG_ALL, \"@%04d %s "$3"\", __LINE__, SYS_NAME_NAME"$4");"      \
        }                   \
    }                       \
    else                    \
    {                       \
        print               \
    }                       \
}'
