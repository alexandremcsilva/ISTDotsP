/*        ISTDots by Alexandre Manuel Cardoso Silva (AMCS), number 90004

          This works is destined to the Projeto Intermédio de Programação of the MEEC curse.
          The program realized after this commentary join points with the same color, 1 case away (can't connect diagonally)
          and in case a square is made (ideally it should've been all geometric formulas) or a rectangle it blows all the
          points with the same color and even if there are points inside of the square/retangle, they should disapear aswell.

          This work was realised during 1st April until 15th April, we're the last couple of days we're used to improve some
          parts of the code.

          Note that i couldn't get right the info in the screen when a shuffle is, so i just make the shuffle function but i cannot
          show that its doing in screen.
*/


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SQR(a)      (a)*(a)
#define MAX(a,b)    (((a)>(b))?(a):(b))
#define M_PI 3.14159265
#define STRING_SIZE 100       // max size for some strings
#define TABLE_SIZE 600       // main game space size
#define LEFT_BAR_SIZE 150     // left white bar size
#define WINDOW_POSX 250       // initial position of the window: x
#define WINDOW_POSY 10       // initial position of the window: y
#define SQUARE_SEPARATOR 8    // square separator in px
#define BOARD_SIZE_PER 0.7f   // board size in % wrt to table size
#define MAX_BOARD_POS 15      // maximum size of the board
#define MAX_COLORS 5
#define MARGIN 5
#define MAX_NAME 30 // Tamanho random para a string do nome do jogador (playername)


// declaration of the functions related to graphical issues
void InitEverything(int , int , TTF_Font **, TTF_Font **, SDL_Surface **, SDL_Window ** , SDL_Renderer ** );
void InitSDL();
void InitFont();
SDL_Window* CreateWindow(int , int );
SDL_Renderer* CreateRenderer(int , int , SDL_Window *);
int RenderText(int, int, const char *, TTF_Font *, SDL_Color *, SDL_Renderer *);
int RenderLogo(int, int, SDL_Surface *, SDL_Renderer *);
int RenderTable(int, int, int [], TTF_Font *, SDL_Surface **, SDL_Renderer *, int [][MAX_BOARD_POS], int [][MAX_BOARD_POS]);
void ProcessMouseEvent(int , int , int [], int , int *, int * );
void RenderPoints(int [][MAX_BOARD_POS], int, int, int [], int, SDL_Renderer *);
void RenderStats( SDL_Renderer *, TTF_Font *, int, int, int [] );
void filledCircleRGBA(SDL_Renderer * , int , int , int , int , int , int );




//my functions
void ReadParameters(int*, int*, int*, int [MAX_BOARD_POS], int [MAX_BOARD_POS], int*, int*, char []); // Função que lê os parâmetros
void GenerateTable (int [][MAX_BOARD_POS], int , int , int );    //Função que gera um novo tabuleiro
void TableMatrix(int [][MAX_BOARD_POS], int , int , int* , int* ); //Função que gera o tabuleiro moves para registo de jogadas
int VerifyConditionsMotion (int , int [][MAX_BOARD_POS] , int, int, int [][MAX_BOARD_POS], int, int, int*, int*, int*, int*); // Função que verifica se a jogada é possivel, se recuamos ou passamos num ponto ja marcado (quadrado);
void ReplaceColor ( int , int , int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int, int, int [], int*, int*, int [][MAX_BOARD_POS], int [], int); //FUnção que apos largado o rato substitui o devido por -2
void VectorColorDown (int , int , int [][MAX_BOARD_POS], int vector_column[MAX_BOARD_POS]); //Função que retira os vetores colunas necessarios para ficar com as cores por ordem de baixo para cima
void GetNewColor (int , int , int [][MAX_BOARD_POS], int, int, int ); //Esta função substitui os -2 por valores random.
void SquareBlow (int , int, int [][MAX_BOARD_POS], int, int, int, int, int, int*, int*, int[], int[], int, int[][MAX_BOARD_POS]); // Função que rebenta quadrados e retangulos
void GamingPossibilities(int , int , int [][MAX_BOARD_POS], int*, int, int,int [], int*, int*,int [], int, int[], int); //Analisa após cada M.B.Up se existem combinaçoes possiveis para a proxima jogada, bem como se perdeu ou ganhou o jogo
void ShuffleGame(int , int , int [][MAX_BOARD_POS]); //Caso a condiçao atras de não haver jogadas possiveis se verifique esta funçao dá shuffle no jogo
void VectorTransformation(int []); //simpes função que tranforma um vetor inicializado a 0 para -1 (util para a escrita no final)
void Showthetxt(int , int [], int [],char []); //função que realiza a escrita do texto
void RenderMessage(SDL_Renderer *, TTF_Font *, int , int, int*, int); //funçao que renderiza as imagens de ter ganho ou perdido.
int clean_stdin(); //le e "esquece" caracteres until ate ler algo caracteristico do pedido
void LinePoints(SDL_Renderer *,int [][MAX_BOARD_POS],int [],int, int[], int, int, int,int [][MAX_BOARD_POS],int [2][250]); //gera a linha que une pontos
void UndoIt(int , int*, int*, int [], int [], int [][MAX_BOARD_POS], int [][MAX_BOARD_POS], int , int ); //opçao avançada que faz undo


// definition of some strings: they cannot be changed when the program is executed !
const char myName[] = "Alexandre Silva";
const char myNumber[] = "IST190004";
const int colors[3][MAX_COLORS] = {{246, 52, 255, 186, 124},{255, 211, 0, 0, 39},{37, 21, 0, 93, 137}};

/**
 * main function: entry point of the program
 * only to invoke other functions !
 */
int main( void )
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    TTF_Font *serif = NULL;
    TTF_Font *sans = NULL; //nova fonte
    SDL_Surface *imgs[2];
    SDL_Event event;
    int delay = 300;
    int quit = 0;
    int width = (TABLE_SIZE + LEFT_BAR_SIZE);
    int height = TABLE_SIZE;
    int square_size_px = 0, board_size_px[2] = {0};
    int board_pos_x = 0, board_pos_y = 0;
    int board[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}};
    int boardbefore[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}}; // guarda a board anterior para o undo
    int moves[MAX_BOARD_POS][MAX_BOARD_POS] = {{0}}; // guarda os moves
    int making_move = 0;
    int pt_x = 0, pt_y = 0;
    char playername[30] = {0}; //irá guardar o nome do jogador
    int colorquantity = 0; //guarda a quantidade de cores a serem jogadas
    int achievementpoints[MAX_BOARD_POS] = {0}; //vetor que guarda quantas bolas temos que rebentar por cor para ganhar (objetivos)
    int achievementpointscopy[MAX_BOARD_POS] = {0}; //faz uma copia para caso o utilizador clique no n
    int achievementpointsbefore[MAX_BOARD_POS] = {0};
    int roundsquantity = 0; //quantidade de rondas mácima que o utilizador quer jogar para atingir os objetivos
    int roundsquantitycopy = 0;//copia o valor anterior de modo ao undo saber que valor copiar para la caso aconteça
    int roundsquantitybefore = 0; //copia do anterior para calculo de numero de rondas em que, caso ganhe, aconteceu e serve de copia para caso seja ativado o n
    int count = 0; // faz o count do numero de bolas por onde passei
    int ex_x = 0, ex_y = 0; //guarda a posição anterior onde passei(util caso ande para tras )
    int vector_column[MAX_BOARD_POS] = {0}; // vetor usado na funçao vectordown que guarda por coluna a ordem depois de rebentadas as bolas
    int flagquadrado = 0; //flag que sinaliza se a funçao squareblow é usada ou nao
    int cor = -1; //guarda a cor caso haja quadrado para percorrer o ciclo que marque todas as bolas com essa cor
    int minx = 16; //minimo.x do quadrado
    int miny = 16; //minimo.y do quadrado
    int maxx = -1; //maximo.x do quadrado
    int maxy = -1; //maximo.y do quadrado     estas 4 servem para determinar entre que valores, caso haja algo dentro do quadrado, eu tenho de rebentar
    int any_move = 0; // variavel que quando a 0, indica que nao tenho mais opçoes e como tal terei que dar shuffle
    int victorloose[100] = {0}; //vetor que guarda informaçoes por jogo de derrota ou vitoria
    int win_game = 0; //caso esta variavel fique a 1, significa que o utilizador ganhou o jogo e como tal ativa certas funçoes
    int loose_game = 0; //caso esta variavel fique a 1, significa que o utilizador perdeu o jogo e como tal ativa certas funçoes
    int round_number = 0; // guarda o numero do jogo, ou seja, no final diz-nos quantos jogos o jogador fez
    int playsifwin[100] = {0}; //caso ganhe, este vetor guarda o numero de jogadas que fez quando ganhou
    int playable = 0; //variavel que deixa ou nao jogar consoante as caracteristicas do jogo (antes de clicar em n, quando se perde e aparece imagem ou quando se ganha)
    int line_coor[4]={0}; //variavel para a funçao das linhas
    int move_vector[2][250] = {{0}}; //vetor bidimensional que guarda as posiçoes por onde se passa de modo a gerar a linha

    ReadParameters(&board_pos_x, &board_pos_y, &colorquantity, achievementpoints, achievementpointscopy, &roundsquantity, &roundsquantitycopy, playername);

    GenerateTable (board,colorquantity, board_pos_x, board_pos_y);

    VectorTransformation (victorloose);


    // initialize graphics
    InitEverything(width, height, &serif, &sans, imgs, &window, &renderer);


    while( quit == 0 )
    {
        // while there's events to handle
        while( SDL_PollEvent( &event ) )
        {
            if( event.type == SDL_QUIT )
            {
                if (loose_game == 0 && win_game == 0) victorloose[round_number] = 0; //condiçoes para dar quit e contar como derrot
                quit = 1;

            }
            else if ( event.type == SDL_KEYDOWN )
            {
                switch ( event.key.keysym.sym )
                {
                    case SDLK_n:

                         if (loose_game == 1) victorloose[round_number] = 0; //quando se pressa n, caso se tenha perdido ele vai contar como uma derrota
                         if (win_game == 1) victorloose[round_number] = 1; //caso se tenha ganho ele vai contar como uma vitoria
                         if (loose_game == 0 && win_game == 0 &&round_number > 0) victorloose[round_number] = 0; //caso já se tenha jogado e clicado no n ele conta como um derrota
                         round_number++; //incrementa o nº de jogada
                         win_game = 0; //volta a por as variaveis para um jogo inicial e copia os parametros do jogo de volta
                         loose_game = 0;
                         roundsquantity = roundsquantitycopy; //guarda valor para undo (ambos)
                         for (int v = 0; v < colorquantity; v++){
                              achievementpoints[v] = achievementpointscopy[v];
                         }
                         playable = 1; //diz que se pode jogar (ou seja executar operaçoes com o rato)

                         GenerateTable (board,colorquantity, board_pos_x, board_pos_y);

                    break;

                    case SDLK_q:

                         victorloose[round_number] = 0; //caso se clique em q ele conta como derrota e sai
                         quit = 1;

                    break;

                    case SDLK_u: //ocpional

                    UndoIt(colorquantity, &roundsquantity, &roundsquantitybefore, achievementpoints, achievementpointsbefore, board, boardbefore, board_pos_x, board_pos_y);
                    //repoe os valores da ronda anterior com esta funçao
                        break;

                    default:
                        break;
                }
            }

            else if ( event.type == SDL_MOUSEBUTTONDOWN )
            {
                if (playable == 1){
                     ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);

                     ex_x = 0;
                     ex_y = 0;

                     TableMatrix (moves, pt_x ,pt_y , &making_move, &count);

                     ex_x = pt_x; //guarda a posiçao anterior
                     ex_y = pt_y;
                }
            }

            else if ( event.type == SDL_MOUSEBUTTONUP )
            {
                 if (playable == 1){
                      ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);
                      making_move = 0; //diz que ja n tou a jogar e volta a por o count com o valor inicial


                      count = 1;
                      if (flagquadrado == 0){
                           //caso isto se verifique ele apenas subsitui a cor dos selecionado
                           ReplaceColor (board_pos_x, board_pos_y, board, moves, pt_x, pt_y, achievementpoints, &roundsquantity, &roundsquantitybefore, boardbefore,achievementpointsbefore, colorquantity);
                      }

                      if (flagquadrado == 1){
                           //senão ele substitui todas essas cores e eventualmente as dentro dela por -2
                           SquareBlow (board_pos_y, board_pos_x, board, cor, minx, miny, maxx, maxy, &roundsquantity, &roundsquantitybefore, achievementpoints, achievementpointsbefore, colorquantity, boardbefore);
                      }


                      VectorColorDown (board_pos_x, board_pos_y, board, vector_column); // guarda num vetor as posiçoes a preencher no novo board e mete -1 nos espaços vazios em cima

                      GetNewColor ( board_pos_x, board_pos_y, board, colorquantity, cor, flagquadrado); //repoe os quadrados sem cores com novas

                      GamingPossibilities (board_pos_x, board_pos_y, board, &any_move, roundsquantity, colorquantity, achievementpoints, &win_game, &loose_game,
                                           victorloose, round_number, playsifwin, roundsquantitycopy ); //verifica se ganhamos perdemos ou nao ha jogadas

                      if(any_move == 0){
                          // dá shuffle
                           ShuffleGame( board_pos_y, board_pos_x, board);
                      }

                      flagquadrado = 0; //volta a por as variaveis ao estado inicial
                      cor = -1;
                      minx = 16;
                      miny = 16;
                      maxx = -1;
                      maxy = -1;
                      any_move = 0;
                 }
           }



            else if ( event.type == SDL_MOUSEMOTION )
            {
                if (playable == 1){
                     ProcessMouseEvent(event.button.x, event.button.y, board_size_px, square_size_px, &pt_x, &pt_y);

                     if (VerifyConditionsMotion (making_move, board, pt_x, pt_y, moves, ex_x, ex_y, &minx, &miny, &maxx, &maxy) == 1){ //andar pa frente
                          moves[pt_x][pt_y] = ++count; //marca por onde passo
                          ex_x=pt_x; //guarda o anterior
                          ex_y=pt_y;
                     }

                    else if (VerifyConditionsMotion (making_move, board, pt_x, pt_y, moves, ex_x, ex_y, &minx, &miny, &maxx, &maxy) == 2){ //andar pa tras
                         moves[ex_x][ex_y] = 0; //caso se volte pa tras vai buscar os valores anteriores.
                         flagquadrado = 0;
                         ex_x = pt_x;
                         ex_y = pt_y;
                         count--;
                    }

                    else if (VerifyConditionsMotion (making_move, board, pt_x, pt_y, moves, ex_x, ex_y, &minx, &miny, &maxx, &maxy) == 3){ //forma geometrica detetada

                         moves[pt_x][pt_y] = ++count;
                         ex_x=pt_x;                    // caso se faça quadrado faz o mesmo que o primeiro if mas aciona a flag quadrado para executar o squareblow
                         ex_y=pt_y;
                         flagquadrado = 1;
                         cor = board[pt_x][pt_y];
                    }
               }
          }
     }


        // render game table
        square_size_px = RenderTable( board_pos_x, board_pos_y, board_size_px, serif, imgs, renderer, board, moves);
        // render board
        RenderPoints(board, board_pos_x, board_pos_y, board_size_px, square_size_px, renderer);
        // render in the screen all changes above
        if (count>1) LinePoints(renderer, moves,board_size_px, square_size_px, line_coor, board_pos_x, board_pos_y, count, board, move_vector); //renderiza a linha
        RenderStats( renderer, serif, colorquantity, roundsquantity, achievementpoints); //renderiza os stats

        RenderMessage(renderer, sans, loose_game, win_game, &playable, round_number); //renderiza as mensagem de ganhar e perder e de shuffle

        SDL_RenderPresent(renderer);
        // add a delay
        SDL_Delay( delay );
   }



   Showthetxt(round_number, victorloose, playsifwin, playername); //executa o ficheiro quando o jogo acaba!

    // free memory allocated for images and textures and closes everything including fonts
   TTF_CloseFont(serif);
   TTF_CloseFont(sans);
   SDL_FreeSurface(imgs[0]);
   SDL_FreeSurface(imgs[1]);
   SDL_DestroyRenderer(renderer);
   SDL_DestroyWindow(window);
   SDL_Quit();
   return EXIT_SUCCESS;
}

/*
 * ProcessMouseEvent: gets the square pos based on the click positions !
 * \param _mouse_pos_x position of the click on pixel coordinates
 * \param _mouse_pos_y position of the click on pixel coordinates
 * \param _board_size_px size of the board !
 * \param _square_size_px size of each square
 * \param _pt_x square nr
 * \param _pt_y square nr
 */
void ProcessMouseEvent(int _mouse_pos_x, int _mouse_pos_y, int _board_size_px[], int _square_size_px, int *_pt_x, int *_pt_y )
{
    int sq_x = 0, sq_y = 0;
    int circleX = 0, circleY = 0, circleR = 0, dist = 0;
    // corner of the board
    int x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    int y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // verify if valid cordinates
    if (_mouse_pos_x < x_corner || _mouse_pos_y < y_corner || _mouse_pos_x > (x_corner + _board_size_px[0])
        || _mouse_pos_y > (y_corner + _board_size_px[1]) )
    {
        *_pt_x = -1;
        *_pt_y = -1;
        return;
    }

    // computes the square where the mouse position is

    sq_x = ( _mouse_pos_x - x_corner ) / (_square_size_px + SQUARE_SEPARATOR);
    sq_y = ( _mouse_pos_y - y_corner ) / (_square_size_px + SQUARE_SEPARATOR);

    circleX = x_corner + (sq_x + 1)*SQUARE_SEPARATOR + sq_x*(_square_size_px)+(_square_size_px>>1);
    circleY = y_corner + (sq_y + 1)*SQUARE_SEPARATOR + sq_y*(_square_size_px)+(_square_size_px>>1);
    circleR = (int)(_square_size_px*0.4f);

    dist =  (int)floor(sqrt( SQR(_mouse_pos_x - circleX) + SQR(_mouse_pos_y - circleY)));                //conjunto de calculos que em vez

    if ( dist < circleR ){                                                                           //de detetar o quadrado deteta o circulo
      *_pt_x = sq_x;                                                                       //Calcula o centro da bola (quadrado) e caso passe
      *_pt_y = sq_y;                                                                 //o raio calculado, ele dá como se tivesse fora!
    }
    else {
      *_pt_x = -1;
      *_pt_y = -1;
    }
}

/**
 * RenderPoints: renders the board
 * \param _board 2D array with integers representing board colors
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _square_size_px size of each square
 * \param _board_size_px size of the board
 * \param _renderer renderer to handle all rendering in a window
 */
void RenderPoints(int _board[][MAX_BOARD_POS], int _board_pos_x, int _board_pos_y,
        int _board_size_px[], int _square_size_px, SDL_Renderer *_renderer )
{
    int clr, x_corner, y_corner, circleX, circleY, circleR;

    // corner of the board
    x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
    y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

    // renders the squares where the dots will appear
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
                // define the size and copy the image to display
                circleX = x_corner + (i+1)*SQUARE_SEPARATOR + i*(_square_size_px)+(_square_size_px>>1);
                circleY = y_corner + (j+1)*SQUARE_SEPARATOR + j*(_square_size_px)+(_square_size_px>>1);
                circleR = (int)(_square_size_px*0.4f);
                // draw a circle
                clr = _board[i][j];
                filledCircleRGBA(_renderer, circleX, circleY, circleR, colors[0][clr], colors[1][clr], colors[2][clr]);
        }
    }
}

/**
 * filledCircleRGBA: renders a filled circle
 * \param _circleX x pos
 * \param _circleY y pos
 * \param _circleR radius
 * \param _r red
 * \param _g gree
 * \param _b blue
 */
void filledCircleRGBA(SDL_Renderer * _renderer, int _circleX, int _circleY, int _circleR, int _r, int _g, int _b)
{
    int off_x = 0;
    int off_y = 0;
    float degree = 0.0;
    float step = M_PI / (_circleR*8);

    SDL_SetRenderDrawColor(_renderer, _r, _g, _b, 255);

    while (_circleR > 0)
    {
        for (degree = 0.0; degree < M_PI/2; degree+=step)
        {
            off_x = (int)(_circleR * cos(degree));
            off_y = (int)(_circleR * sin(degree));
            SDL_RenderDrawPoint(_renderer, _circleX+off_x, _circleY+off_y);
            SDL_RenderDrawPoint(_renderer, _circleX-off_y, _circleY+off_x);
            SDL_RenderDrawPoint(_renderer, _circleX-off_x, _circleY-off_y);
            SDL_RenderDrawPoint(_renderer, _circleX+off_y, _circleY-off_x);
        }
        _circleR--;
    }
}

//desenho da linha que liga os pontos
//void RenderLine(SDL_Renderer* _renderer)


void LinePoints(SDL_Renderer * _renderer,int moves[][MAX_BOARD_POS], int _board_size_px[],
               int _square_size_px, int line_coor[4], int board_pos_x, int board_pos_y, int count, int board[][MAX_BOARD_POS], int move_vector[2][250]){


     int x_corner, y_corner;


     int i=0, j=0, k=0,p=0, q=0, clr=0;

          for( i=0; i<count; i++){
               //procura pelas posiçoes de move na matriz
               for(j=0; j<board_pos_x; j++){
                    for( k=0; k<board_pos_y; k++){
                         if (moves[j][k]==i+1){
                              move_vector[0][i]=j; //guarda por onde tem que passar a linha neste vetor
                              move_vector[1][i]=k;
                         }
                    }
               }
          }

     x_corner = (TABLE_SIZE - _board_size_px[0]) >> 1;
     y_corner = (TABLE_SIZE - _board_size_px[1] - 15);

     clr=board[move_vector[0][0]][move_vector[1][0]]; //define a cor da linha
     SDL_SetRenderDrawColor( _renderer, colors[0][clr], colors[1][clr], colors[2][clr], 255 ); //renderiza a cor da linha

     for (p=0; p<(count-1); p++){
          for (q=(-_square_size_px*0.1); q<=(_square_size_px*0.1); q++){             //Desnha varias linhas de modo a ficar com espessura
               line_coor[0] = x_corner + (move_vector[0][p]+1)*SQUARE_SEPARATOR + move_vector[0][p]*(_square_size_px)+(_square_size_px>>1)+q;
               line_coor[1] = y_corner + (move_vector[1][p]+1)*SQUARE_SEPARATOR + move_vector[1][p]*(_square_size_px)+(_square_size_px>>1)+q;
               line_coor[2] = x_corner + (move_vector[0][p+1]+1)*SQUARE_SEPARATOR + move_vector[0][p+1]*(_square_size_px)+(_square_size_px>>1)+q;
               line_coor[3] = y_corner + (move_vector[1][p+1]+1)*SQUARE_SEPARATOR + move_vector[1][p+1]*(_square_size_px)+(_square_size_px>>1)+q;
               SDL_RenderDrawLine(_renderer, line_coor[0],line_coor[1],line_coor[2],line_coor[3]); //renderiza as linhas
          }
     }
}





/*
 * Shows some information about the game:
 * - Goals of the game
 * - Number of moves
 * \param _renderer renderer to handle all rendering in a window
 * \param _font font to display the scores
 * \param _goals goals of the game
 * \param _ncolors number of colors
 * \param _moves number of moves remaining
 */


void RenderStats( SDL_Renderer *_renderer, TTF_Font *_font, int colorquantity, int roundsquantity, int achievementpoints[MAX_BOARD_POS])
{
     int i = 0;
     int j = 0;

  SDL_Color light = {205, 193, 181};
  SDL_Color dark = {120, 110, 102};
  SDL_Rect rect = {30, 95, 80, 50};



  char str[STRING_SIZE] = {0};
  char pointsofgame[MAX_COLORS] = {0};

  SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a); //define a cor do retangulo

  SDL_RenderFillRect(_renderer, &rect); //desenhar i retangulo fundo do texto

  sprintf(str, "Plays: %d", roundsquantity);         //converto o inteiro moves para string

  RenderText(50, 100, str, _font, &dark, _renderer ); //renderiza o testo com o jogo

  for (i = 160; i <= 80+(80*(colorquantity)); i=i+80){
       SDL_Rect rect = {i,100,60,45};
       SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a);
       SDL_RenderFillRect(_renderer, &rect);

       sprintf(pointsofgame, "%d", achievementpoints[j]);                                       //renderiza os quadrados com a info do jogo, sempre o das jogadas e

       if (achievementpoints[j] < 0) RenderText(i + 40, 105, "0", _font, &dark, _renderer );   //vai fazendo o numero de cores dependendo de quantas se jogam.

       else RenderText (i + 40, 105, pointsofgame, _font, &dark, _renderer);

       filledCircleRGBA (_renderer, i + 20,120,10, colors[0][j], colors[1][j], colors[2][j]);

       j++;
  }
}

  //TTF_SizeText( _font, str, &text_area.w, &text_area.h );   //para saber o tamanho do texto em strings
  //SDL_RenderFillRect(_renderer, &text_area);   //desenhar o retanguo (fundo do texyo)
  //RenderText(text_area.x, text_area.y, str, _font, &dark, _renderer );                   //desenhar o texto


/*
 * RenderTable: Draws the table where the game will be played, namely:
 * -  some texture for the background
 * -  the right part with the IST logo and the student name and number
 * -  the grid for game board with squares and seperator lines
 * \param _board_pos_x number of positions in the board (x axis)
 * \param _board_pos_y number of positions in the board (y axis)
 * \param _board_size_px size of the board
 * \param _font font used to render the text
 * \param _img surfaces with the table background and IST logo (already loaded)
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderTable( int _board_pos_x, int _board_pos_y, int _board_size_px[],
        TTF_Font *_font, SDL_Surface *_img[], SDL_Renderer* _renderer, int _board[][MAX_BOARD_POS], int moves[][MAX_BOARD_POS])
{
    SDL_Color black = { 0, 0, 0 }; // black
    SDL_Color light = { 255, 255, 255 };
    SDL_Color dark = { 255, 255, 255 };
    SDL_Texture *table_texture;
    SDL_Rect tableSrc, tableDest, board, board_square;
    int height, board_size, square_size_px, max_pos /*,clr=0*/;

    // set color of renderer to some color
    SDL_SetRenderDrawColor( _renderer, 255, 255, 255, 255 );

    // clear the window
    SDL_RenderClear( _renderer );

    tableDest.x = tableSrc.x = 0;
    tableDest.y = tableSrc.y = 0;
    tableSrc.w = _img[0]->w;
    tableSrc.h = _img[0]->h;
    tableDest.w = TABLE_SIZE;
    tableDest.h = TABLE_SIZE;

    // draws the table texture
    table_texture = SDL_CreateTextureFromSurface(_renderer, _img[0]);
    SDL_RenderCopy(_renderer, table_texture, &tableSrc, &tableDest);

    // render the IST Logo
    height = RenderLogo(TABLE_SIZE, 0, _img[1], _renderer);

    // render the student name
    height += RenderText(TABLE_SIZE+3*MARGIN, height, myName, _font, &black, _renderer);

    // this renders the student number
    RenderText(TABLE_SIZE+3*MARGIN, height, myNumber, _font, &black, _renderer);

    // compute and adjust the size of the table and squares
    max_pos = MAX(_board_pos_x, _board_pos_y);
    board_size = (int)(BOARD_SIZE_PER*TABLE_SIZE);
    square_size_px = (board_size - (max_pos+1)*SQUARE_SEPARATOR) / max_pos;
    _board_size_px[0] = _board_pos_x*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;
    _board_size_px[1] = _board_pos_y*(square_size_px+SQUARE_SEPARATOR)+SQUARE_SEPARATOR;

    // renders the entire board background
    SDL_SetRenderDrawColor(_renderer, dark.r, dark.g, dark.b, dark.a );
    board.x = (TABLE_SIZE - _board_size_px[0]) >> 1;
    board.y = (TABLE_SIZE - _board_size_px[1] - 15);
    board.w = _board_size_px[0];
    board.h = _board_size_px[1];
    SDL_RenderFillRect(_renderer, &board);

    // renders the squares where the numbers will appear
    SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );

    // iterate over all squares
    for ( int i = 0; i < _board_pos_x; i++ )
    {
        for ( int j = 0; j < _board_pos_y; j++ )
        {
            board_square.x = board.x + (i+1)*SQUARE_SEPARATOR + i*square_size_px;
            board_square.y = board.y + (j+1)*SQUARE_SEPARATOR + j*square_size_px;
            board_square.w = square_size_px;
            board_square.h = square_size_px;

            SDL_RenderFillRect(_renderer, &board_square);
            SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a );
        }
    }

    // destroy everything
    SDL_DestroyTexture(table_texture);
    // return for later use
    return square_size_px;
}

/**
 * RenderLogo function: Renders the IST logo on the app window
 * \param x X coordinate of the Logo
 * \param y Y coordinate of the Logo
 * \param _logoIST surface with the IST logo image to render
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderLogo(int x, int y, SDL_Surface *_logoIST, SDL_Renderer* _renderer)
{
    SDL_Texture *text_IST;
    SDL_Rect boardPos;

    // space occupied by the logo
    boardPos.x = x;
    boardPos.y = y;
    boardPos.w = _logoIST->w;
    boardPos.h = _logoIST->h;

    // render it
    text_IST = SDL_CreateTextureFromSurface(_renderer, _logoIST);
    SDL_RenderCopy(_renderer, text_IST, NULL, &boardPos);

    // destroy associated texture !
    SDL_DestroyTexture(text_IST);
    return _logoIST->h;
}

/**
 * RenderText function: Renders some text on a position inside the app window
 * \param x X coordinate of the text
 * \param y Y coordinate of the text
 * \param text string with the text to be written
 * \param _font TTF font used to render the text
 * \param _color color of the text
 * \param _renderer renderer to handle all rendering in a window
 */
int RenderText(int x, int y, const char *text, TTF_Font *_font, SDL_Color *_color, SDL_Renderer* _renderer)
{
    SDL_Surface *text_surface;
    SDL_Texture *text_texture;
    SDL_Rect solidRect;

    solidRect.x = x;
    solidRect.y = y;
    // create a surface from the string text with a predefined font
    text_surface = TTF_RenderText_Blended(_font,text,*_color);
    if(!text_surface)
    {
        printf("TTF_RenderText_Blended: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    // create texture
    text_texture = SDL_CreateTextureFromSurface(_renderer, text_surface);
    // obtain size
    SDL_QueryTexture( text_texture, NULL, NULL, &solidRect.w, &solidRect.h );
    // render it !
    SDL_RenderCopy(_renderer, text_texture, NULL, &solidRect);
    // clear memory
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
    return solidRect.h;
}

/**
 * InitEverything: Initializes the SDL2 library and all graphical components: font, window, renderer
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _font font that will be used to render the text
 * \param _img surface to be created with the table background and IST logo
 * \param _window represents the window of the application
 * \param _renderer renderer to handle all rendering in a window
 */
void InitEverything(int width, int height, TTF_Font **_font, TTF_Font **_font2, SDL_Surface *_img[], SDL_Window** _window, SDL_Renderer** _renderer)
{
    InitSDL();
    InitFont();
    *_window = CreateWindow(width, height);
    *_renderer = CreateRenderer(width, height, *_window);

    // load the table texture
    _img[0] = IMG_Load("table_texture.png");
    if (_img[0] == NULL)
    {
        printf("Unable to load image: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // load IST logo
    _img[1] = SDL_LoadBMP("ist_logo.bmp");
    if (_img[1] == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    // this opens (loads) a font file and sets a size
    *_font = TTF_OpenFont("FreeSerif.ttf", 16);
    if(!*_font)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    *_font2 = TTF_OpenFont("OpenSans.ttf", 35); //inicializaçao da outra fonte
    if(!*_font2)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitSDL: Initializes the SDL2 graphic library
 */
void InitSDL()
{
    // init SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        printf(" Failed to initialize SDL : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * InitFont: Initializes the SDL2_ttf font library
 */
void InitFont()
{
    // Init font library
    if(TTF_Init()==-1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

/**
 * CreateWindow: Creates a window for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \return pointer to the window created
 */
SDL_Window* CreateWindow(int width, int height)
{
    SDL_Window *window;
    // init window
    window = SDL_CreateWindow( "ISTDots - 90004", WINDOW_POSX, WINDOW_POSY, width, height, 0 );
    // check for error !
    if ( window == NULL )
    {
        printf("Failed to create window : %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    return window;
}

/**
 * CreateRenderer: Creates a renderer for the application
 * \param width width in px of the window
 * \param height height in px of the window
 * \param _window represents the window for which the renderer is associated
 * \return pointer to the renderer created
 */
SDL_Renderer* CreateRenderer(int width, int height, SDL_Window *_window)
{
    SDL_Renderer *renderer;
    // init renderer
    renderer = SDL_CreateRenderer( _window, -1, 0 );

    if ( renderer == NULL )
    {
        printf("Failed to create renderer : %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // set size of renderer to the same as window
    SDL_RenderSetLogicalSize( renderer, width, height );

    return renderer;
}



void ReadParameters(int* board_pos_x , int* board_pos_y, int* colorquantity, int achievementpoints[MAX_BOARD_POS], int achievementpointscopy[MAX_BOARD_POS],
      int* roundsquantity, int *roundsquantitycopy, char playername[30])
{

     char ca;

     //Introduçao de dados rrelativos ao jogo.
     do
     {
          printf("Introduza o tamanho do tabuleiro (linhas): ");
     }    while (((scanf("%d%c", board_pos_y, &ca)!=2 || ca!='\n') && clean_stdin()) || (*board_pos_y < 5 || *board_pos_y > 15));

     do
     {
          printf("Introduza o tamanho do tabuleiro (colunas): ");
     }    while (((scanf("%d%c", board_pos_x, &ca)!=2 || ca!='\n') && clean_stdin()) || (*board_pos_x < 5 || *board_pos_x > 15));




    printf("Introduza o seu nickname: ");
          fgets(playername, MAX_NAME, stdin);
          while (strlen(playername) > 9){
               printf("Introduza o seu nickname de novo: ");
               fgets(playername, MAX_NAME, stdin);
          }



     do
     {
          printf("Introduza número de cores: ");
     }    while (((scanf("%d%c", colorquantity, &ca)!=2 || ca!='\n') && clean_stdin()) || *colorquantity<0 || *colorquantity>5);



    for ( int i = 0; i < *colorquantity; i++){
         do
         {
              printf("Objetivos para a %da cor: ", i+1);
         }    while (((scanf("%d%c", &achievementpoints[i], &ca)!=2 || ca!='\n') && clean_stdin()) || achievementpoints[i]<1 || achievementpoints[i]>99);
    }

        for(int k = 0; k < *colorquantity; k++){
             achievementpointscopy[k] = achievementpoints[k];  //copiar para guardar o inicialmente caso se clique em n
        }

        do
        {
             printf("Numero de rondas em que quer completar os objetivos: ");
        }    while (((scanf("%d%c", roundsquantity, &ca)!=2 || ca!='\n') && clean_stdin()) || *roundsquantity<1 || *roundsquantity>99);

        *roundsquantitycopy = *roundsquantity;                  //copiar para guardar o inicialmente caso se clique em n

}


void GenerateTable (int board[][MAX_BOARD_POS], int colorquantity, int board_pos_x, int board_pos_y) //gera o tabuleiro do jogo
{
      int i = 0, j = 0;

      srand(time(NULL));

      for ( i = 0; i < board_pos_y; i++ ){
          for (j = 0; j < board_pos_x; j++){

               board[j][i] = rand() % colorquantity; //gera o tabuleiro com valores das cores entre 0 e o que escolhemos nos parametros iniciais
          }
     }
}


void TableMatrix (int moves[][MAX_BOARD_POS], int pt_x, int pt_y, int *making_move, int *count)
{
     if (pt_x==-1) return;

     int i = 0, j = 0;

     for ( i = 0; i < MAX_BOARD_POS; i++)              //caso se clique em -1 ele n faz nada.
          for (j = 0; j < MAX_BOARD_POS; j++)          //voltar a por a matriz moves a 0
               moves[i][j]= 0;

               *count = 1;                             //começa a contar a matriz moves
               if (moves[pt_x][pt_y] == 0)
                    moves [pt_x][pt_y] = *count;

                    *making_move = 1;   //indica que estamos a iniciar o movimento e a partir de agora é a contar o selecionado

}



int VerifyConditionsMotion (int making_move, int board[][MAX_BOARD_POS],
                      int pt_x, int pt_y, int moves[][MAX_BOARD_POS], int ex_x, int ex_y, int *minx, int *miny, int *maxx, int *maxy)
{
     if (making_move == 1) {
          if (moves[pt_x][pt_y]==0){
               if (board[ex_x][ex_y]==board[pt_x][pt_y]){
                    if ((ex_x==pt_x+1&&ex_y==pt_y)||(ex_x==pt_x-1&&ex_y==pt_y)||(ex_x==pt_x&&ex_y==pt_y+1)||(ex_x==pt_x&&ex_y==pt_y-1)){

                         if (pt_x < *minx){       //verifica todas as condiçoes para ser uma jogada possivel e
                              *minx = pt_x;       //vai alterando maximo e minimo caso seja necessario. não conta quadrado
                         }

                         if (pt_y < *miny){
                              *miny = pt_y;
                         }

                         if (pt_x > *maxx){
                              *maxx = pt_x;
                         }

                         if (pt_y > *maxy){
                              *maxy = pt_y;
                         }
                              return 1;
                         }
                         else return 0;
                    }
                    else return 0;
              }
              if(moves[pt_x][pt_y] == moves[ex_x][ex_y] - 1){ //caso isto aconteca o utilizador voltou pa tras e vai por os valores anteriores.

                   return 2;
              }
              if(moves[pt_x][pt_y] > 0 && moves[pt_x][pt_y] != moves[ex_x][ex_y] - 1){
                   if ((ex_x==pt_x+1&&ex_y==pt_y)||(ex_x==pt_x-1&&ex_y==pt_y)||(ex_x==pt_x&&ex_y==pt_y+1)||(ex_x==pt_x&&ex_y==pt_y-1)){
                        return 3;       //Caso andemos para algum sitio possivel e onde ja temos passado ele retorna 3 para ativar a flag quadrado
                   }
                   else return 0;
              }
              else return 0;
         }
         else return 0;
}

void ReplaceColor ( int board_pos_x, int board_pos_y, int board[][MAX_BOARD_POS], int moves[][MAX_BOARD_POS], int pt_x, int pt_y,
      int achievementpoints[MAX_BOARD_POS], int *roundsquantity, int *roundsquantitybefore, int boardbefore[][MAX_BOARD_POS], int achievementpointsbefore[MAX_BOARD_POS], int colorquantity)
{
     int i = 0;
     int j = 0;

     for (i = 0; i < board_pos_y; i++){
          for (j = 0; j < board_pos_x; j++){
               boardbefore[i][j] = board[i][j];  //guarda a matriz caso tenha que fazer undo.
          }
     }

     if ( moves[pt_x][pt_y] > 1){
          *roundsquantitybefore = *roundsquantity;
          *roundsquantity = *roundsquantity - 1;       //decrementa o numero de jogadas, e troca todos os valores que deve mser selecionados
                                                       //por -2, de modo funçao a seguir n contar com eles
          for ( i = 0; i < colorquantity; i++){
               achievementpointsbefore[i]=achievementpoints[i];
          }


          for ( i = 0; i < board_pos_y; i++){          //por -2, de modo funçao a seguir n contar com eles
               for (j = 0; j < board_pos_x; j++){
                    if (moves[i][j] > 0 ){
                         achievementpoints[board[i][j]] = achievementpoints[board[i][j]] - 1;
                         board[i][j] = -2;
                    }
               }
          }
     }
}

void VectorColorDown (int board_pos_x, int board_pos_y, int board[][MAX_BOARD_POS], int vector_column[MAX_BOARD_POS])
{
     int count = 0;
     int a = 0;
     int i = 0;
     int j = 0;

     for ( i = 0; i < board_pos_x; i++){
          count = 0;                                                  //Vai guardando vetores coluna de modo a baixar os valores q ficaram no board

          for (j = board_pos_y - 1 ; j >= 0  ; j--){
               if ( board[i][j] >= 0 ){
                    vector_column[count] = board[i][j];
                    count++;
               }
          }
          a = 0;

          for ( j = board_pos_y - 1; (j > board_pos_y - count - 1); j--){
               board[i][j] = vector_column[a];
               a++;                                                                       //vai substituindo as posiçoes que ficam em cima de -2 pa -1
          }                                                                                    //(n necessario)
          for ( j = board_pos_y - count - 1; j >= 0; j--){
               board[i][j] = - 1;
          }
     }
}


void GetNewColor (int board_pos_x, int board_pos_y, int board[][MAX_BOARD_POS], int colorquantity, int cor, int flagquadrado)
{
     int i = 0;
     int j = 0;
     srand(time(NULL));

     for ( i = 0; i < board_pos_y; i++){
          for ( j = 0; j < board_pos_x; j++){
               if( board[i][j] == -1){                           //a todas as posiçoes com -1 em cima da random color.
                    board[i][j] = rand() % colorquantity;
                    if (flagquadrado==1){
                         while(1){
                              if (board[i][j]!=cor) break;
                              board[i][j] = rand() % colorquantity;
                         }
                    }
               }
          }
     }

}

void SquareBlow (int board_pos_y, int board_pos_x, int board[][MAX_BOARD_POS], int cor, int minx, int miny,int maxx, int maxy, int *roundsquantity,
      int *roundsquantitybefore, int achievementpoints[MAX_BOARD_POS], int achievementpointsbefore[], int colorquantity, int boardbefore[][MAX_BOARD_POS])
{
     int i = 0;
     int j = 0;
     *roundsquantitybefore = *roundsquantity;
     *roundsquantity = *roundsquantity - 1;
     for ( i = 0; i < colorquantity; i++){   //guarda a quantidade de rondas anterior/decrementa a atual/ guarda o valor das cores que falta juntar ate final da jogada anterior
          achievementpointsbefore[i]=achievementpoints[i];
     }

     for (i = 0; i < board_pos_y; i++){
          for (j = 0; j < board_pos_x; j++){
               boardbefore[i][j] = board[i][j];  //guarda a matriz caso tenha que fazer undo.
          }
     }


     for (i = 0; i < board_pos_y; i++){
          for (j = 0; j < board_pos_x; j++){                                         //Neste caso ele cai iterar por todas as posiçoes e todos os que tiverem
               if ( (i > minx) && (i < maxx) && (j > miny) && (j < maxy)){                // a mesma cor ou que estejam dentro (iteraçao seguinte) do quadrado ele substitui por -2
                    achievementpoints[board[i][j]] = achievementpoints[board[i][j]] - 1;
                    board[i][j] = -2;
               }
               else;
          }
     }

     i = 0;
     j = 0;

     for (i = 0; i < board_pos_y; i++){
          for (j = 0; j < board_pos_x; j++){
               if (board[j][i] == cor){                //faz o objetivo da funçao, rebentar todas as bolas com a cor (quadrado retangulo e algumas fig.geom. incluidas.
                    achievementpoints[board[j][i]] = achievementpoints[board[j][i]] - 1;
                    board[j][i] = -2;
               }
          }
     }
}


void GamingPossibilities(int board_pos_x, int board_pos_y, int board[][MAX_BOARD_POS], int *any_move,int roundsquantity,
     int colorquantity, int achievementpoints[MAX_BOARD_POS], int *win_game, int *loose_game, int victorloose[30], int round_number, int playsifwin[100], int roundsquantitycopy)
{

     int i = 0;
     int j = 0;
     int k = 0;


     if (roundsquantity == 0){                    //testa se perdi o jogo e caso perca ativa a variavel correspondente
          for (k = 0; k < colorquantity; k++){
               if (achievementpoints[k] > 0){
                    *loose_game = 1;
               }
          }
     }


     if (roundsquantity > 0 && (achievementpoints[0] < 1 && achievementpoints[1] < 1 && achievementpoints[2] < 1 &&achievementpoints[3] < 1 && achievementpoints[4] < 1) ){
          *win_game = 1;
          victorloose[round_number] = 1;                                        //testa se ganhei o jogo e caso ganhe ativa a variavel correspondente
          playsifwin[round_number] = roundsquantitycopy - roundsquantity;       // e conta as jogadas em que ganhei!


     }


     for ( i = 0; i < board_pos_y; i++){
		for ( j = 0; j < board_pos_x; j++){
			if ((i==board_pos_y-1) && (j==board_pos_x-1)) break;
			else if (j==board_pos_x-1){
				if (board[j][i]==board[j][i+1]){                        //itera pelas posiçoes e ve se existem jogadas possiveos
					*any_move=1;                                      //caso nao haja anymoves fica a 0 e realiza o shuffle
					break;
				}
				else continue;
			}
			else if (i==board_pos_y-1){
				if (board[j][i]==board[j+1][i]){                        //enquanto que de 0 ate board_pos_y-2-1 e 0 ate jboard_pos_x-2 ele itera e checka o de baixo e lado
					*any_move=1;                                          // em board_pos_x-1 e board_pos_y-1 ele so tem que verificar para baixo e po lado, respetivamente,
					break;                                            //e como tal sao estes casos
				}
				else continue;
			}
			else if ( (board[j][i]==board[j+1][i]) || (board[j][i]==board[j][i+1])){
				*any_move=1;
				break;
			}
          }
     }
}



void ShuffleGame(int board_pos_y, int board_pos_x, int board[][MAX_BOARD_POS])
{
     int k = 0;
     int i = 0;
     int j = 0;
     int a = 0;
     int b = 0;
     int aux = 0;

     srand(time(NULL));

     for (k = 0; k < 10; k++){

          i = rand() % board_pos_y;          //o shuffle é realizado com uma formula simples, em que gera-se 10 vezes 2 posiçoes aleatorias de cada vez
          j = rand() % board_pos_x;          // e em cada vez troca-se a posiçao de uma pela outra, ou seja, no final trocamos 10 prontos e a probabilidade
          a = rand() % board_pos_y;          // de haver combinaçoes é muito elevada
          b = rand() % board_pos_x;

          aux = board[i][j];
          board[i][j] = board[a][b];
          board[a][b] = aux;


     }

}

void VectorTransformation(int victorloose[100])
{
     int i = 0;

     for (i = 0; i < 100; i++){         // o meu vetor inicializado a 0 para -1 para nao confundir na escirta
          victorloose[i] = -1;
     }
}

void Showthetxt(int round_number, int victorloose[100], int playsifwin[100], char playername[100])
{

     FILE *stats;

     stats = fopen("stats.txt", "w");
     if (stats == NULL)
     {
          printf("Error opening file!\n");
          exit(1);
     }


     fprintf(stats, "This is your stats in ISTDOTS by AMCS: ");
     fprintf(stats, "\n");
     fprintf(stats, "\n");

     /* print integers and floats */
     fprintf(stats, "Player: %s\n",playername);                            //faz o txt, onde imprime um simples texto, o nome do jogador, os jogos que começou
                                                                           // e o resultado de cada 1.
     fprintf(stats, "You have started %d games: ", round_number);

     fprintf(stats, "\n");
     fprintf(stats, "\n");

     int i = 0;

     for (i = 1; i <= round_number; i++){
          if (victorloose[i] == 0){
               fprintf(stats, "Round: %d, Result: Defeat\n", i);
          }
          if (victorloose[i] == 1){
               fprintf(stats, "Round: %d, Result: Victory, Played rounds: %d\n", i, playsifwin[i]);
          }
     }
}


void RenderMessage(SDL_Renderer *_renderer, TTF_Font *_font, int loose_game, int win_game, int *playable, int round_number)
{
          SDL_Color light = {238, 232, 170};
          SDL_Color dark = {0 , 0, 0};                      //alteradas as cores
          SDL_Rect rect = {130, 240, 400, 150};


          if (round_number == 0){
               SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a);

               SDL_RenderFillRect(_renderer, &rect);

               RenderText(rect.x+ 45, rect.y+15, "Welcome my friend", _font, &dark, _renderer ); //gera o aviso caso fique sem hipoteses de movimento
               RenderText(rect.x+35, rect.y+65, "Press n to start now", _font, &dark, _renderer );
          }


          if (loose_game == 1){
               *playable = 0;

               SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a);

               SDL_RenderFillRect(_renderer, &rect);                                                     //gera o aviso caso perca o jogo

               RenderText(rect.x+ 12, rect.y+15, "You have lost the game!", _font, &dark, _renderer );
               RenderText(rect.x+65, rect.y+65, "Press n to restart", _font, &dark, _renderer );
          }

          if (win_game == 1){
               *playable = 0;

               SDL_SetRenderDrawColor(_renderer, light.r, light.g, light.b, light.a);                    //gera o aviso caso fique sem hipoteses de movimento

               SDL_RenderFillRect(_renderer, &rect);

               RenderText(rect.x +45, rect.y+15, "Congratz, you won!", _font, &dark, _renderer );
               RenderText(rect.x+70, rect.y+65, "Press n to restart", _font, &dark, _renderer );
          }
}

int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

void UndoIt(int colorquantity, int *roundsquantity, int *roundsquantitybefore, int achievementpoints[MAX_BOARD_POS], int achievementpointsbefore[MAX_BOARD_POS],
               int board[][MAX_BOARD_POS], int boardbefore[][MAX_BOARD_POS], int board_pos_x, int board_pos_y)
{
     int i = 0;
     int j = 0;

     for ( i = 0; i < colorquantity; i++){
          achievementpoints[i]=achievementpointsbefore[i];  // volta a por os valores anterios do que temos que retirar as cores bem como a seguir das rondas q faltam
     }

     *roundsquantity = *roundsquantitybefore;

     for (i = 0; i < board_pos_y; i++){
          for (j = 0; j < board_pos_x; j++){
               board[i][j] = boardbefore[i][j];  //guarda a matriz caso tenha que fazer undo.
          }
     }
}
