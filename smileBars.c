
/*
    SMILE BARS, THE GAME
*/

#include<stdio.h>
#include<stdlib.h>
#include<windows.h>
#include <conio2.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define Y_MAX 23
#define Y_MIN 2
#define X_MAX 80
#define X_MIN 1

#define PONTUACAO_MAXIMA 30
#define TEMPO_PARA_APARECER_PODER 30 //o tempo varia se o jogador morrer fazendo com que os poderes apare�am somente quando o desempenho do jogador � melhor
#define TEMPO_JOGADOR_COM_PODER 10

#define FUNDOX 61
#define FUNDOY 20
#define OPX 56
#define ACIMA 72
#define ABAIXO 80

#define INICIAR 10
#define RECORDE 13
#define SAIR 16
#define TAM_RECORDES 5
#define VALOR_PODER 6


typedef struct
{
    int x;
    int y;
} COORDENADA;
typedef enum
{
    NULO,
    LEVITACAO,
    SUPER_PULO,
    INVENCIBILIDADE,
    PARAR_TEMPO,
    BAIXA_GRAVIDADE,
    VIDA_EXTRA,
    PONTOS_EXTRAS

} COR_PODER;

typedef enum
{
    CIMA,
    DIREITA,
    ESQUERDA,
    BAIXO
} DIRECOES;

typedef struct
{
    int cor;
    int direcao; // 0..2 para cada uma das dire��es
    int tamanho; // n�mero de caracteres que ocupa
    int velocidade;
    COORDENADA posicao;
} PLATAFORMA;

typedef struct
{
    char nome[51];
    int ch; // ASCII do caracter que representa o jogador
    int cor; // cor que o caracter vai ser plotado
    int pulando; //  que indica se o jogador est� ou n�o pulando
    int pulo2_ativo;  // Vari�vel que diz o segundo pulo est� habilitado
    int pulou2;
    int caindo; // que indica se o jogador est� caindo
    int pontuacao;
    int poder;
    int morreu;
    int tempo_poder;
    int alcance; // inteiro que indica o alcance do pulo do jogador
    COORDENADA posicao;
} JOGADOR;

typedef struct
{
    COORDENADA posicao; //posi��o em que o poder ser� plotado
    int efeito; //cor do poder a ser plotado
    int visivel; //n�mero (0 ou 1) para identificar se h� ou n�o algum poder na tela
    int tempo_visivel; // Tempo que o poder est� aparecendo.
    int tempo_invisivel; // Tempo que o poder n�o est� aparecendo.
} PODER;

//FUN��ES PLATAFORMA

void desenha_plataforma(PLATAFORMA plataforma);    // FUN��O QUE IMPRIME A PLATAFORMA NA TELA CONFORME O TAMANHO
void desenha_plataforma_cor_do_fundo(PLATAFORMA plataforma); // FUN��O USADA PARA APAGAR UMA PLATAFORMA QUE SOBE
int coloca_plataformas_na_tela(PLATAFORMA lista[], int quantidade, int cont); //COMBINA MOVIMENTA PLATAFORMA e CARREGA_PLATAFORMAS
void atualiza_plataformas(PLATAFORMA plataformas[], int nr_plataformas); // Fun��o verifica se alguma plataforma j� saiu da tela e sorteia uma nova posi��o de inicio
void movimenta_plataforma(PLATAFORMA *plataforma); //FUN��O QUE RECEBE UMA PLATAFORMA E FAZ O MOVIMENTO CONFORME A DIRE��O
void desenha_plataformas(PLATAFORMA plataforma[], int num); // DESENHA PLATAFORMAS SEM FAZER A MOVIMENTA��O
void carrega_plataformas(PLATAFORMA plataformas[], int *nr_plataformas); // CARREGAS AS PLATAFORMAS DO TXT E ALTERA UM VETOR DE PLATAFORMAS
int plataforma_aparecendo(PLATAFORMA plataforma); //Fun��o verifica se a plataforma em quest�o est� aparecendo

// Fun��es JOGADOR

void move_jogador(JOGADOR *jogado, int direcao);
void jogador_cai(JOGADOR *jogador, int cont); // MOVIMENTA O JOGADOR PARA BAIXO
void jogador_pula(JOGADOR *jogador);// IMPLEMENTA O PULO DO JOGADOR
int jogador_sob_plataforma(JOGADOR jogador, PLATAFORMA plataforma); // RETORNA BOOLEAN QUE INDICA SE O JOGADOR EST� SOB A PLATAFORMA PASSADA COMO PAR�METRO
int jogador_em_alguma_plataforma(JOGADOR jogador, PLATAFORMA plataforma[],int num, int *num_plat); //RETORNA BOOLEAN QUE INDICA SE O JOGADOR EST� SOB ALGUMA PLATAFORMA
int jogador_vivo(JOGADOR jogador); //RETORNA BOOLEAN QUE INDICA SE O JOGADOR EST� NA �REA DE JOGO
void inicializa_jogador(JOGADOR *jogador);
void le_nome(JOGADOR *jogador); // L� o nome do jogador
void gerencia_controle(JOGADOR *jogador, int sob_plat, int alcance_aux, int tecla, int cont);
void gerencia_movimentacao_no_ar(JOGADOR *jogador, int sob_plat, int cont);
// Fun��es Screen

void desenha_mapa(); // Desenha mapa na tela
void fixa_mapa();    // Fixa mapa no topo
void imprime_hud(char nome[], int pontos, int poder, int vida);
void opcao();
void seletor();
void opcao_2();
void seletor_2();
void plota_barra();
void colocamenu(); // Imprime o menu inicial
void colocamenu_2(); //Imprime o segundo menu

// Fun��es Arquivo
void le_arquivo(); // L� os recordes
void ordena_recordes(JOGADOR lista[], int tam);
void cria_recordes();

// Fun��es Poder
void inicializa_poder(PODER *poder);
int verifica_poder(JOGADOR *jogador, PODER *poder);
void limpa_poder(PODER *poder);
void plota_poder(PODER poder);

// Fun��es de ger�ncia hibrida

void laco_jogo();    // Chama as fun��es necess�rias para o jogo
void gerencia_movimentacao(JOGADOR *jogador, PLATAFORMA plataformas[], int alcance_aux, int nr_plataformas, int cont);

//FUN��ES DE ENTRADA
int le_tecla();
void aguarda_space();


//MAIN S� CHAMA OUTRAS FUN��ES
int main()
{

    plota_barra(); // plota a barra do menu inicial
    colocamenu(); // chama menu inicial que chama as demais fun��es
    getchar();
    return 0;
}

/*a fun��o  desenha_plataforma recebe como argumento uma estrutura
PLATAFORMA e usa a coordenada da plataforma como o v�rtice inicial do ret�ngulo da plataforma
a partir do qual a plataforma � colorida at� que chegue ao limite do tamanho dado na estrutura
com a altura de um caractere*/

void desenha_plataforma(PLATAFORMA plataforma)
{
    int i,x,y,tam;
    x=plataforma.posicao.x;
    y=plataforma.posicao.y;
    tam=plataforma.tamanho;

    for(i=x; i<x+tam; i++)
    {
        if(i>1&&i<80 && y>1 && y<25)
        {
            textbackground(DARKGRAY);
            putchxy (i, y, ' ');
        }
    }
    textbackground(0);
    fixa_mapa();

}
/*a fun��o  desenha_plataformas quando o tempo estiver parado. Serve para evitar que elas sejam apagadas pelo jogador*/
void desenha_plataformas(PLATAFORMA plataforma[], int num)
{
    int i,x,y,tam,k;

    for(k=0; k<num; k++)
    {
        x=plataforma[k].posicao.x;
        y=plataforma[k].posicao.y;
        tam=plataforma[k].tamanho;
        for(i=x; i<x+tam; i++)
        {
            if(i>1&&i<80 && y>1 && y<25)
            {
                textbackground(DARKGRAY);
                putchxy (i, y, ' ');
            }
        }
    }

    textbackground(0);
    fixa_mapa();

}

/*a fun��o seguinte tem como base a fun��o desenha_plataforma, por�m desenha uma plataforma com a cor do fundo do texto
tornando poss�vel utiliza-la para "apagar" uma plataforma desenhada. O argumento de entrada � o mesmo da fun��o desenha_plataforma*/
void desenha_plataforma_cor_do_fundo(PLATAFORMA plataforma) // fun��o usada para apagar uma plataforma que sobe
{
    int i,x,y,tam;
    x=plataforma.posicao.x;
    y=plataforma.posicao.y;
    tam=plataforma.tamanho;

    for(i=x; i<x+tam; i++)
    {
        if(i>1&&i<80 && y>1 && y<25)
        {
            textbackground(0);
            putchxy (i, y, ' ');
        }
    }

    fixa_mapa();
}
/*a fun��o a seguir utiliza as fun��es desenha_plataforma e a fun��o desenha_plataforma_cor_do_fundo para dar a sensa��o de movimento para a plataforma.
em vez de receber uma estrutura plataforma, a fun��o recebe um ponteiro de plataforma, pois � necess�rio que os dados sejam modificados para que a cada chamada da fun��o
sejam atualizadas as coordenadas da plataforma*/

void movimenta_plataforma(PLATAFORMA *plataforma)
{
    int i,x,y,tam;
    PLATAFORMA plataforma_aux;

    x=plataforma->posicao.x;
    y=plataforma->posicao.y;
    tam=plataforma->tamanho;
    plataforma_aux=*plataforma; // serve para armazenar o valor antigo da plataforma

    switch(plataforma -> direcao)
    {
    case 0:

        plataforma->posicao.y=y-1; // sobe plataforma
        desenha_plataforma(*plataforma);
        desenha_plataforma_cor_do_fundo(plataforma_aux); // apaga plataforma anterior
        break;
    case 1:
        plataforma->posicao.x=x+1;
        desenha_plataforma(*plataforma);
        textbackground(0);
        if(x<80 && x != 1)
        {
            putchxy (x, y, ' ');
        }
        break;
    case 2:
        plataforma->posicao.x=x-1;
        desenha_plataforma(*plataforma);
        textbackground(0);
        if(x+tam>1 && x+tam != 80)
        {
            putchxy (x+tam, y, ' ');
        }
        break;
    }

    fixa_mapa();
}


/*a fun��o desenha mapa n�o recebe dados, ela apenas printa o quadro
que limita o jogo, usando a fun��o fixa_mapa para colocar o mapa do jogo no canto superior esquerdo*/

void desenha_mapa()
{
    int lin,col;
    for(lin=1; lin<=25; lin++)
        for(col=1; col<=80; col++)
        {
            if(lin==1)
            {
                textbackground (BLUE);
                putchxy (col,lin, ' ');
            }
            if(col==1 || col == 80)
            {
                if(lin!=1)
                {
                    textbackground (BLACK);
                    textcolor(WHITE);
                    putchxy (col,lin, '+');
                }

            }
            if(lin==25 && lin != 1 && lin != 80)
            {
                textbackground (RED);
                putchxy (col,lin, ' ');
            }
        }
    fixa_mapa();
}
/*a fun��o a seguir movimenta o jogador na tela, apagando a posi��o atual
do jogador e colocando ele em uma posi��o nova de acordo com a dire��o recebida
pelo teclado do computador*/

void move_jogador (JOGADOR *jogador, int direcao)
{

    int x,y;
    int nome = jogador->ch;
    x=jogador->posicao.x;
    y=jogador->posicao.y;
    long int r,t;

    fflush(stdin);
    textbackground(0);
    textcolor(jogador->cor);
    switch(direcao)
    {
    case CIMA: //CIMA
        jogador->posicao.y=y-1;
        if(jogador->posicao.y<=1 && jogador->poder == INVENCIBILIDADE)
            jogador->posicao.y=y;
        else
        {
            if(y-1>1 && y-1<25 && x>1 && x<80)
                putchxy(x,y-1,nome);
            if(y<25 && y>1 && x>1 && x<80)
                putchxy (x,y, ' ');
        }

        break;
    case DIREITA: //DIREITA

        jogador->posicao.x=x+1;
        if(jogador->posicao.x>=X_MAX && jogador->poder == INVENCIBILIDADE)
            jogador->posicao.x=x;
        else
        {
            if(x+1<80 && x+1>1 && y<25 && y>1)
                putchxy(x+1,y,nome);
            if(x<80 && x>1 && y<25 && y>1)
                putchxy (x,y, ' ');
        }
        break;
    case ESQUERDA: //ESQUERDA

        jogador->posicao.x=x-1;
        if(jogador->posicao.x<=X_MIN && jogador->poder == INVENCIBILIDADE)
            jogador->posicao.x=x;
        else
        {
            if(x-1>1 && x-1<80 && y<25 && y>1)
                putchxy(x-1,y,nome);
            if(x>1 && x<80 && y<25 && y>1)
                putchxy (x,y, ' ');
        }
        break;
    case BAIXO: //BAIXO

        jogador->posicao.y++;
        if(jogador->posicao.y>=Y_MAX+2 && jogador->poder == INVENCIBILIDADE)
            jogador->posicao.y=y;
        else
        {
            if(y+1>1 && y+1<25 && x>1 && x<80)
                putchxy(x,y+1,nome);
            if(y>1 && y<25 && x>1 && x<80)
                putchxy(x,y, ' ');
        }
        break;
    }


}

/*a fun��o a seguir � respons�vel por fazer o personagem cair, ela recebe o ponteiro de jogador
uma vez que � necess�rio mudar o estado do jogador para "caindo', e recebe um inteiro "count" respons�vel pela ger�ncia da velocidade da queda*/

void jogador_cai(JOGADOR *jogador, int cont)
{
    if(jogador->poder == BAIXA_GRAVIDADE)
    {
        if(cont%3<2) // O racioc�nio utilizado aqui � o mesmo utilizado na manuten��o das velocidades dasa plataformas (aqui dividimos a velovidade por 2
        {
            jogador->caindo=1;
            if(!jogador->pulou2)
                jogador->pulo2_ativo=1;
            else
                jogador->pulo2_ativo=0;

            move_jogador(jogador,BAIXO);
        }

    }
    else
    {
        jogador->caindo=1;
        if(!jogador->pulou2)
            jogador->pulo2_ativo=1;
        else
            jogador->pulo2_ativo=0;

        move_jogador(jogador,BAIXO);
    }

}

/*
    A fun��o jogador_pula recebe o ponteiro de jogador
    (para que seja alterado o estado do jogador para "pulando")
*/
void jogador_pula(JOGADOR *jogador)
{
    if(jogador->alcance>0 && jogador->poder!=SUPER_PULO)
    {
        if(!jogador->pulou2)
            jogador->pulo2_ativo=1;
        else
            jogador->pulo2_ativo=0;
        jogador->alcance-=1;

        move_jogador(jogador,CIMA);
        jogador->pulando = 1;
    }
    else if(jogador->alcance>0 && jogador->poder==SUPER_PULO)
    {

        if(jogador->pulou2)
            jogador->pulo2_ativo=0;
        else
            jogador->pulo2_ativo=1;
        jogador->alcance-=1;
        move_jogador(jogador, CIMA);
        move_jogador(jogador, CIMA);
        jogador->pulando = 1;
    }
    else
        jogador->pulando=0;


}
/*a fun��o a seguir recebe um elemento do tipo JOGADOR e um do tipo PLATAFORMA.
A fun��o serve para verificar se o personagem se encontra(retorna 1) ou n�o(retorna 0) em cima de uma
plataforma, utilizando a posi��o do personagem, da plataforma e o tamanho da plataforma*/
int jogador_sob_plataforma(JOGADOR jogador, PLATAFORMA plataforma)
{
    int j_x=jogador.posicao.x;
    int j_y=jogador.posicao.y;
    int p_x=plataforma.posicao.x;
    int p_y=plataforma.posicao.y;

    if(j_x>=p_x && j_x<=p_x+plataforma.tamanho && j_y==p_y-1)
        return 1;
    return 0;
}
/*a fun��o a seguir recebe um jogador, um array de plataformas, um n�mero (referente ao n�mero de plataformas) e
um ponteiro de um n�mero que representa o �ndice da plataforma que o personagem est� sobre*/
int jogador_em_alguma_plataforma(JOGADOR jogador, PLATAFORMA plataforma[],int num, int *num_plat)
{
    int i;

    for(i=0; i<num; i++)
    {
        if(jogador_sob_plataforma(jogador,plataforma[i]))
        {
            *num_plat=i;
            return 1;
        }
    }
    return 0;
}
/*a fun��o jogador_morreu coloca a mensagem "jogador Morreu" na tela*/

int jogador_morreu() // FUN��O PRECISA GRAVAR A PONTUA��O NO ARQUIVO, IMPRIMIR O RANKING E PERGUNTAR SE QUER REINICIAR O JOGO
{
    textcolor(WHITE);
    textbackground(RED);
    cputsxy(32,25,"Jogador Morreu!");
    return -1;
}
/*a fun��o a seguir verifica se o jogador est� vivo atrav�s da sua posi��o, dada atrav�s da estrutura jogador*/
int jogador_vivo(JOGADOR jogador)
{
    int x,y;
    x=jogador.posicao.x;
    y=jogador.posicao.y;

    if(x<=1||x>=80 ||y<=1 || y>=25)
        return 0;
    return 1;
}
/*a fun��o a seguir n�o recebe argumentos, ela verifica o estado das teclas de fun��o do jogo
e retorna uma dire��o */
int le_tecla()
{
    if(GetKeyState(VK_SPACE) & 0x80)
        return CIMA;
    if(GetKeyState(VK_RIGHT) & 0x80)
        return DIREITA;
    if(GetKeyState(VK_LEFT) & 0x80)
        return ESQUERDA;
    else
        return 69;
}

/*a fun��o prepara o programa para escrever qualquer coisa no come�o do mapa*/
void fixa_mapa()
{
    gotoxy(2,1);
}

/*a fun��o recebe um array de plataformas e um ponteiro do tipo inteiro referente ao n�mero de plataformas
que ser�o lidas do arquivo*/
void carrega_plataformas(PLATAFORMA plataformas[], int *nr_plataformas)
{
    char url[] = "plataformas.txt";
    int tam, vel,i,quantidade;
    FILE *arq;
    arq=fopen(url,"r");
    if(arq == NULL)
    {
        textbackground(RED);
        cputsxy(15,25, "Erro ao ler arquivo!");
    }
    else
        gotoxy(5,10);
    i=0;
    fscanf(arq,"%i \n",&quantidade);
    *nr_plataformas=quantidade;
    srand(time(0));

    while((fscanf(arq,"%i %i \n",&tam, &vel))!=EOF )
    {
        int r_direcao, rx, ry;



        rx=(int) X_MIN +(rand() / (double) RAND_MAX * (X_MAX));
        ry=(int) Y_MIN +(rand() / (double) RAND_MAX * (Y_MAX));
        r_direcao=(int) (rand() / (double) RAND_MAX * (3));

        plataformas[i].velocidade=vel;
        plataformas[i].tamanho=tam;
        plataformas[i].direcao=r_direcao;
        plataformas[i].posicao.y=ry;

        if(r_direcao==ESQUERDA)
        {
            plataformas[i].posicao.x=rx;
        }
        else if(r_direcao==DIREITA)
        {
            plataformas[i].posicao.x=rx;
        }
        else if(r_direcao==CIMA)
        {
            plataformas[i].posicao.x = rx;
            plataformas[i].posicao.y = 24;
        }

        i++;
    }


}
/*a fun��o recebe uma lista de plataformas e a quantidade m�xima de plataformas
e coloca na tela */
int coloca_plataformas_na_tela(PLATAFORMA lista[], int quantidade, int cont)
{
    int i;
    for(i=0; i<quantidade; i++)
    {
        if((cont%3) < lista[i].velocidade)
            movimenta_plataforma(&lista[i]);
        textbackground(GREEN);
    }
    return;
}
/*se alguma plataforma n�o estiver nos limites do mapa do jogo, o jogo far� com que elas apare�am no mapa seguindo a probabilidade descrita */
void atualiza_plataformas(PLATAFORMA plataformas[], int nr_plataformas)
{
    int i,r,ry,rx;
    //srand(time(0));
    for(i=0; i<nr_plataformas; i++)
    {
        if(!plataforma_aparecendo(plataformas[i])) // Se a plataforma n�o est� aparecendo
        {
            r= (int) 1 +(rand() / (double) RAND_MAX * (10));
            if(r<=4)
            {
                ry=(int) Y_MIN +(rand() / (double) RAND_MAX * (Y_MAX));
                plataformas[i].posicao.y=ry;
                plataformas[i].posicao.x=79;
                plataformas[i].direcao=ESQUERDA;
            }
            else if(r<=8)
            {
                ry=(int) Y_MIN +(rand() / (double) RAND_MAX * (Y_MAX));
                plataformas[i].posicao.y=ry;
                plataformas[i].posicao.x=2-plataformas[i].tamanho;
                plataformas[i].direcao=DIREITA;
            }
            else
            {
                rx=(int) X_MIN +(rand() / (double) RAND_MAX * (X_MAX));
                plataformas[i].posicao.x=rx;
                plataformas[i].posicao.y=24;
                plataformas[i].direcao=CIMA;
            }
        }
    }
}
/* a fun��o recebe a plataforma e atrav�s da sua posi��o verifica se ela se encontra ou n�o no mapa*/
int plataforma_aparecendo(PLATAFORMA plataforma)
{
    switch(plataforma.direcao)
    {
    case CIMA:
        if(plataforma.posicao.y<2)
            return 0;
        return 1;
        break;
    case DIREITA:
        if(plataforma.posicao.x>=80)
            return 0;
        return 1;
        break;
    case ESQUERDA:
        if(plataforma.posicao.x+plataforma.tamanho<=1)
            return 0;
        return 1;
        break;
    }
}

/*a fun��o do menu do jogo*/
void colocamenu()
{
    // plota_nome();
    gotoxy(40,7);
    textcolor(YELLOW);
    puts("SMILE BARS");
    textcolor(WHITE);
    opcao();
    seletor();
}

/*printa as op��es do jogo na tela*/
void opcao()
{
    int i, j;
    int  x = 34;
    int  y = 10;
    int opy = 17;
    textcolor(WHITE);
    textbackground(BLACK);
    gotoxy(x, y);
    printf("INICIAR NOVO JOGO\n");
    y = y+3;
    gotoxy(x, y);
    printf("TOP FIVE RECORDES\n");
    y = y+3;
    gotoxy(x, y);
    printf("SAIR DO JOGO\n");
}

/*seletor do menu para operar sobre as op��es do jogo*/
void seletor()
{
    int x = 27;
    int y = 10;
    char c;
    int seletor;
    textbackground(BLACK);
    gotoxy (x, y);
    textcolor(RED);
    printf(">>>>");
    gotoxy (x+26, y);
    printf("<<<<");
    do
    {
        c = getch();
        switch(c)
        {
        case ACIMA:
        {
            if(y > 10)
            {
                gotoxy(x, y);
                printf("    ");
                gotoxy (x+26, y);
                printf("    ");
                y = y-3;
                gotoxy (x, y);
                printf(">>>>");
                gotoxy (x+26, y);
                printf("<<<<");
            }
        }
        break;
        case ABAIXO:
        {
            if(y < 16)
            {
                gotoxy(x, y);
                printf("    ");
                gotoxy (x+26, y);
                printf("    ");
                y = y+3;
                gotoxy (x, y);
                printf(">>>>");
                gotoxy (x+26, y);
                printf("<<<<");
            }
            break;
        }
        case VK_RETURN:
        {
            clrscr();
            switch(y)
            {
            case INICIAR:
                gotoxy(40,12);
                laco_jogo();

                break;

            case RECORDE:
                gotoxy(40,12);
                le_arquivo();
                break;
            case SAIR:
                clrscr();
                gotoxy(40,12);
                puts("FIM DE JOGO!");
                Sleep(1000);
                exit(0);
                break;
            }
            break;
        }
        }
    }
    while(c != VK_RETURN);
}
//MENU 2
void colocamenu_2()
{
    // plota_nome();
    opcao_2();
    seletor_2();
}
/*printa as op��es do jogo na tela*/
void opcao_2()
{
    int i, j;
    int  x = 34;
    int  y = 10;
    int opy = 17;
    textcolor(WHITE);
    textbackground(BLACK);
    gotoxy(x, y);
    printf("MENU INICIAL\n");
    y = y+3;
    gotoxy(x, y);
    printf("SAIR DO JOGO\n");
}

/*seletor do menu para operar sobre as op��es do jogo*/
void seletor_2()
{
    int x = 27;
    int y = 10;
    char c;
    int seletor;
    textbackground(BLACK);
    gotoxy (x, y);
    textcolor(RED);
    printf(">>>>");
    gotoxy (x+26, y);
    printf("<<<<");
    do
    {
        c = getch();
        switch(c)
        {
        case ACIMA:
        {
            if(y > 10)
            {
                gotoxy(x, y);
                printf("    ");
                gotoxy (x+26, y);
                printf("    ");
                y = y-3;
                gotoxy (x, y);
                printf(">>>>");
                gotoxy (x+26, y);
                printf("<<<<");
            }
        }
        break;
        case ABAIXO:
        {
            if(y < 13)
            {
                gotoxy(x, y);
                printf("    ");
                gotoxy (x+26, y);
                printf("    ");
                y = y+3;
                gotoxy (x, y);
                printf(">>>>");
                gotoxy (x+26, y);
                printf("<<<<");
            }
            break;
        }
        case VK_RETURN:
        {
            clrscr();
            switch(y)
            {
            case INICIAR:
                gotoxy(40,12);
                colocamenu();
                break;
            case 13:
                clrscr();
                gotoxy(40,12);
                puts("FIM DE JOGO!");
                Sleep(1000);
                exit(0);
                break;
            }
            break;
        }
        }
    }
    while(c != VK_RETURN);
}

/*anima��o da entrada do jogo no menu*/
void plota_barra()
{
    int i,j;

    for(i=1; i<6; i++)
    {
        Sleep(200);
        if(!(i%2))
        {
            textbackground(RED);
            for(j=80; j>=1; j--)
            {
                putchxy(j,i,VK_SPACE);
                Sleep(2);
            }
        }
        else
        {
            textbackground(BLUE);
            for(j=1; j<=80; j++)
            {
                putchxy(j,i,VK_SPACE);
                Sleep(2);
            }
        }
    }
}

/*a fun��o imprime dados de controle do jogo na parte superior do mapa
para que o jogador tenha conhecimento dos seus pontos, se est� ou n�o sob o efeito de algum
poder e quantas vidas lhe restam. O nome do jogador tamb�m � colocado no topo do mapa por esta fun��o*/
void imprime_hud(char nome[], int pontos, int poder, int vida)
{
    textbackground(BLUE);
    textcolor(WHITE);
    gotoxy(5,1);
    printf("Nome: %s", nome);
    printf("\tPontos:%5d", pontos);
    printf("\t\t Poder:%2d", poder);
    printf("\t Vidas:%2d", vida);
    gotoxy(2,25);
    textbackground(RED);
    textcolor(WHITE);
    switch (poder)
    {
    case NULO:
        printf("               ");
        break;
    case LEVITACAO:
        printf("%15s", "Levitacao");
        break;
    case SUPER_PULO:
        printf("%15s", "Super Pulo");
        break;
    case INVENCIBILIDADE:
        printf("%15s", "Invencivel");
        break;
    case PARAR_TEMPO:
        printf("%15s", "PAROU O TEMPO");
        break;
    case BAIXA_GRAVIDADE:
        printf("%15s", "Esta na lua!");
        break;
    case VIDA_EXTRA:
        printf("%15s", "Ganhou vida");
        break;
    case PONTOS_EXTRAS:
        printf("%15s", "ganhou pontos");
        break;
    }
}
/*a fun��o cria um arquivo com os records */
void cria_recordes()
{
    FILE *arq;
    JOGADOR lista[5];
    char url[20];
    char naux[20];
    int i,j,r;
    JOGADOR aux;

    strcpy(url,"recordes.bin");

    srand(time(0));
    clrscr();

    for(i=0; i<TAM_RECORDES; i++)
    {
        textcolor(YELLOW);
        puts("\nEntre com o nome do jogador: ");
        gets(lista[i].nome);
        r=(int) (rand() / (double) RAND_MAX * (PONTUACAO_MAXIMA)); // pontua��o maxima
        lista[i].pontuacao = r;
        printf("%d \n", r);
    }

    ordena_recordes(lista, TAM_RECORDES);

    arq=fopen(url,"w+b");

    if(!arq)
    {
        puts ("Erro na abertura do arquivo!");
    }
    else
    {
        for(i=0; i<TAM_RECORDES; i++)
        {
            fwrite(&lista[i],sizeof(JOGADOR),1,arq);
        }
        puts("Arquivo Gravado com sucesso!");
        fclose(arq);
        fflush(stdin);
    }
}

/*a fun��o recebe um array de jogadores para serem ordenados e um n�mero inteiro */
void ordena_recordes(JOGADOR lista[], int tam)
{
    int i,j;
    JOGADOR aux;

    for(i=tam-1; i >= 1; i--)
    {
        for(j=0; j < i ; j++)
        {
            if(lista[j].pontuacao<lista[j+1].pontuacao)
            {
                aux = lista[j]; //ORDENA PONTOS
                lista[j] = lista[j+1];
                lista[j+1] = aux;
            }
        }
    }
}

/*a fun��o n�o recebe argumentos, ela serve para abrir o arquivo*/
void le_arquivo()
{
    JOGADOR lista[TAM_RECORDES];
    FILE *arq;
    char url[20];
    int i,j,r;

    strcpy(url,"recordes.bin");

    arq=fopen(url,"rb");

    if(!arq)
    {
        cria_recordes();
    }
    else
    {
        gotoxy(36,9);
        textcolor(YELLOW);
        puts("   Recordes: \n");
        for(i=0; i<TAM_RECORDES; i++)
        {
            fread(&lista[i],sizeof(JOGADOR),1,arq);
            printf("\t\t\t  � \t %d - %5d ...%10s �\n",i+1, lista[i].pontuacao, lista[i].nome);

        }
        printf("\n");
        fclose(arq);
        aguarda_space();
        fflush(stdin);
        clrscr();
        colocamenu_2();
    }

}

/*a fun��o recebe um jogador e insere a sua posi��o na lista de records se este estiver entre os 5 maiores*/
void insere_recorde(JOGADOR jogador)
{
    FILE *arq;
    JOGADOR bufer[TAM_RECORDES], aux, bufer_aux[TAM_RECORDES+1], player; //BUFFER [6]
    char url[20];
    int i,j, lugar=6;

    strcpy(url,"recordes.bin");

    arq=fopen(url,"rb"); // ABRINDO COMO LEITURA

    if(!arq)
    {
        puts("Erro ao abrir o arquivo!");
    }
    else
    {
        i=0;
        while(i<TAM_RECORDES) // Enquanto n�o for o fim do arquivo
        {
            fread(&player,sizeof(JOGADOR),1,arq);
            bufer[i]=player;
            bufer_aux[i]=bufer[i];

            if(jogador.pontuacao>player.pontuacao)
                lugar-=1;
            i++;
        }
        fclose(arq);

        if(jogador.pontuacao>bufer_aux[i-1].pontuacao)
        {
            arq=fopen(url,"w+b");
            bufer_aux[TAM_RECORDES] = jogador;

            ordena_recordes(bufer_aux,TAM_RECORDES+1);

            for(i=0; i <TAM_RECORDES; i++)
            {
                player=bufer_aux[i];
                fwrite(&player,sizeof(JOGADOR),1,arq);
            }

            fclose(arq);
            clrscr();
            gotoxy(25,18);
            textcolor(WHITE);
            printf("Jogador ficou no %2d lugar do ranking!", lugar);
            Sleep(1000);
        }
        else
        {
            clrscr();
            gotoxy(22,18);
            textcolor(WHITE);
            puts("Pontos insufucientes para entrar no top 5!");
            Sleep(1000);
        }
        fclose(arq);


    }
}
/*a fun��o inicializa todos os atributos do jogador*/
void inicializa_jogador(JOGADOR *jogador)
{
    jogador->ch=2; //smile, caractere 2 da tabela ASCII
    jogador->alcance=6;
    jogador->cor=YELLOW;
    jogador->posicao.x=40;
    jogador->posicao.y=12;
    jogador->tempo_poder=0;
    jogador->pulando=0;
    jogador->poder=NULO;
    jogador->pulou2=0;
    jogador->pulo2_ativo=0; // desabilita o segundo �ulo do jogador.
}

/* a fun��o insere na tela um menu para que o jogador insira o seu nome*/
void le_nome(JOGADOR *jogador)
{
    clrscr();
    gotoxy(36,8);
    puts("Seu nome:");
    gotoxy(36,15);
    gets(jogador->nome);
    fflush(stdin);
}
/*a fun��o a seguir recebe um ponteiro do tipo JOGADOR para que possam ser alterados os atributos do jogador como indicadores de movimenta��o e a sua posi��o
e recebe o inteiro "sob_plat" que representa uma vari�vel booleana respons�vel por dizer se o personagem se encontra ou n�o sobre uma plataforma, al�m da vari�vel alcance
auxiliar que recebe o alcance inicial do jogador para que ap�s as altera��es o alcance seja recuperado. A vari�vel tecla recebe o valor da tecla lida para gerar o movimento*/
void gerencia_controle(JOGADOR *jogador, int sob_plat, int alcance_aux, int tecla, int cont)
{
        tecla=le_tecla();
        switch(tecla)
        {
        case CIMA:

       if((!jogador->pulando && sob_plat)||( jogador->poder==INVENCIBILIDADE && jogador->posicao.y == Y_MAX+1) || jogador->pulo2_ativo)
            {
                if(jogador->pulo2_ativo)
                {
                    jogador->pulou2=1;
                    jogador->alcance=alcance_aux/2;
                    jogador_pula(jogador);
                    Beep(660,100);
                }
                else
                {
                    jogador_pula(jogador);
                    jogador->pulo2_ativo = 1;
                    jogador->pulou2=0;
                    Beep(660,100);
                }

            }

            break;
        case ESQUERDA:
            if(jogador->pulando || jogador->caindo)
            {
                if(cont%3<2) // metade da velocidade
                    move_jogador(jogador, ESQUERDA);
            }
            else
                move_jogador(jogador, ESQUERDA);
            break;
        case DIREITA:
            if(jogador->pulando || jogador->caindo)
            {
                if(cont%3<2) // metade da velocidade
                    move_jogador(jogador, DIREITA);
            }
            else
                move_jogador(jogador, DIREITA);
            break;
        }
}
/* A fun��o recebe um ponteiro para jogador, uma vari�vel booleana que indica se o jogador est� ou n�o sobre uma plataforma e o cont do jogo
   para gerenciar o movimento do jogador quando ele n�o est� sobre uma plataforma */
void gerencia_movimentacao_no_ar(JOGADOR *jogador, int sob_plat, int cont)
{
    if(!sob_plat)
    {
        if(jogador->poder!=LEVITACAO)
        {
            if(!jogador->pulando)
                jogador_cai(jogador, cont);
            else
            {
                jogador_pula(jogador);
            }
        }
        else
            jogador->alcance=0;

    }
    if(sob_plat && jogador->pulando)
        jogador_pula(jogador);
}

/*a fun��o consome:
um ponteiro do tipo JOGADOR(porque a posi��o do jogador deve ser atualizada a cada movimento sofrido);
um array de plataformas( para checar se h� alguma plataforma sobre a qual o jogador se encontra);
dois n�meros do tipo inteiro( o primeiro serve para restaurar o alcance inicial do pulo do jogador e o segundo
                              se refere ao n�mero de plataformas plotaveis).*/
void gerencia_movimentacao(JOGADOR *jogador, PLATAFORMA plataformas[], int alcance_aux, int nr_plataformas, int cont)
{
    int num; // representa a plataforma que o jogador estava em cima
    int tecla; //representa a tecla lida do teclado
    int sob_plat;

    sob_plat=jogador_em_alguma_plataforma(*jogador, plataformas,nr_plataformas, &num);

    if(kbhit())
    {
        gerencia_controle(jogador, sob_plat, alcance_aux, tecla, cont);
    }
    else if(jogador->pulando || jogador->caindo) // quando alguma tecla n�o est� pressionada, seta as condi��es do segundo pulo
    {

        jogador->pulou2=0;
        jogador->pulo2_ativo=1;

    }

    if( jogador->poder==INVENCIBILIDADE && jogador->posicao.y == Y_MAX+1) // faz com que a �ltima linha simule uma grande "barra".
    {
        jogador->caindo=0;
        jogador->alcance=alcance_aux;
    }
    if((sob_plat && !jogador->pulando)&& jogador->poder != PARAR_TEMPO ) // faz as gerencias de quando o jogador est� sobre uma plataforma
    {
        jogador->pulou2=0;
        jogador->pulo2_ativo=0;
        jogador->caindo=0;
        jogador->alcance=alcance_aux;
        if((jogador->poder != LEVITACAO) || (plataformas[num].direcao == CIMA))
        {
            if(cont%3 < plataformas[num].velocidade) // Faz com que o jogador ande na mesma velocidade que est� em cima.
            {
                move_jogador(jogador, plataformas[num].direcao);
            }
        }
    }
    else if((sob_plat && !jogador->pulando)&& jogador->poder == PARAR_TEMPO) // Gerencia movimenta��o quando o jogador est� sobre plataforma e o tempo parado
    {
        jogador->caindo=0;
        jogador->alcance=alcance_aux;
    }
    if(jogador->poder != PARAR_TEMPO) // Caso n�o seja o poder de parar o tempo
    {
        coloca_plataformas_na_tela(plataformas,nr_plataformas, cont);
        atualiza_plataformas(plataformas,nr_plataformas);
    }
    else
    {

        desenha_plataformas(plataformas, nr_plataformas);

    }
    sob_plat=jogador_em_alguma_plataforma(*jogador, plataformas,nr_plataformas, &num);
    gerencia_movimentacao_no_ar(jogador, sob_plat, cont);

}

/* a fun��o aguarda que o jogador aperte espa�o para continuar o jogo */
void aguarda_space()
{
    int i=1;
    fflush(stdin);

    do
    {
        if(i>10)
            i=1;

        Sleep(75);
        textcolor(i);
        gotoxy(1,20);
        puts("\t\t\t  Pressione SPACE para continuar...");

        i++;
    }
    while(le_tecla()!=0);
}

/*a fun��o a seguir � a respons�vel por manter o jogo funcionando at� que o jogador gaste todas as suas vidas*/
void laco_jogo()
{

    int vida = 3, alcance_aux, parado,continua=1,i,sob_plat=0;
    int nr_plataformas,num=99;
    int tecla=69, cont;
    double tempo=0,tempo_total_segundos=0;
    clock_t inicio, fim;

    COORDENADA coord,coord1;
    PLATAFORMA plataforma;
    JOGADOR jogador;
    PODER poder;
    PLATAFORMA plataformas[25];

    jogador.pontuacao=0;
    le_nome(&jogador);
    clrscr();
    fflush(stdin);
    jogador.morreu=0;

    do
    {
        cont=0;
        nr_plataformas=0;
        fflush(stdin);
        desenha_mapa();
        carrega_plataformas(plataformas, &nr_plataformas);
        parado=1;
        inicializa_jogador(&jogador);
        limpa_poder(&poder);
        gotoxy(36,8);
        alcance_aux=jogador.alcance;

        // salva tempo ao iniciar
        while(jogador_vivo(jogador))
        {
            cont++;
            if(parado)
            {
                imprime_hud(jogador.nome, jogador.pontuacao,jogador.poder,vida);
                fflush(stdin);
                if(le_tecla()==0)
                {
                    parado=0;
                    inicio = clock();
                }
                else
                {
                    coloca_plataformas_na_tela(plataformas,nr_plataformas,cont);
                    atualiza_plataformas(plataformas,nr_plataformas);


                    textbackground(BLACK);
                    textcolor(WHITE);
                    putchxy(jogador.posicao.x,jogador.posicao.y,jogador.ch);

                    if(jogador.morreu)
                    {
                        gotoxy(25,25);
                        textbackground(RED);
                        textcolor(WHITE);
                        printf("Jogador morreu, aperte SPACE para continuar");
                    }
                    else
                    {
                        textbackground(RED);
                        textcolor(WHITE);
                        gotoxy(30,25);
                        printf("Aperte SPACE para jogar");
                    }

                }
                Sleep(50);
            }
            else
            {
                gotoxy(25,25);
                textbackground(RED);
                printf("                                                       ");


                Sleep(100);
                fim=clock();
                tempo=(int)((fim-inicio) / CLOCKS_PER_SEC);
                if(tempo >= 1)
                {
                    inicio = clock();
                    if(poder.visivel)
                    {
                        poder.tempo_visivel+=1;
                    }
                    else
                    {
                        poder.tempo_invisivel+=1;
                    }
                    if(jogador.poder)
                    {
                        jogador.tempo_poder+=1;
                    }
                }
                if(jogador.poder == PONTOS_EXTRAS)
                {
                    jogador.pontuacao += 60;
                    jogador.poder = 0;
                }
                //
                jogador.pontuacao += tempo;

                imprime_hud(jogador.nome, jogador.pontuacao, jogador.poder,vida);

                gerencia_movimentacao(&jogador, plataformas, alcance_aux, nr_plataformas, cont);
                verifica_poder(&jogador,&poder);

                if (poder.visivel)
                {
                    plota_poder(poder);
                }
                //  else if(jogador.pontuacao%150 == 0)
                else if(poder.tempo_invisivel == TEMPO_PARA_APARECER_PODER && !poder.tempo_visivel) // 10 segundos
                {
                    inicializa_poder(&poder);
                    plota_poder(poder);
                }
                if(poder.visivel && poder.tempo_visivel>15) // PARA TIRAR O PODER DA TELA SE O JOGADOR N�O PEG�-LO NO TEMPO
                {
                    limpa_poder(&poder);
                }
                if(jogador.tempo_poder>TEMPO_JOGADOR_COM_PODER) // CASO O JOGADOR ESTIVER COM O PODER POR MAIS DE 10 SEGUNDOS
                {
                    jogador.poder=0; // TIRA O PODER DO JOGADOR
                    jogador.tempo_poder=0;  // SETA O TEMPO PARA ZERO
                    poder.tempo_invisivel=0;
                }

                if(!jogador_vivo(jogador))
                {
                    vida--;
                    jogador.morreu=1;
                }


                if(jogador.poder == VIDA_EXTRA)
                {
                    vida++;
                    jogador.poder = 0;
                }
            }

        }
        textbackground(BLACK);
        clrscr();
        gotoxy(36,12);
        clrscr();
    }

    while(vida>0);

    insere_recorde(jogador);
    le_arquivo();

    getchar();
}
/*a fun��o inicializa_poder recebe um ponteiro de poder, ela �
    respons�vel pela randomicidade da cor e da posi��o do poder e consequentemente a fun��o do poder*/
void inicializa_poder(PODER *poder)
{
    int  r, efeito, tempoauxiliar;
    r=(int) 2 +(rand()/(double) RAND_MAX * (X_MAX - 2));
    poder->posicao.x = r;
    r = (int) 2 + (rand() / (double) RAND_MAX * (Y_MAX - 1));
    poder->posicao.y = r;

    r = (int) 1+ (rand() / (double) RAND_MAX * (VALOR_PODER + 1));
    poder->efeito = r; // ALTERADO

    poder->tempo_invisivel = 0;
    poder->tempo_visivel = 0;
    poder->visivel=1;
}
/*a fun��o seguinte apenas desenha um espa�o na cor de um poder na tela para que o jogador possa pegar*/
void plota_poder(PODER poder)
{
    textbackground(poder.efeito);
    if(poder.visivel)
    {
        putchxy (poder.posicao.x, poder.posicao.y, ' ');
    }
}
/*a fun��o verifica_poder verifica se existe um poder visivel na tela, pois quando h� um poder visivel � possivel
que o jogador o pegue e ele precisa desaparecer depois de determinado tempo*/

int verifica_poder(JOGADOR *jogador, PODER *poder)
{
    if(jogador->posicao.x == poder->posicao.x && jogador->posicao.y == poder->posicao.y)
    {
        jogador->poder=poder->efeito;
        poder->visivel=0;
        poder->tempo_visivel=0;
        return 1;
    }
    return 0;

}
/*fun��o recebe um ponteiro de PODER para modificar a visibilidade do poder, a fun��o serve para apagar um poder na tela*/
void limpa_poder(PODER *poder)
{
    poder->efeito = NULO;
    poder->visivel = 0;
    poder ->tempo_invisivel = 0;
    poder ->tempo_visivel = 0;
    textbackground(0);
    cputsxy(poder->posicao.x, poder->posicao.y, " ");
}

