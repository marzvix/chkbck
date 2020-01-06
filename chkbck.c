#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* objetivo: listar os arquivos (savesets) de backup que se encontram em server */

char cmdlin[1024];
char newdate[32];
char newserver[32];

//char server[1024] = "vitac.dyndns-server.com";
//char *repo = "/srv/vitac/Documentos/VITAC/BACKUPS/APOLLO";

char server[1024] = "ckcont.duckdns.org";
/* char *repo = "/home/CKCONTABIL_DOCS/CKCONTABIL/TI/BACKUPS/APOLLO"; */
char *repo = "/var/local/TI/BACKUPS/APOLLO";

//char server[1024] = "freenas";
//char *repo = "/mnt/triton/CKCONTABIL/CKCONTABIL_DOCS/CKCONTABIL/TI/BACKUPS/APOLLO";

char year[5];
char mon[3];
char day[3];

int ssetcnt_expect = 5;
int ssetcnt;

char *outpcmd="/tmp/$0_$$.tmp";

#define MAX_SSETS 16
#define MAX_SSETLEN 512
char ssetdata[MAX_SSETS][MAX_SSETLEN];
char * procnumdata[MAX_SSETS];
int procnumcnt;

typedef struct sattr {
    int mark;
    char *size;
    char *hora;
    char *procnum;
    char *fname;
} ssattr;

ssattr ssetattrs[MAX_SSETS];

/*
 *  UTILITÁRIOS
 */

/* executa um comando no sistema e carrega um ponteiro tipo FILE
   apontando a saída (stdout) gerada pelo comando */

int open_cmd(const char *cmdlin, FILE **fp )
{
    int sts;

    *fp = popen(cmdlin, "r" );

    if (*fp == NULL) {
        puts ("problemas...");
        return 1; /* falha */
    }

    return 0; /* sucesso */
}

/* libera o ponteiro para os dados de  saída do comando */
int close_cmd(FILE *fp)
{
    return pclose(fp);
}

/*
 *  AÇÕES de execução
 */

/* comando de teste dos savesets */
void test_sset( const char *sset)
{
    FILE *fp;
    sprintf(cmdlin, "gzip -t /tmp/%s", sset);

    open_cmd( cmdlin, &fp);
    close_cmd( fp );
}

/* processo de recuperação começa aqui */
void recover(void)
{
    FILE *fp;
    int i;

    for (i = 0; i < ssetcnt; i++ )
        if(ssetattrs[i].mark == 1) {
            printf("Recuperando [%d] %s...\n", i+1, ssetattrs[i].fname);
            sprintf(cmdlin, "scp %s:%s/%s/%s/%s /tmp",
                    server,
                    repo,
                    year,
                    mon,
                    ssetattrs[i].fname
                    );
            open_cmd( cmdlin, &fp);
            close_cmd( fp );
            printf("Testando %s...\n", ssetattrs[i].fname);
            test_sset(ssetattrs[i].fname);
        }
}

/* seletor de opções selecionadas pelo usuário */
void selecttorec(int *selvet, int selcnt)
{
    int i;

    for (i = 0; i < ssetcnt; i++ )
            ssetattrs[i].mark = 0;

    while (selcnt--) {
        if ( *selvet > 0 && *selvet <= (ssetcnt+1))
            ssetattrs[(*selvet)-1].mark = 1;
        else
            printf("índice inválido: %d\n", *selvet);
        selvet ++;
    }
}

/* diálogo de seleção dos savesets */
void askforoptions()
{
    int i;
    int selcnt;
    int selvet[MAX_SSETS];
    char resp[CHAR_MAX];

    puts ("Recuperar todos [S/N/X] ? ");

 reask:
    fgets(resp, 4, stdin);
    if ( resp[0] == 'X' )
             puts("Cancelado.");

    else if ( resp[0] == 'S' )
        for (i = 0; i <= ssetcnt; i++, selcnt++)
            selvet[i] = i+1;

    else if ( resp[0] == 'N' ) {
        char *s, *p;
        puts("Selecionar quais são para recuperar (índices sep. por espaços):");
        
        fgets( resp, CHAR_MAX, stdin);
        i = 0;
        selcnt = 0;
        s = p = resp;
        while (*s && (s-resp < CHAR_MAX))
            if (isspace(*s)) {
                *s = 0;
                s ++;
                //                printf("%d\n", atoi(p));
                selvet[i++] = atoi(p);
                while(*s && isspace(*s)) s++;
                p = s;
                selcnt++;
                }
            else
                s++;
    }
    else {
             puts("Resposta invalida");
             goto reask;
    }
    puts ("ok");
    selecttorec(selvet, selcnt);
}

/* carregamento (na memória) dos dados dos savesets */
int load_ssets(char *cmdlin)
{
    FILE *fp;
    int sts;

    char path[1024];

    if ( open_cmd( cmdlin, &fp )) return 1;

    ssetcnt = 0;
    while (fgets(path, 1024, fp ) != NULL ) {
      /* /\*Debug*\/ printf("%s\n", path); */
      sprintf(&ssetdata[ssetcnt][0], "%s", path);
        if (ssetcnt == MAX_SSETS) {
            puts("Esgotado o array para dados de saveset.");
            break;
        }
        ssetcnt ++;
    }
    
    return close_cmd(fp);
}

/* separação dos atributos listados dos savesets */
parse_ssets()
{
    int i;
    char *p;
    int procnumcntant = 0;
    procnumcnt = 0;
    procnumdata[procnumcnt] = "nil";

    for ( i = 0; i < ssetcnt; i++)
        {
            p = (void *) &ssetdata[i][0];

            ssetattrs[i].size = p;
            p = strchr(p, ';');
            *p++ = 0;
            //            puts(ssetattrs[i].size);
            ssetattrs[i].hora = p;
            p = strchr(p, ';');
            *p ++= 0;
            //            puts (ssetattrs[i].hora);
            ssetattrs[i].fname = p;
            p = strchr(p, ';');
            *p ++= 0;
            //            puts (ssetattrs[i].fname);
            ssetattrs[i].procnum = p;
            *(p + strlen(p) -1) = 0;

            if ( strcmp(ssetattrs[i].procnum, procnumdata[procnumcntant])) {
                    procnumdata[procnumcnt] = ssetattrs[i].procnum;
                    procnumcntant = procnumcnt;
                    procnumcnt++;
                }
            //            puts (ssetattrs[i].procnum);
         }
}

/* lista de savesets */
list_ssets()
{
    int i;
    printf("Savesets encontrados: %d\n", ssetcnt);
    for ( i = 0; i < ssetcnt; i++)
        printf("\t[%2d] %-32s %9d (bytes) gerado às %s\n",
               i+1, ssetattrs[i].fname,
               ssetattrs[i].size,
               ssetattrs[i].hora);
}

/* lista de (numeros) de processos que geraram os savesets */
/* nota: estes números servem para agrupar os savesets feitos
   numa mesma rodada de execução dos backups */
list_procs()
{
    int i;
    printf("Processos encontrados: %d\n", procnumcnt);

    for ( i = 0; i < procnumcnt; i++)
        printf("\t[%2d] %s \n",
               i+1, procnumdata[i]);
}

/* busca pelos savesets armazenados no repositório, pré organizados
   por ano, mes e dia de execução de backup e a geração deles */
int search_ssets()
{
    /* este comando usa:
       ssh: conectar ao servidor
       ls:  listar os savesets
       sed/cut: separar os campos retornados pelo ls
    */
    char *cmd =
        "ssh %s "
        "ls -lrt %s/%s/%s/%s%s%s* | "
        "sed 's/\\/.*\\///;s/ \\+/;/g;s/_\\([0-9]\\+\\).*/&;\\1/' | "
        "cut -d ';' -f 5,7,8,9";

    int sts;

    sprintf( cmdlin, cmd , server, repo, year, mon, year, mon, day );

    printf ( "Backups em %s-%s-%s\n", day, mon, year );
    printf ( "Repositorio\n\tserver:\t%s\n\tdir:\t%s\n", server, repo );

    load_ssets(cmdlin);
    parse_ssets();
    list_ssets();
    if (procnumcnt > 1 )
        list_procs();

    return sts;
}

/* inicialização da variável contendo o nome do servidor */
void get_server (char *srv)
{
    strcpy(server, srv);
}

/* stub message: uma orientação via cli */
void help_msg( char *cmdname )
{
    
    printf("Uso: %s [dd-mm-aaaa] [server]\n", cmdname);
}

/* inicializa as váriaveis de data com a data de hoje (default) */
void setdefdate(void)
{
    time_t t;
    struct tm *hoje;

    t = time(NULL);
    hoje = localtime(&t);

    sprintf (day, "%02d", hoje->tm_mday);
    sprintf (mon, "%02d", 1 + hoje->tm_mon);
    sprintf (year, "%04d", 1900 + hoje->tm_year);
}

/* separa a data fornecida pelo usuário em partes: dia-mes-ano
   e sobrepoe a data default */
int parse_date( char *dt )
{
    if ( strlen(dt) == 10 ) {
        dt[2] = 0;
        dt[5] = 0;
        if (( atoi (dt) > 0 && atoi (dt) <= 31 ) &&
            ( atoi (&dt[3]) > 0 && atoi (&dt[3]) <= 12 ))
            {
                sprintf(day, "%02d", atoi(dt));
                sprintf(mon, "%02d", atoi(&dt[3]));
                sprintf(year, "%04d", atoi(&dt[6]));
                return 0;
            }
    }
    printf ("%s: data inválida\n", dt);
    return 1;
}

/* analisa e traduz os argumentos da linha de comandos */
int parse_cli(int argc, char ** argv)
{
    if (argc > 1 ) {
        if ( parse_date( argv[1] )) {
            help_msg( argv[0] );
            return 1;
        }
        if (argc > 2 )
            get_server( argv[2] );
    }
    return 0;
}

/* controla  sequencialmente as etapas de execução */
int main(int argc, char ** argv)
{
    setdefdate();

    if(parse_cli(argc, argv)) return 1;

    search_ssets();

    if (ssetcnt > 1) {
        askforoptions();
        recover();
    }

    return 0;
}
