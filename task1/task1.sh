#!/bin/bash

############################# FUNCTIONS #############################

# func: isEmpty - check variable for a null (""), 0, 0.0 values
# param: $value
function isEmpty(){
    local var="$1"

    # Return true if:
    # 1.    var is a null string ("" as empty string)
    # 2.    a non set variable is passed
    # 3.    a declared variable or array but without a value is passed
    # 4.    an empty array is passed
    if test -z "$var"
    then
        [[ $( echo "1" ) ]]
        return

    # Return true if var is zero (0 as an integer or "0" as a string)
    elif [ "$var" == 0 2> /dev/null ]
    then
        [[ $( echo "1" ) ]]
        return

    # Return true if var is 0.0 (0 as a float)
    elif [ "$var" == 0.0 2> /dev/null ]
    then
        [[ $( echo "1" ) ]]
        return
    fi

    [[ $( echo "" ) ]]
}


# func: CreateFiles - creates files in entered or default path with num copies 
# param: $path
# param: $num_files
function CreateFiles() {

################ Date
echo "Date: " $(date -R) >> $1
echo "---- Hardware ----" >> $1
################

################ CPU
CPU="$(grep -m 1 "model name" /proc/cpuinfo)"
echo "CPU: \"${CPU:13}\"" >> $1
################

################ RAM v1
RAM="$(grep -m 1 "MemTotal" /proc/meminfo)"
echo "RAM: ${RAM:17}" >> $1

#RAM v2
#RAM="$(sudo dmidecode -t memory | grep Size)"
#echo "RAM: ${RAM:20}" >> $1
################


################ MOTHERBOARD
#BoardInfo="$(sudo dmidecode -t 2)"
#Manufact="$(grep $BoardInfo "Manufacturer")"
#ProductName="$(grep $BoardInfo "Product Name")"
Manufact="$(sudo dmidecode -t 2 | grep "Manufacturer")"
ProductName="$(sudo dmidecode -t 2 | grep "Product Name")"
SN="$(sudo dmidecode -t 2 | grep "Serial Number")"
Manufact=${Manufact:15}
ProductName=${ProductName:15}
SN=${SN:15}

if [ -z "$Manufact" ] ; then
Manufact="Unknown"
fi
if [ -z "$ProductName" ] ; then
ProductName="Unknown"
fi
if [ -z "$SN" ] ; then
SN="Unknown"
fi

echo "Motherboard: \"$Manufact\", \"$ProductName\"" >> $1
echo "Serial System Number: $SN" >> $1
################ 

echo "---- System ----" >> $1
################ OS
Distrib="$(grep "DISTRIB_DESCRIPTION" /etc/*-release)"
Distrib=${Distrib:20}

#if (isEmpty $Distrib) ; then Distrib="Unknow"
if [ -z "$Distrib" ] ; then
Distrib="$(grep "PRETTY_NAME" /etc/*-release)"
Distrib=${Distrib:12}
fi
if [ -z "$Distrib" ] ; then
Distrib="Unknow"
fi

echo "OS Destribution: \"$Distrib\"" >> $1

Kernel="$(uname -mrs)"
Kernel=${Kernel:5}
echo "Kernel version: " >> $1
echo "Installation date: " >> $1
echo "Hostname: " >> $1
echo "Uptime: " >> $1
echo "Processes running: " >> $1
echo "User logged in: " >> $1
################


################ NETWORK
#cat /sbin/ifconfig
#ip addr show (ip -d addr)
echo "---- Network ----" >> $1
echo "lo: " >> $1
echo "eth0: " >> $1
echo "----\"EOF\"----" >> $1
################

#if [ -n $path]		#if path is not NULL
#delete prev output files
#create new files
#else			#if path NULL
			#create task1.out
#fi

}


# func: help - prints help in terminal
function help(){

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "Використовування: ./task1.sh [-h|--help] [-n num] [file]" 
	echo "num -- кількість файлів із результатами,"
	echo "file -- шлях та ім'я файла, у який треба записати результат;"
elif [[ "$LANG" = "ru_RU.UTF-8" || "$LANG" = "ru_UA.UTF-8" ]] ; then
	echo "Использование: ./task1.sh [-h|--help] [-n num] [file]" 
	echo "num -- количество файлов с результатами,"
	echo "file -- путь и имя файла, в который необходимо записать результат;"
else
	echo "Usage: ./task1.sh [-h|--help] [-n num] [file]" 
	echo "num -- files count with results,"
	echo "file -- path and file name to write result;"
fi


}
############################# FUNCTIONS #############################



############################# PARAMS PARSE #############################

while [ -n "$1" ]
do

case "$1" in

############# help
-h) 
help
exit 0 
;;
--help)
help
exit 0
;;
############# help

############# num & path
-n) #echo "Found the -n option"
shift

re='^[0-9]+$'
if ! [[ $1 =~ $re && $1 -gt 0 ]];	# check for integer val & var > 0
then

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: 'num' не може бути менше 1 або не цілим числом!" >&2
elif [[ "$LANG" = "ru_RU.UTF-8" || "$LANG" = "ru_UA.UTF-8" ]] ; then
	echo "ОШИБКА: 'num' не может быть меньше 1 или не целым числом!" >&2
else
	echo "ERROR: 'num' cannot be less then 1 or floating value!" >&2
fi
exit 1
fi

num_files=$1
path=$2  	#here may be a path
if [ -z "$path" ] ; then 	#if path NULL
path="task1.out"
echo "Path not entered. Default file name is $path"
fi

shift
;;
############# num & path

############# default (another)
*)
if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не корректні параметри!" >&2
elif [[ "$LANG" = "ru_RU.UTF-8" || "$LANG" = "ru_UA.UTF-8" ]] ; then
	echo "ОШИБКА: не корректные параметры!" >&2
else
	echo "ERROR: incorrect parameters!" >&2
fi
exit 2
;;
############# default (another)

esac	#end case

shift
done	#end while

#############################################################################


#rm -rf mydir		#delete all files in directory
#rm file*		#delete all files with beginning "file"
#rm file[1-8]		#delete files with endian file1...file8

dir_path=$(dirname $path)		#creating folders
if ! mkdir -p $dir_path ; then

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не вдалося створити папку!" >&2
elif [[ "$LANG" = "ru_RU.UTF-8" || "$LANG" = "ru_UA.UTF-8" ]] ; then
	echo "ОШИБКА: не удалось создать папку!" >&2
else
	echo "ERROR: could not create folder!" >&2
fi

exit 3
fi

filename="${path%.*}"
extension="${path##*.}"

n="0000"
if [ -f "$path" ] ; then
#rm $path
cd $dir_path
oldname="$filename.$extension"
#newname="$filename-$(date +%Y%m%d)-$n.$extension"
flag=1
while (( $flag ))
do
newname="$filename-$(date +%Y%m%d)-$n.$extension"
echo $newname
if [ -f "$newname" ] ; then		#if file exist
n="$n+1"
echo $n
else
mv $oldname $newname 			#rename the file
echo "file was renamed"
flag=0
fi
done
cd ..
fi


if ! touch $path ; then		#creating file

if [ "$LANG" = "uk_UA.UTF-8" ] ; then
	echo "ПОМИЛКА: не вдалося створити файл!" >&2
elif [[ "$LANG" = "ru_RU.UTF-8" || "$LANG" = "ru_UA.UTF-8" ]] ; then
	echo "ОШИБКА: не удалось создать файл!" >&2
else
	echo "ERROR: could not create file!" >&2
fi

exit 4
fi

echo "Hello" >> $path

#if [ -d "$path" ]
#then
#echo "$path is a directory"
#elif [ -f "$path" ]
#then
#echo "$path is a file"
#fi

exit 0



