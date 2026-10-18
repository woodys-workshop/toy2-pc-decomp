#if 0

// The game has a statically linked single threaded CRT
// The names of a lot of these are probably wrong and will need to be changed over time

// LIBRARY: TOY2 0x0047C7F0
// ___setargv

// LIBRARY: TOY2 0x004B2AA0
// __mkgmtime

// LIBRARY: TOY2 0x004CEB00
// __fpmath

// LIBRARY: TOY2 0x004CEB18
// __cfltcvt_init

// LIBRARY: TOY2 0x004CEB60
// __ftol

// LIBRARY: TOY2 0x004CEB87
// _qsort

// LIBRARY: TOY2 0x004CECDB
// _shortsort

// LIBRARY: TOY2 0x004CED29
// _swap

// LIBRARY: TOY2 0x004CED60
// _strncpy

// LIBRARY: TOY2 0x004CEE5E
// _free

// LIBRARY: TOY2 0x004CEE8D
// _sprintf

// LIBRARY: TOY2 0x004CEEDF
// __onexit

// LIBRARY: TOY2 0x004CEF4C
// _atexit

// LIBRARY: TOY2 0x004CEF5E
// ___onexitinit

// LIBRARY: TOY2 0x004CEF8D
// _atol

// LIBRARY: TOY2 0x004CF018
// _atoi

// LIBRARY: TOY2 0x004CF023
// __atoi64

// LIBRARY: TOY2 0x004CF0DD
// _fscanf

// LIBRARY: TOY2 0x004CF0F3
// _fgetc

// LIBRARY: TOY2 0x004CF118
// _strlwr

// LIBRARY: TOY2 0x004CF1B6
// _fclose

// LIBRARY: TOY2 0x004CF20C
// __fsopen

// LIBRARY: TOY2 0x004CF22C
// _fopen

// LIBRARY: TOY2 0x004CF23F
// _strtok

// LIBRARY: TOY2 0x004CF2DB
// _vsprintf

// LIBRARY: TOY2 0x004CF32C
// __cinit

// LIBRARY: TOY2 0x004CF359
// _exit

// LIBRARY: TOY2 0x004CF36A
// __exit

// LIBRARY: TOY2 0x004CF37B
// __cexit

// LIBRARY: TOY2 0x004CF38A
// __c_exit

// LIBRARY: TOY2 0x004CF399
// _doexit

// LIBRARY: TOY2 0x004CF432
// __initterm

// LIBRARY: TOY2 0x004CF44C
// _malloc

// LIBRARY: TOY2 0x004CF45E
// __nh_malloc

// LIBRARY: TOY2 0x004CF48A
// __heap_alloc

// LIBRARY: TOY2 0x004CF4C0
// _strstr

// LIBRARY: TOY2 0x004CF54A
// _rand

// LIBRARY: TOY2 0x004CF568
// _strerror

// LIBRARY: TOY2 0x004CF594
// _fread

// LIBRARY: TOY2 0x004CF680
// __chkstk

// LIBRARY: TOY2 0x004CF6AF
// _fwrite

// LIBRARY: TOY2 0x004CF7B9
// _abs

// LIBRARY: TOY2 0x004CF7D0
// __CIpow

// LIBRARY: TOY2 0x004CF7E9
// _pow

// LIBRARY: TOY2 0x004CF7F2
// msvc_float_method8

// LIBRARY: TOY2 0x004CF9C5
// msvc_float_method7

// LIBRARY: TOY2 0x004CF9ED
// _fputc

// LIBRARY: TOY2 0x004CFA21
// _fprintf

// LIBRARY: TOY2 0x004CFA53
// _remove

// LIBRARY: TOY2 0x004CFA88
// _ftell

// LIBRARY: TOY2 0x004CFBE0
// _fseek

// LIBRARY: TOY2 0x004CFD62
// __amsg_exit

// LIBRARY: TOY2 0x004CFD87
// _fast_error_exit

// LIBRARY: TOY2 0x004CFDAB
// __statusfp

// LIBRARY: TOY2 0x004CFDBE
// __clearfp

// LIBRARY: TOY2 0x004CFDD2
// __control87

// LIBRARY: TOY2 0x004CFE07
// __controlfp

// LIBRARY: TOY2 0x004CFE1D
// __fpreset

// LIBRARY: TOY2 0x004CFE47
// __abstract_cw

// LIBRARY: TOY2 0x004CFED9
// __hw_cw

// LIBRARY: TOY2 0x004CFF62
// __abstract_sw

// LIBRARY: TOY2 0x004CFFA0
// __CIacos

// LIBRARY: TOY2 0x004CFFB4
// _acos

// LIBRARY: TOY2 0x004CFFBD
// msvc_float_method9

// LIBRARY: TOY2 0x004D0070
// __alldiv

// LIBRARY: TOY2 0x004D0120
// __allmul

// LIBRARY: TOY2 0x004D0154
// ??2@YAPAXI@Z

// LIBRARY: TOY2 0x004D0162
// ??3@YAXPAX@Z

// LIBRARY: TOY2 0x004D0170
// _strrchr

// LIBRARY: TOY2 0x004D0197
// _getenv

// LIBRARY: TOY2 0x004D0214
// __setdefaultprecision

// LIBRARY: TOY2 0x004D0226
// __ms_p5_test_fdiv

// LIBRARY: TOY2 0x004D0264
// __ms_p5_mp_test_fdiv

// LIBRARY: TOY2 0x004D028D
// __forcdecpt

// LIBRARY: TOY2 0x004D02E7
// __cropzeros

// LIBRARY: TOY2 0x004D0335
// __positive

// LIBRARY: TOY2 0x004D034D
// __fassign

// LIBRARY: TOY2 0x004D038B
// __cftoe

// LIBRARY: TOY2 0x004D048F
// __cftof

// LIBRARY: TOY2 0x004D056D
// __cftog

// LIBRARY: TOY2 0x004D0608
// __cftoe_g

// LIBRARY: TOY2 0x004D062F
// __cftof_g

// LIBRARY: TOY2 0x004D0652
// __cfltcvt

// LIBRARY: TOY2 0x004D06A3
// __shift

// LIBRARY: TOY2 0x004D06C8
// __heap_init

// LIBRARY: TOY2 0x004D0704
// __heap_term

// LIBRARY: TOY2 0x004D077F
// __set_sbh_threshold

// LIBRARY: TOY2 0x004D0796
// ___sbh_heap_init

// LIBRARY: TOY2 0x004D07D4
// ___sbh_find_block

// LIBRARY: TOY2 0x004D07FF
// ___sbh_free_block

// LIBRARY: TOY2 0x004D0B2A
// ___sbh_alloc_block

// LIBRARY: TOY2 0x004D0E33
// ___sbh_alloc_new_region

// LIBRARY: TOY2 0x004D0EE4
// ___sbh_alloc_new_group

// LIBRARY: TOY2 0x004D0FDF
// ___sbh_resize_block

// LIBRARY: TOY2 0x004D13A6
// ___sbh_heap_check

// LIBRARY: TOY2 0x004D16D5
// __flsbuf

// LIBRARY: TOY2 0x004D17EA
// __output

// LIBRARY: TOY2 0x004D1F2B
// _write_char

// LIBRARY: TOY2 0x004D1F60
// _write_multi_char

// LIBRARY: TOY2 0x004D1F91
// _write_string

// LIBRARY: TOY2 0x004D1FC9
// _get_int_arg

// LIBRARY: TOY2 0x004D1FD6
// _get_int64_arg

// LIBRARY: TOY2 0x004D1FE6
// _get_short_arg

// LIBRARY: TOY2 0x004D1FF4
// _realloc

// LIBRARY: TOY2 0x004D2114
// __msize

// LIBRARY: TOY2 0x004D213D
// __isctype

// LIBRARY: TOY2 0x004D21B2
// __input

// LIBRARY: TOY2 0x004D2BD7
// __hextodec

// LIBRARY: TOY2 0x004D2C0E
// _fgetc_0

// LIBRARY: TOY2 0x004D2C28
// __un_inc

// LIBRARY: TOY2 0x004D2C3F
// __whiteout

// LIBRARY: TOY2 0x004D2C63
// __filbuf

// LIBRARY: TOY2 0x004D2D40
// _strcpy

// LIBRARY: TOY2 0x004D2D50
// _strcat

// LIBRARY: TOY2 0x004D2E30
// ___crtLCMapStringA

// LIBRARY: TOY2 0x004D3054
// _strncnt

// LIBRARY: TOY2 0x004D307F
// __close

// LIBRARY: TOY2 0x004D3132
// __freebuf

// LIBRARY: TOY2 0x004D315D
// _fflush

// LIBRARY: TOY2 0x004D3198
// __flush

// LIBRARY: TOY2 0x004D31F4
// __flushall

// LIBRARY: TOY2 0x004D31FD
// _flsall

// LIBRARY: TOY2 0x004D326A
// __openfile

// LIBRARY: TOY2 0x004D33DA
// __getstream

// LIBRARY: TOY2 0x004D3468
// __callnewh

// LIBRARY: TOY2 0x004D34A0
// _strchr

// LIBRARY: TOY2 0x004D355C
// __read

// LIBRARY: TOY2 0x004D3760
// _memcpy

// LIBRARY: TOY2 0x004D3A95
// __write

// LIBRARY: TOY2 0x004D3C61
// __fFEXP

// LIBRARY: TOY2 0x004D3D37
// _zerotoxdone

// LIBRARY: TOY2 0x004D3D82
// _expbigret

// LIBRARY: TOY2 0x004D3DA1
// _rtforexpinf

// LIBRARY: TOY2 0x004D3DAE
// __ffexpm1

// LIBRARY: TOY2 0x004D3DF1
// _isintTOS

// LIBRARY: TOY2 0x004D3E16
// _isintTOSret

// LIBRARY: TOY2 0x004D3E17
// _notanint

// LIBRARY: TOY2 0x004D3E1E
// _evenint

// LIBRARY: TOY2 0x004D3E25
// _usepowhlp

// LIBRARY: TOY2 0x004D4030
// __startTwoArgErrorHandling

// LIBRARY: TOY2 0x004D4047
// __startOneArgErrorHandling

// LIBRARY: TOY2 0x004D4090
// __twoToTOS

// LIBRARY: TOY2 0x004D40A5
// __load_CW

// LIBRARY: TOY2 0x004D40BC
// __convertTOStoQNaN

// LIBRARY: TOY2 0x004D40D5
// __fload_withFB

// LIBRARY: TOY2 0x004D4118
// __checkTOS_withFB

// LIBRARY: TOY2 0x004D4165
// __check_overflow_exit

// LIBRARY: TOY2 0x004D4179
// __check_range_exit

// LIBRARY: TOY2 0x004D421C
// __powhlp

// LIBRARY: TOY2 0x004D434C
// __d_inttype

// LIBRARY: TOY2 0x004D43B1
// __stbuf

// LIBRARY: TOY2 0x004D443E
// __ftbuf

// LIBRARY: TOY2 0x004D447B
// __dosmaperr

// LIBRARY: TOY2 0x004D44E2
// __ioinit

// LIBRARY: TOY2 0x004D46B0
// __lseek

// LIBRARY: TOY2 0x004D474A
// __XcptFilter

// LIBRARY: TOY2 0x004D488B
// _xcptlookup

// LIBRARY: TOY2 0x004D48CE
// __wincmdln

// LIBRARY: TOY2 0x004D4926
// __setenvp

// LIBRARY: TOY2 0x004D49DF
// __setargv

// LIBRARY: TOY2 0x004D4A78
// _parse_cmdline

// LIBRARY: TOY2 0x004D4C2C
// ___crtGetEnvironmentStringsA

// LIBRARY: TOY2 0x004D4D60
// __global_unwind2

// LIBRARY: TOY2 0x004D4D80
// __unwind_handler

// LIBRARY: TOY2 0x004D4DA2
// __local_unwind2

// LIBRARY: TOY2 0x004D4E0A
// __abnormal_termination

// LIBRARY: TOY2 0x004D4E2D
// __NLG_Notify1

// LIBRARY: TOY2 0x004D4E36
// __NLG_Notify

// LIBRARY: TOY2 0x004D4E58
// __except_handler3

// LIBRARY: TOY2 0x004D4F15
// __seh_longjmp_unwind@4

// LIBRARY: TOY2 0x004D4F30
// __FF_MSGBANNER

// LIBRARY: TOY2 0x004D4F69
// __NMSG_WRITE

// LIBRARY: TOY2 0x004D50ED
// __mbsnbicoll

// LIBRARY: TOY2 0x004D5130
// _strlen

// LIBRARY: TOY2 0x004D51AB
// ___wtomb_environ

// LIBRARY: TOY2 0x004D5219
// __tolower

// LIBRARY: TOY2 0x004D5221
// _tolower

// LIBRARY: TOY2 0x004D52EC
// __ZeroTail

// LIBRARY: TOY2 0x004D5335
// __IncMan

// LIBRARY: TOY2 0x004D538B
// __RoundMan

// LIBRARY: TOY2 0x004D5417
// __CopyMan

// LIBRARY: TOY2 0x004D5432
// __FillZeroMan

// LIBRARY: TOY2 0x004D543E
// __IsZeroMan

// LIBRARY: TOY2 0x004D5459
// __ShrMan

// LIBRARY: TOY2 0x004D54E6
// __ld12cvt

// LIBRARY: TOY2 0x004D5652
// __ld12tod

// LIBRARY: TOY2 0x004D5668
// __ld12tof

// LIBRARY: TOY2 0x004D567E
// __ld12told

// LIBRARY: TOY2 0x004D56F8
// __atodbl

// LIBRARY: TOY2 0x004D5725
// __atoldbl

// LIBRARY: TOY2 0x004D5753
// __atoflt

// LIBRARY: TOY2 0x004D5780
// __fptostr

// LIBRARY: TOY2 0x004D57F7
// __fltout

// LIBRARY: TOY2 0x004D585B
// ___dtold

// LIBRARY: TOY2 0x004D5920
// _memset

// LIBRARY: TOY2 0x004D5980
// _memmove

// LIBRARY: TOY2 0x004D5CB5
// __fptrap

// LIBRARY: TOY2 0x004D5CBE
// __getbuf

// LIBRARY: TOY2 0x004D5D02
// __isatty

// LIBRARY: TOY2 0x004D5D28
// ___initstdio

// LIBRARY: TOY2 0x004D5DCD
// ___endstdio

// LIBRARY: TOY2 0x004D5DE1
// _wctomb

// LIBRARY: TOY2 0x004D5E50
// __aulldiv

// LIBRARY: TOY2 0x004D5EC0
// __aullrem

// LIBRARY: TOY2 0x004D5F35
// ___crtGetStringTypeA

// LIBRARY: TOY2 0x004D607E
// _mbtowc

// LIBRARY: TOY2 0x004D6146
// _isalpha

// LIBRARY: TOY2 0x004D6174
// _isupper

// LIBRARY: TOY2 0x004D619C
// _islower

// LIBRARY: TOY2 0x004D61C4
// _isdigit

// LIBRARY: TOY2 0x004D61EC
// _isxdigit

// LIBRARY: TOY2 0x004D6219
// _isspace

// LIBRARY: TOY2 0x004D6241
// _ispunct

// LIBRARY: TOY2 0x004D6269
// _isalnum

// LIBRARY: TOY2 0x004D6297
// _isprint

// LIBRARY: TOY2 0x004D62C5
// _isgraph

// LIBRARY: TOY2 0x004D62F3
// _iscntrl

// LIBRARY: TOY2 0x004D631B
// ___isascii

// LIBRARY: TOY2 0x004D6328
// ___toascii

// LIBRARY: TOY2 0x004D6330
// ___iscsymf

// LIBRARY: TOY2 0x004D636D
// ___iscsym

// LIBRARY: TOY2 0x004D63B0
// __allshl

// LIBRARY: TOY2 0x004D63CF
// _ungetc

// LIBRARY: TOY2 0x004D643D
// __alloc_osfhnd

// LIBRARY: TOY2 0x004D64D2
// __set_osfhnd

// LIBRARY: TOY2 0x004D6549
// __free_osfhnd

// LIBRARY: TOY2 0x004D65C3
// __get_osfhandle

// LIBRARY: TOY2 0x004D6600
// __open_osfhandle

// LIBRARY: TOY2 0x004D669D
// __commit

// LIBRARY: TOY2 0x004D670B
// __sopen

// LIBRARY: TOY2 0x004D775B
// __87except

// LIBRARY: TOY2 0x004D782B
// __set_exp

// LIBRARY: TOY2 0x004D7894
// __set_bexp

// LIBRARY: TOY2 0x004D78B8
// __sptype

// LIBRARY: TOY2 0x004D7912
// __decomp

// LIBRARY: TOY2 0x004D79D3
// __frnd

// LIBRARY: TOY2 0x004D79E5
// __copysign

// LIBRARY: TOY2 0x004D7A06
// __chgsign

// LIBRARY: TOY2 0x004D7A2B
// __scalb

// LIBRARY: TOY2 0x004D7A41
// __logb

// LIBRARY: TOY2 0x004D7B28
// __nextafter

// LIBRARY: TOY2 0x004D7DBD
// __finite

// LIBRARY: TOY2 0x004D7DD1
// __isnan

// LIBRARY: TOY2 0x004D7DFF
// __fpclass

// LIBRARY: TOY2 0x004D7EA2
// __ismbbkprint

// LIBRARY: TOY2 0x004D7EB3
// __ismbbkpunct

// LIBRARY: TOY2 0x004D7EC4
// __ismbbalnum

// LIBRARY: TOY2 0x004D7ED8
// __ismbbalpha

// LIBRARY: TOY2 0x004D7EEC
// __ismbbgraph

// LIBRARY: TOY2 0x004D7F00
// __ismbbprint

// LIBRARY: TOY2 0x004D7F14
// __ismbbpunct

// LIBRARY: TOY2 0x004D7F25
// __ismbblead

// LIBRARY: TOY2 0x004D7F36
// __ismbbtrail

// LIBRARY: TOY2 0x004D7F47
// __ismbbkana

// LIBRARY: TOY2 0x004D7F6E
// _x_ismbbtype

// LIBRARY: TOY2 0x004D7F9F
// __setmbcp

// LIBRARY: TOY2 0x004D8138
// _getSystemCP

// LIBRARY: TOY2 0x004D8182
// _CPtoLCID

// LIBRARY: TOY2 0x004D81B5
// _setSBCS

// LIBRARY: TOY2 0x004D81DE
// _setSBUpLow

// LIBRARY: TOY2 0x004D8363
// __getmbcp

// LIBRARY: TOY2 0x004D8373
// ___initmbctable

// LIBRARY: TOY2 0x004D838F
// ___crtMessageBoxA

// LIBRARY: TOY2 0x004D8418
// ___crtCompareStringA

// LIBRARY: TOY2 0x004D8695
// _strncnt_0

// LIBRARY: TOY2 0x004D86C0
// ___crtsetenv

// LIBRARY: TOY2 0x004D8847
// _findenv

// LIBRARY: TOY2 0x004D889F
// _copy_environ

// LIBRARY: TOY2 0x004D8906
// ___addl

// LIBRARY: TOY2 0x004D8927
// ___add_12

// LIBRARY: TOY2 0x004D8985
// ___shl_12

// LIBRARY: TOY2 0x004D89B3
// ___shr_12

// LIBRARY: TOY2 0x004D89E0
// ___mtold12

// LIBRARY: TOY2 0x004D8AA7
// ___strgtold12

// LIBRARY: TOY2 0x004D8F78
// ___STRINGTOLD

// LIBRARY: TOY2 0x004D8FB4
// _$I10_OUTPUT

// LIBRARY: TOY2 0x004D9247
// _calloc

// LIBRARY: TOY2 0x004D92C4
// __fcloseall

// LIBRARY: TOY2 0x004D931C
// __chsize

// LIBRARY: TOY2 0x004D9462
// __handle_qnan1

// LIBRARY: TOY2 0x004D94B5
// __handle_qnan2

// LIBRARY: TOY2 0x004D9514
// __except1

// LIBRARY: TOY2 0x004D95AC
// __except2

// LIBRARY: TOY2 0x004D9651
// __raise_exc

// LIBRARY: TOY2 0x004D9904
// __handle_exc

// LIBRARY: TOY2 0x004D9B1B
// __umatherr

// LIBRARY: TOY2 0x004D9BA3
// __set_errno

// LIBRARY: TOY2 0x004D9BC9
// __get_fname

// LIBRARY: TOY2 0x004D9BEE
// __errcode

// LIBRARY: TOY2 0x004D9C1B
// msvc_method_1

// LIBRARY: TOY2 0x004D9C1E
// __statfp

// LIBRARY: TOY2 0x004D9C2C
// __clrfp

// LIBRARY: TOY2 0x004D9C3B
// __ctrlfp

// LIBRARY: TOY2 0x004D9C5E
// __set_statfp

// LIBRARY: TOY2 0x004D9CB4
// _ldexp

// LIBRARY: TOY2 0x004D9E9A
// __mbschr

// LIBRARY: TOY2 0x004D9F0D
// __strdup

// LIBRARY: TOY2 0x004D9F38
// ___ld12mul

// LIBRARY: TOY2 0x004DA158
// ___multtenpow12

// LIBRARY: TOY2 0x004DA1D4
// __setmode

// LIBRARY: TOY2 0x004DBBB0
// __strcmpi

// LIBRARY: TOY2 0x004DBC3C
// _atof

// LIBRARY: TOY2 0x004DBC89
// _printf

// LIBRARY: TOY2 0x004DBCBA
// __fltin

// CRT variables the library functions above refer to

// GLOBAL: TOY2 0x004DF000
// ___xc_a

// GLOBAL: TOY2 0x004DF00C
// ___xc_z

// GLOBAL: TOY2 0x004DF010
// ___xi_a

// GLOBAL: TOY2 0x004DF020
// ___xi_z

// GLOBAL: TOY2 0x00508E2C
// __FPinit

// GLOBAL: TOY2 0x00508E60
// ___app_type

// GLOBAL: TOY2 0x00508E80
// __cfltcvt_tab

// GLOBAL: TOY2 0x00508EA8
// __pctype

// GLOBAL: TOY2 0x005090B4
// ___mb_cur_max

// GLOBAL: TOY2 0x005090B8
// ___decimal_point

// GLOBAL: TOY2 0x005090D0
// __sys_errlist

// GLOBAL: TOY2 0x00509180
// __sys_nerr

// GLOBAL: TOY2 0x00509398
// ___badioinfo

// GLOBAL: TOY2 0x00509418
// __First_FPE_Indx

// GLOBAL: TOY2 0x0050941C
// __Num_FPE

// GLOBAL: TOY2 0x00509424
// __fpecode

// GLOBAL: TOY2 0x00509428
// __NLG_Destination

// GLOBAL: TOY2 0x00509500
// _iob

// GLOBAL: TOY2 0x00509780
// __aexit_rtn

// GLOBAL: TOY2 0x00509AF8
// __pow10pos

// GLOBAL: TOY2 0x00509C58
// __pow10neg

// GLOBAL: TOY2 0x00B62430
// __adjust_fdiv

// GLOBAL: TOY2 0x00B62438
// errno

// GLOBAL: TOY2 0x00B6243C
// __doserrno

// GLOBAL: TOY2 0x00B62440
// __umaskval

// GLOBAL: TOY2 0x00B62454
// ___argc

// GLOBAL: TOY2 0x00B62458
// ___argv

// GLOBAL: TOY2 0x00B62460
// __environ

// GLOBAL: TOY2 0x00B62470
// __pgmptr

// GLOBAL: TOY2 0x00B62478
// __exitflag

// GLOBAL: TOY2 0x00B624AC
// __aenvptr

// GLOBAL: TOY2 0x00B624B4
// ___error_mode

// GLOBAL: TOY2 0x00B624CC
// ___lc_handle

// GLOBAL: TOY2 0x00B624E4
// ___lc_codepage

// GLOBAL: TOY2 0x00B624EC
// __newmode

// GLOBAL: TOY2 0x00B624F0
// ?_pnhHeap@@3P6AHI@ZA

// GLOBAL: TOY2 0x00B624F4
// __stdbuf

// GLOBAL: TOY2 0x00B624FC
// __pxcptinfoptrs

// GLOBAL: TOY2 0x00B62608
// __adbgmsg

// GLOBAL: TOY2 0x00B62640
// __cflush

// GLOBAL: TOY2 0x00B62648
// __commode

// GLOBAL: TOY2 0x00B62660
// __fmode

// GLOBAL: TOY2 0x00E4D9A8
// ___mbcodepage

// GLOBAL: TOY2 0x00E4D9BC
// ___ismbcodepage

// GLOBAL: TOY2 0x00E4DBC8
// ___piob

// GLOBAL: TOY2 0x00E4EBE0
// __nstream

// GLOBAL: TOY2 0x00E4EC00
// ___pioinfo

// GLOBAL: TOY2 0x00E4ED00
// __nhandle

// GLOBAL: TOY2 0x00E4ED04
// ___sbh_sizeHeaderList

// GLOBAL: TOY2 0x00E4ED08
// ___sbh_indGroupDefer

// GLOBAL: TOY2 0x00E4ED0C
// ___sbh_pHeaderScan

// GLOBAL: TOY2 0x00E4ED10
// ___sbh_pHeaderDefer

// GLOBAL: TOY2 0x00E4ED14
// ___sbh_cntHeaderList

// GLOBAL: TOY2 0x00E4ED18
// ___sbh_pHeaderList

// GLOBAL: TOY2 0x00E4ED1C
// __crtheap

// GLOBAL: TOY2 0x00E4ED20
// __acmdln

// GLOBAL: TOY2 0x00E4ED24
// ___env_initialized

// GLOBAL: TOY2 0x00E4ED28
// ___mbctype_initialized

// GLOBAL: TOY2 0x00E4ED2C
// ___onexitend

// GLOBAL: TOY2 0x00E4ED30
// ___onexitbegin

#endif
