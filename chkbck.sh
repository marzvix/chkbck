#!/bin/bash
# objetivo: listar os arquivos (savesets) de backup que se encontram em server

[[ $1 == "-h" ]] && echo "Uso: ${0##*/} [iso-date] [servidor]" && exit

DATE="`date +'%d-%m-%Y'`"
[[ $1 ]] && DATE="`date -d$1 +'%d-%m-%Y'`"

dtin="${DATE//-/ }"
read -a dt <<<$dtin

DAY=${dt[0]}
MON=${dt[1]}
YEAR=${dt[2]}

#SERV="ckcont.duckdns.org"
SERV="bisold"
[[ $2 ]] && SERV=$2

#REPO="/home/CKCONTABIL_DOCS/CKCONTABIL/TI/BACKUPS/APOLLO"
REPO="/mnt/triton/CKCONTABIL/CKCONTABIL_DOCS/CKCONTABIL/TI/BACKUPS/APOLLO"

RMTCMD="ls -ltr"
LOCCMD="ssh"

OUTPCMD="/tmp/$0_$$.tmp"

SSETCNT=5 # qtd de savesets esperados

$LOCCMD $SERV $RMTCMD $REPO/$YEAR/$MON/$YEAR$MON$DAY* > $OUTPCMD
RET=$?

cat $OUTPCMD | awk \
-v ret=$RET \
-v dt=$DATE \
-v serv=$SERV \
-v repo=$REPO/$YEAR/$MON/ \
-v ssetcnt=$SSETCNT \
'BEGIN {
  printf ( "Backups em %s\n", dt )
  printf ( "Repositorio\n\tserver:\t%s\n\tdir:\t%s\nSavesets:\n", serv, repo )
}

{ 
  split($8, a, "/")
  printf ( "\t%s\t%s\t%9d\n", a[10], $7, $5 )
}

END {

  if ( ret != 0 ) 
    printf ("\tErro: Savesets não encontrados.\n")

  else if ( NR < ssetcnt )
    printf ("\tErro: apenas %d saveset%s encontrado%s. Falta%s %d\n", 
        NR, 
          (NR>1)?"s":"", (NR>1)?"s":"", (ssetcnt-NR>1)?"m":"", 
        ssetcnt - NR)
  else if ( NR > ssetcnt )
    printf ("\tAlerta: existe%s %d saveset%s a mais do que o previsto.\n", 
        (NR-ssetcnt>1)?"m":"", NR-ssetcnt, (NR-ssetcnt>1)?"s":"")
}
' 

rm $OUTPCMD


