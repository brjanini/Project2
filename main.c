#include <stdio.h>
#include <stdlib.h>
#include <string.h>    // memcpy -> essencial para movimenta��o
#include <conio.h>     // _kbhit, _getch
#include <windows.h>   // cls -> respons�vel por limpar a tela

//Declara��o das vari�veis globais:
char dino[10][10];     // Matriz que armazena o dinossauro
char tree[8][5];       // Matriz que armazena a �rvore
char bird[3][5];       // Matriz que armazena o p�ssaro
char tela[28][70];     // Matriz que ser� printada -> interface do jogo
FILE *fp;              // Ponteiro que ser� utilizado na leitura de arquivos
int score,s;           // Pontua��o -> s++ a cada limepeza de tela -> score=s/5
char tecla;            // Vari�vel que armazena o caracter da tecla pressionada pelo jogador -> espa�o=pulo
int obst;              // Verifica qual o obst�culo lan�ado -> [1] �rvore / [2] P�ssaro

//Leitura de arquivos bin�rios -> dinossauro, �rvore e p�sssaro
void load_dino(){
    int i;
    fp = fopen("dinossauro","rb");
    for(i=0; i<10; i++){
        fread(dino[i],sizeof(char),10,fp);
    }
    fclose(fp);
}

void load_tree(){
    int i;
    fp = fopen("tree","rb");
    for(i=0; i<8; i++){
        fread(tree[i],sizeof(char),5,fp);
    }
    fclose(fp);
}

void load_bird(){
    int i;
    fp = fopen("bird","rb");
    for(i=0; i<3; i++){
        fread(bird[i],sizeof(char),5,fp);
    }
    fclose(fp);
}

//"Limpeza" da matriz tela + constru��o do ch�o
void load_tela(){
    int i,j;
    for(i=0; i<28; i++){
        for(j=0; j<70; j++){
            tela[i][j] = ' ';
            tela[23][j] = 219;   // C�digo do ret�ngulo na tabela ascii -> Ch�o
        }
        tela[i][68] = '\n';     // Como a matriz tela ser� printada com fputs, � essencial que haja a quebra de linha e a sinaliza��o de final
        tela[i][69] = '\0';     // de string
    }
}

void printa_score(){
    score = s/5;
    printf("%d",score);
}



//Verifica highscore
void highscore(){
    int testscore;
    fp = fopen("highscore.txt","r");
    fscanf(fp,"%d", &testscore);
    fclose(fp);
    if(score>testscore){
        fp = fopen("highscore.txt","w");
        fprintf(fp,"%d",score);
        fclose(fp);
        printf("Score: %d\nPARABENS! VOCE CONQUISTOU A MAIOR PONTUACAO DO JOGO!\n\n",score);
    }else{
         printf("Score: %d\nHighscore: %d\n\n",score,testscore);
     }
}

//Sub-rotina respons�vel pela limpeza da tela
void cls(HANDLE hConsole)
{
    COORD coordScreen = { 0, 0 };    // home for the cursor
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Get the number of character cells in the current buffer.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Fill the entire screen with blanks.
    if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
                                    (TCHAR)' ',      // Character to write to the buffer
                                    dwConSize,       // Number of cells to write
                                    coordScreen,     // Coordinates of first cell
                                    &cCharsWritten)) // Receive number of characters written
    {
        return;
    }

    // Get the current text attribute.
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
    {
        return;
    }

    // Set the buffer's attributes accordingly.
    if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
                                    csbi.wAttributes, // Character attributes to use
                                    dwConSize,        // Number of cells to set attribute
                                    coordScreen,      // Coordinates of first cell
                                    &cCharsWritten))  // Receive number of characters written
    {
        return;
    }

    // Put the cursor at its home coordinates.
    SetConsoleCursorPosition(hConsole, coordScreen);
}

void finaliza(int x){
    int i,vel;
    for(i=0; i<24; i++){
        fputs(tela[i],stdout);
    }
    printa_score();
    if(score<300) vel=20;
    if(score>299 && score<500) vel=15;
    if(score>499 && score<700) vel=10;
    if(score>700 && score<1000) vel=5;
    if(score>1000) vel=1;

    if(x==1){
        Sleep(2000);
        cls(GetStdHandle(STD_OUTPUT_HANDLE));
        highscore();
        exit(1);
            }
    Sleep(vel);
    cls(GetStdHandle(STD_OUTPUT_HANDLE));
    s++;
}

int main()
{
    int i,j;
    int p = 0;       // Verifica se o dinossauro est� pulando(p=1) ou n�o(p=0)
    int obst = 0;    // Verifica qual � o obstaculo(obst=1 �rvore / obst=2 p�ssaro) e se n�o h� nenhum(obst=0)
    int dy = 0;      // Deslocamento vertical do dinossuaro
    int ny = 0;      // Conta os incrementos a dy e verifica se o pulo j� se encerrou
    int dxa = 0;     // Deslocamento horizontal da �rvore
    int dxb = 0;     // Deslocamento horizontal do p�ssaro
    int gameover = 0;


    load_bird(); load_dino(); load_tree(); load_tela();  //Chamada das sub-rotinas para armazenamento dos elementos nas matrizes e limpeza da tela

    // O jogo, mantido dentro de um while, � dividido em duas partes -> com e sem pulo
    while(1){
        // Sem pulo
        if(p==0){
            load_tela();
            for(i=13,j=0; i<23; i++,j++){
                memcpy(&tela[i][8],&dino[j][0],10);
            }
            // �rvore
            if(obst==1){
                for(i=15,j=0; i<23; i++,j++){
                    if(dxa<5){
                        memcpy(&tela[i][68-dxa],&tree[j][0],dxa);
                    }else if(dxa<69){
                         if(dxa>49){
                             if(tela[i][17]!=' '){
                                gameover=1;
                             }
                         }
                         memcpy(&tela[i][68-dxa],&tree[j][0],5);
                     }else{
                          memcpy(&tela[i][0],&tree[j][dxa-68],73-dxa);
                      }
                }
                    if(dxa>71){
                        dxa=0;
                        obst=0;
                    }else{
                         dxa++;
                     }
            }
            // P�ssaro
            if(obst==2){
                for(i=7,j=0; i<10; i++,j++){
                    if(dxb<5){
                        memcpy(&tela[i][68-dxb],&bird[j][0],dxb);
                    }else if(dxb>4 && dxb<69){
                         memcpy(&tela[i][68-dxb],&bird[j][0],5);
                     }else{
                          memcpy(&tela[i][0],&bird[j][dxb-68],73-dxb);
                      }
                }
                    if(dxb>71){
                        dxb=0;
                        obst=0;
                    }else{
                         dxb++;
                     }
            }

            finaliza(gameover);

            // Sem obst�culo
            if(obst==0){
                obst = rand()%3;
                continue;
            }
            if(_kbhit()){
                tecla = _getch();
                if(tecla==' '){
                    p = 1;
                    continue;
                }
            }
        }

        //Com pulo
        if(p==1){
            load_tela();
            ny++;
            if(ny<13){
                dy++;
            }
            if(ny>17){
                dy--;
            }
            for(i=13-dy,j=0; i<23-dy; i++,j++){
                memcpy(&tela[i][8],&dino[j][0],10);
            }
            // �rvore
            if(obst==1){
                for(i=15,j=0; i<23; i++,j++){
                    if(dxa<5){
                        memcpy(&tela[i][68-dxa],&tree[j][0],dxa);
                    }else if(dxa<69){
                         if(dxa<63 && (tela[i][67-dxa]!=' ' || tela[14][70-dxa]!=' ')){
                            gameover = 1;
                         }
                         memcpy(&tela[i][68-dxa],&tree[j][0],5);
                     }else{
                          memcpy(&tela[i][0],&tree[j][dxa-68],73-dxa);
                      }
                }
                    if(dxa>71){
                        dxa=0;
                        obst=0;
                    }else{
                         dxa++;
                     }
            }
            // P�ssaro
            if(obst==2){
                for(i=7,j=0; i<10; i++,j++){
                    if(dxb<5){
                        memcpy(&tela[i][68-dxb],&bird[j][0],dxb);
                    }else if(dxb>4 && dxb<69){
                         if(dxb<64 && (tela[7][68-dxb]!=' ' || tela[6][69-dxb]!=' ' || tela[8][67-dxb]!=' ' || tela[9][69-dxb]!=' ' || tela[9][71-dxb]!=' ')){
                            gameover = 1;
                         }
                         memcpy(&tela[i][68-dxb],&bird[j][0],5);
                     }else{
                          memcpy(&tela[i][0],&bird[j][dxb-88],73-dxb);
                      }
                }
                    if(dxb>71){
                        dxb=0;
                        obst=0;
                    }else{
                         dxb++;
                     }
            }

            finaliza(gameover);

            if(ny==29){
                p=0;
                dy=0;
                ny=0;
            }
            // Sem obst�culo
            if(obst==0){
                obst = rand()%3;
                continue;
            }
        }
        }

    return 0;
}
