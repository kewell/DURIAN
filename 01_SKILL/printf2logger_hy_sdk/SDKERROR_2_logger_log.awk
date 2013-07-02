cat $1 | awk -F'[()"]'     \
'{                          \
    if ($0 ~ /SDKERROR/ && $0 !~ /sprintf/ && $0 !~ /fprintf/ && $0 ~ /;/ && $0 !~ /#define/) \
    {                       \
        if($0 ~ /\/\/SDKERROR/)                           \
        {   \
            TAB=match($0, "\t")     - 1;for (I=0; I<TAB; I++)printf"    ";               \
            BLANK=match($0, "/")     - 1;for (I=0; I<BLANK; I++)printf" ";                \
            printf "//";                                \
        }   \
        else    \
        {   \
            BLANK=match($0, "[a-Z]") - 1;for (I=0; I<BLANK; I++)printf" ";                \
        } \

        if($3 ~ /\\n/)       \
        {                   \
            print "logs(&g_stLog, LOG_ERROR, \"@%04d %s "substr($3, 0, index($3, "\\") - 1) "\", __LINE__, MOD_NAME"$4");"        \
        }                   \
        else                \
        {                   \
            print "logs(&g_stLog, LOG_ERROR, \"@%04d %s "$3"\", __LINE__, MOD_NAME"$4");"      \
        }                   \
    }                       \
    else                    \
    {                       \
        print               \
    }                       \
}'
