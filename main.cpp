/******************************************************************
 ******************************************************************
 **************************BEST OF KUSOGE**************************
 ******************************************************************
 ****************Shooting Kuso Game ~Extra Edition~****************
 ******************************************************************
 ******************************************************************/

/*==========================Header File===========================*/

#include <iostream>
#include <unistd.h>
#include <curses.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <time.h>

/*================================================================*/

/*=============================Macro==============================*/

#define WIN_W 40 // Window Width
#define WIN_H 30 // Window Height
#define WIN_OUT 60
#define ENE_MAX 5 // Maximum Number of Enemies
#define WEA_MAX 30 // Maximum Number of Weaks
#define MIS_MAX 100 // Maximum Number of Missiles
#define EMIS_MAX 30 // Maximum Number of Enemies' Missiles
#define INF (2 << 25)

/*================================================================*/

using namespace std;

/*========================Structure body==========================*/

/*---------------------------Class Player-------------------------*/

class Player{

private:

  char player; // プレイヤー機
  int x; // プレイヤー座標横
  int y; // プレイヤー座標縦
  int mis_kind;
  int mis_lock;
  
public:

  // コンストラクタ
  Player(){
    
    player = 'A';
    x = (WIN_W-1)/2;
    y = WIN_H-1;
    mis_kind = 2;
    mis_lock = 0;
    
  }
  ~Player(){} // デストラクタ
  void Move(); // プレイヤー動作
  void Damage(int bx, int by);
  void Died(int bx, int by); // プレイヤー撃墜
  void Reset(){
    
    player = 'A';
    x = (WIN_W-1)/2;
    y = WIN_H-1;
    mis_kind = 2;
    mis_lock = false;
    
  }

}P;

/*-------------------------Class Enemy----------------------------*/

class Enemy{

private:

  int enemy;
  //char enemy; // エネミー機
  int x; // エネミー座標横
  int y; // エネミー座標縦

public:

  // コンストラクタ
  Enemy(int name, bool sec){

    //enemy = name;
    if(!sec) enemy = 'V';
    else enemy = '\"';
    x = rand()%WIN_W;
    y = 1;
    
  }
  ~Enemy(){} // デストラクタ
  void Move(int num); // エネミー動作
  int Died(int bx, int by); // エネミー撃墜
  int Coord(int mx){

    if(mx > x) return -1;
    else if(mx == x) return 0;
    else if(mx < x) return 1;

  }

};

/*--------------------------Class Boss----------------------------*/

class Boss{

private:

  char boss; // ボス機
  int x; // ボス座標
  int y; // ボス座標

public:

  Boss(){

    boss = 'W';
    x = (WIN_W-1)/2;
    y = 1;

  }
  ~Boss(){}
  void Move();
  void Died(int bx, int by);
  void Damage(int bx, int by);
  void Temp();
  void Reset(){

    boss = 'W';
    x = (WIN_W-1)/2;
    y = 1;

  }
  void Change(){ boss = '\''; }
  int Coord(int mx){

    if(mx > x) return -1;
    else if(mx == x) return 0;
    else if(mx < x) return 1;

  }

}B;

/*--------------------------Class Weak----------------------------*/

class Weak{

private:

  char weak;
  int x;
  int y;
  int advance;

public:

  Weak(){

    weak = 'U';
    x = rand()%WIN_W;
    y = 0;
    advance = rand()%2;

  }
  ~Weak(){}
  void Move();

};

/*------------------------Class Missile---------------------------*/

class Missile{

private:

  char missile; // ミサイル
  int x; // ミサイル座標横
  int y; // ミサイル座標縦
  int x_start; // ミサイル発射座標横
  int y_start; // ミサイル発射座標縦
  int dire;
  int cnter;

public:

  // コンストラクタ
  Missile(int sx, int sy, int kind, int number){

    switch(kind){
    case 0: missile = 'o'; break;
    case 1: missile = '|'; break;
    case 2: missile = 'l'; break;
    case 3: missile = '.'; break;
    case 4: missile = 'i'; break;
    }
    dire = number;
    x = x_start = sx;
    y = y_start = sy;
    cnter = 0;

  }
  ~Missile(){} // デストラクタ
  void Move(); // ミサイル動作
  void EMove(); // 敵ミサイル動作

};

/*--------------------------Class Laser---------------------------*/

class Laser{

private:

  char laser[5];
  char space[5];
  int x;
  int y;
  int x_start;
  int y_start;
  bool thin;

public:

  Laser(int sx, int sy){

    if(sx == 0){
      strcpy(laser, "vv");
      strcpy(space, "  ");
      x = x_start = sx;
      y = y_start = sy;
      thin = true;
    }
    else{
      if(sx == WIN_W-1){
	strcpy(laser, "vv");
	strcpy(space, "  ");
      }
      else{
	strcpy(laser, "vvv");
	strcpy(space, "   ");
      }
      x = x_start = sx-1;
      y = y_start = sy;
      thin = false;
    }

  }
  ~Laser(){}
  void Move();

};

/*================================================================*/

/*==========================Global variable=======================*/
 
Enemy *ene_point[6]; // Enemyポインタ
Weak *wea_point[30];
Missile *mis_point[100]; // Missileポインタ
Missile *emis_point[6][30]; // Missileポインタ(敵ミサイル)
Laser *las_point[30];
int ene_grave[6]; // 撃墜エネミー登録
int wea_exist[30]; // 出現中ウィーク登録
int ene_cnt; // エネミー出現数
int ene_broken; // エネミー撃墜数
int wea_cnt;
int mis_cnt; // ミサイル発射数
int mis_cnt_temp;
int emis_cnt[6]; // ミサイル発射数(敵ミサイル)
int las_cnt;
int pla_hp;
int bos_hp;
int pla_hp_temp;
int bos_hp_temp;
int pla_barrier;
int bos_barrier;
int ene_pro; // エネミー動作確率
int wea_pro; // ウィーク動作確率
int bos_pro; // ボス動作確率
int mis_i;
int wea_i;
int emis_j[6];
int las_i;
double score;
bool ene_hit; // エネミー撃墜確認
bool pla_hit; // プレイヤー撃墜確認
bool bos_hit;
bool wea_lost;
bool mis_lost;
bool emis_lost;
bool las_lost;
bool pla_finisher;
bool next;
bool end;
bool master;
bool secret;

char message0[] = "Hyper Beam: z-key";
char message1[] = "Enemy ";
char message2[] = "Final Score";
char message3[] = "Score ";
char message4[] = "Boss ";
char message5[] = "Missile ";
char message6[] = "Moving: Cursor-keys";
char message7[] = "Launching: Space-key";
char message8[] = "Quit: q-key";
char message9[] = "Player ";
char message10[] = "Select a Level...";
char message11[] = "Easy";
char message12[] = "Normal";
char message13[] = "Hard";
char message14[] = " Retry";
char message15[] = " Home";
char message16[] = " Quit";
char message17[] = "Amateur";
char message18[] = "Expert";
char message19[100];
char message20[] = "Quit: Ctrl+c-key";
char message21[] = "Secret";

/*================================================================*/

/*======================Class Member Function=====================*/

/*--------------------------Player Member-------------------------*/

void Player::Move(){

  int key; // 入力キー
  int px = x; // 1つ前のx
  int py = y; // 1つ前のy
  bool judge = true; // 動作確認
  int ch; // 移動先の状態

  if(mis_lock){

    mis_point[mis_cnt%MIS_MAX] = new Missile(x, y, 1, 0);
    ++mis_cnt;
    --mis_lock;

  }
  // 動作の決定
  key = getch();
  if(key == ' ' && !mis_lock){

    // ミサイル発射
    mis_point[mis_cnt%MIS_MAX] = new Missile(x, y, mis_kind, 0);
    ++mis_cnt;
    ++mis_cnt_temp;
    score *= 0.999;

    if(mis_kind == 3){
      mis_point[mis_cnt%MIS_MAX] = new Missile(x, y, mis_kind, 1);
      ++mis_cnt;
      ++mis_cnt_temp;
      score *= 0.999;
      mis_point[mis_cnt%MIS_MAX] = new Missile(x, y, mis_kind, 2);
      ++mis_cnt;
      ++mis_cnt_temp;
      score *= 0.999;
    }

  }
  else if(key == '\n' && !secret){
    switch(mis_kind){
    case 2: mis_kind = 3; break;
    case 3: mis_kind = 4; break;
    case 4: mis_kind = 2; break;
    }
  }
  else if(key == 'q') end = true;
  else if(key == 'a' && !secret){ (player == 'A') ? player = ' ': player = 'A'; }
  else if(key == 'z' && pla_finisher && pla_hp == 1 && !mis_lock && !secret ||
	  key == 'z' && master && !mis_lock && !secret){ 

    mis_point[mis_cnt%MIS_MAX] = new Missile(x, y, 1, 0);
    ++mis_cnt;
    mis_lock = 100;
    pla_finisher = false;
    move(WIN_H-5, WIN_OUT-strlen(message0)-5);
    clrtoeol();

  }
  else if(key == 'd' && !secret) P.Damage(x, y);
  switch(key){
  case KEY_UP: --y; break;
  case KEY_DOWN: ++y; break;
  case KEY_LEFT: --x; break;
  case KEY_RIGHT: ++x; break;
  default : judge = false;
  }
  if(x < 0) x = 0;
  else if(x >= WIN_W) x = WIN_W-1;
  if(y < 0) y = 0;
  else if(y >= WIN_H) y = WIN_H-1;

  if(pla_barrier > 0){
    (player == 'A') ? player = ' ': player = 'A';
    --pla_barrier;
  }

  // 移動
  if(judge) mvaddch(py, px, ' ');
  move(y, x);
  ch = inch();
  if(ch == 'o' && pla_barrier == 0 || 
     ch == 'v' && pla_barrier == 0 || 
     ch == 'V' && pla_barrier == 0 ||
     ch == 'W' && pla_barrier == 0 ||
     ch == 'U' && pla_barrier == 0){ if(!master) P.Damage(x, y); }
  else addch(player);

  timeout(1);

}

/*----------------------------------------------------------------*/

void Player::Damage(int bx, int by){

    mvaddch(by, bx, ' ');
    --pla_hp;
    score -= 500;
    if(pla_hp == 0) P.Died(bx, by);
    else{
      x = (WIN_W-1)/2;
      y = WIN_H-1;
      pla_barrier = 50;
    }

}

/*----------------------------------------------------------------*/

void Player::Died(int bx, int by){

  pla_hit = true;
  score -= 1000;
  player = '*';
  mvaddch(by, bx, player);
  move(WIN_H-15, WIN_OUT-10);
  clrtoeol();
  mvprintw(WIN_H-15, WIN_OUT-10, "0");
  refresh();

}

/*--------------------------Enemy Member--------------------------*/

void Enemy::Move(int num){

  int dire; // 移動先
  int px = x; // 1つ前のx
  int py = y; // 1つ前のy
  bool judge = true; // 動作確認
  int ch; // 移動先の状況

  // 動作の決定
  dire = rand()/(num+1)%ene_pro;

  switch(dire){ 
  case 0: --y; break;
  case 1: ++y; break;
  case 2: --x; break;
  case 3: ++x; break;    
  case 4:
    // ミサイル発射
    emis_point[num][emis_cnt[num]%EMIS_MAX] = new Missile(x, y, 0, 0);
    ++emis_cnt[num];
    judge = false;
    break;
  default: judge = false; break;
  }
  if(x < 0) x = 0;
  else if(x >= WIN_W) x = WIN_W-1;
  if(y < 1) y = 1;
  else if(y > WIN_H-2) y = WIN_H-2;
  if(judge) mvaddch(py, px, ' ');

  // 移動
  move(y, x);
  ch = inch();
  if(ch == 'l' || ch == '|') ene_hit = false;
  else if(ch == 'A' && pla_barrier == 0 && !master) P.Damage(x, y);
  addch(enemy);
  //printw("%x", enemy);

}

/*----------------------------------------------------------------*/

int Enemy::Died(int bx, int by){

  if(bx == x && by == y) return 1;
  else return -1;

}

/*---------------------------Boss Member--------------------------*/

void Boss::Move(){

  int dire;
  int px = x;
  int py = y;
  bool judge = true;
  int ch;                      

  dire = rand()%bos_pro;
  switch(dire){
  case 0: --y; break;
  case 1: ++y; break;
  case 2: --x; break;
  case 3: ++x; break;
  case 4:
    las_point[las_cnt%EMIS_MAX] = new Laser(x, y);
    ++las_cnt;
    judge = false;
    break;
  case 5: 
    bos_barrier = 30;
    judge = false;
    break;
  default: judge = false;
  }
  if(x < 0) x = 0;
  else if(x >= WIN_W) x = WIN_W-1;
  if(y < 1) y = 1;
  else if(y >= WIN_H/2) y = WIN_H/2-1;
  if(judge) mvaddch(py, px, ' ');

  if(bos_barrier > 0){
    mvaddch(py+1, px, ' ');
    mvaddch(y+1, x, '@');
    --bos_barrier;
  }
  if(bos_barrier == 0) mvaddch(y+1, x, ' ');

  move(y, x);
  ch = inch();
  addch(boss);
  if(ch == 'l' && bos_barrier == 0 ||
     ch == '|' && bos_barrier == 0) B.Damage(x, y);
  else if(ch == 'A' && pla_barrier == 0 && !master) P.Damage(x, y);

}

/*----------------------------------------------------------------*/

void Boss::Died(int bx, int by){

  bos_hit = true;
  score += 50000;
  boss = '*';
  mvaddch(by, bx, boss);
  //mvaddch(by, bx, '*');
  move(5, WIN_OUT-12);
  clrtoeol();
  mvprintw(5, WIN_OUT-12, "0");
  refresh();
}

/*----------------------------------------------------------------*/

void Boss::Damage(int bx, int by){

  --bos_hp;
  score += 1000;
  if(bos_hp == 0) B.Died(bx, by);

}

/*----------------------------------------------------------------*/

void Boss::Temp(){

  B.Died(x, y);

}

/*---------------------------Weak Member--------------------------*/

void Weak::Move(){

  int ch;

  move(y, x);
  ch = inch();
  addch(' ');
  ++y;
  if(advance == 1) ++x;
  else if(advance == 0) --x;
  if(x >= WIN_W){
    x = WIN_W-1;
    advance = 0;
  }
  else if(x < 0){
    x = 0;
    advance = 1;
  }
  if(y >= WIN_H) wea_lost = false;
  else{
    move(y, x);
    ch = inch();
    addch(weak);
    if(ch == 'A' && pla_barrier == 0 && !master) P.Damage(x, y);
  }

}

/*------------------------Missile Member--------------------------*/
 
void Missile::Move(){

  int ch;
  int pursuit;
  
  if(y < y_start && 0 <= x && x < WIN_W) mvaddch(y, x, ' ');
  --y;
  if(dire == 1 && cnter % 2 == 0) ++x;
  if(dire == 2 && cnter % 2 == 0) --x;
  if(missile == 'i'){
    if(!next){
      for(int i = 0; i < ene_cnt; i++){
	if(ene_grave[i] != 1){
	  pursuit = (ene_point[i] -> Coord(x));
	  break;
	}
      }
    }
    else if(next){
      pursuit = (B.Coord(x));
    }
    if(pursuit == -1) --x;
    else if(pursuit == 1) ++x;
  }
  if(y < 0 && missile != '|') mis_lost = false;
  else if(missile != '|' && 0 <= x && x < WIN_W){
    move(y, x);
    ch = inch();
    addch(missile);
    // エネミー撃破
    if(ch == 'V'){
      // 撃破したエネミー機の確認
      for(int i = 0; i < ene_cnt; i++){
	if(ene_grave[i] != 1){
	  int ene_dead = (ene_point[i] -> Died(x, y));
	  if(ene_dead == 1){
	    delete ene_point[i];
	    ene_grave[i] = 1;
	    ++ene_broken;
	    score += 10000;
	  }
	}
      }
    }
    else if(ch == 'W' && bos_barrier == 0) B.Damage(x, y);
  }
  else if(missile == '|'){
    move(y, x);
    addch(missile);
    if(x > 0) mvaddch(y, x-1, missile);
    if(x < WIN_W-1) mvaddch(y, x+1, missile);
    if(y < y_start-1){
      mvaddch(y+1, x, ' ');
      if(x > 0) mvaddch(y+1, x-1, ' ');
      if(x < WIN_W-1) mvaddch(y+1, x+1, ' ');
    }
    if(y < 0) mis_lost = false;
  }
  ++cnter;
  
}

/*----------------------------------------------------------------*/

void Missile::EMove(){

  int ch;

  if(y > y_start) mvaddch(y, x, ' ');
  ++y;
  if(y >= WIN_H) emis_lost = false;
  else{
    move(y, x);
    ch = inch();
    addch(missile);
    // プレイヤー機撃破
    if(ch == 'A' && pla_barrier == 0 && !master) P.Damage(x, y);
  } 

}

/*--------------------------Laser Member--------------------------*/

void Laser::Move(){

  int ch1;
  int ch2;
  int ch3 = ' ';
  
  if(y > y_start) mvaddstr(y, x, space);
  ++y;
  if(y >= WIN_H) las_lost = false;
  else{
    move(y, x); ch1 = inch();
    move(y, x+1); ch2 = inch();
    if(!thin) move(y, x+2); ch3 = inch();
    mvaddstr(y, x, laser);

    if(ch1 == 'A' && pla_barrier == 0||
       ch2 == 'A' && pla_barrier == 0|| 
       ch3 == 'A' && pla_barrier == 0){ if(!master) P.Damage(x+1, y); }
  }

}

/*================================================================*/

/*==============================Setup=============================*/

void Setting(){

  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  leaveok(stdscr, TRUE);
  scrollok(stdscr, TRUE);

}

/*================================================================*/

/*==========================Level_Select==========================*/

void Level_Select(){

  for(int i = 0; i < 5; i++){
    int xx, yy;
    yy = rand()%(WIN_H/2-7)+1;
    xx = rand()%WIN_OUT;
    mvaddch(yy, xx, 'V');
  }
  for(int i = WIN_H/2-5; i < WIN_H; i += 11){
    for(int j = 0; j < WIN_OUT; j++) mvaddch(i, j, '#');
  }
  mvaddstr(WIN_H/2-3, (WIN_OUT-strlen(message10))/2, message10);
  mvaddstr(WIN_H/2-1, (WIN_OUT-strlen(message11))/2, message17);
  mvaddstr(WIN_H/2, (WIN_OUT-strlen(message11))/2, message11);
  mvaddstr(WIN_H/2+1, (WIN_OUT-strlen(message11))/2, message12);
  mvaddstr(WIN_H/2+2, (WIN_OUT-strlen(message11))/2, message13);
  mvaddstr(WIN_H/2+3, (WIN_OUT-strlen(message11))/2, message18);
  mvaddstr(WIN_H/2+5, WIN_OUT-strlen(message20)-1, message20);
  refresh();
  int demo_cnt = 0;
  int x = (WIN_OUT-strlen(message11))/2-1;
  int y = WIN_H/2;
  int demo_x = WIN_OUT/2;
  int ch;
  int ch1;
  int ch2;
  int demo_mis_x = -1;
  int demo_mis_y = -1;
  bool demo_mis = false;
  do{
    mvaddch(WIN_H-2, demo_x, 'A');
    mvaddch(y, x, '>');
    if(demo_mis_y >= 0){
      move(demo_mis_y, demo_mis_x); ch2 = inch();
      if(ch2 == 'V') ++demo_cnt;
      mvaddch(demo_mis_y, demo_mis_x, '|');
      --demo_mis_y;
      if(demo_mis_y == WIN_H/2+6) demo_mis_y = WIN_H/2-6;
      else if(demo_mis_y == WIN_H/2-7) mvaddch(WIN_H/2+7, demo_mis_x, ' ');
    }
    else demo_mis = false;
    if(demo_cnt == 5){
      mvaddstr(WIN_H/2+4, (WIN_OUT-strlen(message11))/2, message21);
      secret = true;
    }
    ch = getch();
    int px = x;
    int py = y;
    int demo_px = demo_x;
    switch(ch){
    case '\n':
      if(y == WIN_H/2-1){
	ene_pro = 13;
	bos_pro = 10;
	wea_pro = 50;
	pla_hp = pla_hp_temp = 5;
	bos_hp = bos_hp_temp = 50;
	strcpy(message19, "Amateur");
      }
      else if(y == WIN_H/2){
	ene_pro = 13;
	bos_pro = 10;
	wea_pro = 8;
	pla_hp = pla_hp_temp = 3;
	bos_hp = bos_hp_temp = 80;
	strcpy(message19, "Easy");
      }
      else if(y == WIN_H/2+1){
	ene_pro = 11;
	bos_pro = 8;
	wea_pro = 2;
	pla_hp = pla_hp_temp = 3;
	bos_hp = bos_hp_temp = 80;
	strcpy(message19, "Normal");
      }
      else if(y == WIN_H/2+2){
	ene_pro = 7;
	bos_pro = 6;
	wea_pro = 1;
	pla_hp = pla_hp_temp = 3;
	bos_hp = bos_hp_temp = 100;
	strcpy(message19, "Hard");
      }
      else if(y == WIN_H/2+3){
	ene_pro = 7;
	bos_pro = 6;
	wea_pro = 1;
	pla_hp = pla_hp_temp = 1;
	bos_hp = bos_hp_temp = 200;
	strcpy(message19, "Expert");
      }
      else if(y == WIN_H/2+4){
	B.Change();
	ene_pro = 7;
	bos_pro = 6;
	wea_pro = 1;
	pla_hp = pla_hp_temp = 1;
	bos_hp = bos_hp_temp = 200;
	strcpy(message19, "Secret");
      }
      break;
    case KEY_UP: --y; break;
    case KEY_DOWN: ++y; break;
    case 'g':
	master = true;
	ene_pro = 7;
	bos_pro = 6;
	wea_pro = 1;
	pla_hp = pla_hp_temp = INF;
	bos_hp = bos_hp_temp = 1000;
	strcpy(message19, "Game Master");
	break;
    case KEY_RIGHT: ++demo_x; break;
    case KEY_LEFT: --demo_x; break;
    case ' ': 
      if(!demo_mis){
	demo_mis = true;
	demo_mis_x = demo_x;
	demo_mis_y = WIN_H-3;
      }
    }
    if(y < WIN_H/2-1) y = WIN_H/2-1; 
    else if(y > WIN_H/2+3 && !secret) y = WIN_H/2+3;
    else if(y > WIN_H/2+4) y = WIN_H/2+4;
    if(demo_x < 0) demo_x = 0;
    else if(demo_x >= WIN_OUT) demo_x = WIN_OUT-1;
    mvaddch(py, px, ' ');
    mvaddch(WIN_H-2, demo_px, ' ');
    move(demo_mis_y+1, demo_mis_x); ch1 = inch();
    if(ch1 == '|') addch(' ');
    refresh();
    timeout(1);
  }while(ch != '\n' && ch != 'g');
  
}

/*================================================================*/

/*==========================Message_Out===========================*/

void Message_Out(){

  mvaddstr(1, WIN_OUT-strlen(message3)-11, message19);
  mvaddstr(WIN_H-15, WIN_OUT-strlen(message3)-11, message9);
  if(pla_hp > 100) mvaddstr(WIN_H-15, WIN_OUT-10, "∞");
  else mvprintw(WIN_H-15, WIN_OUT-10, "%d", pla_hp);
  mvaddstr(WIN_H-13, WIN_OUT-strlen(message3)-11, message5);
  move(WIN_H-13, WIN_OUT-9);
  clrtoeol();
  mvprintw(WIN_H-13, WIN_OUT-9, "%d", mis_cnt_temp);
  mvaddstr(10, WIN_OUT-strlen(message3)-11, message3);
  move(10, WIN_OUT-11);
  clrtoeol();
  mvprintw(10, WIN_OUT-11, "%g", score);
  mvaddstr(WIN_H-9, WIN_OUT-strlen(message3)-11, message6);
  mvaddstr(WIN_H-7, WIN_OUT-strlen(message3)-11, message7);
  if(pla_hp == 1 && pla_finisher && !secret|| master && !secret)
    mvaddstr(WIN_H-5, WIN_OUT-strlen(message3)-11, message0);
  mvaddstr(WIN_H-3, WIN_OUT-strlen(message3)-11, message8);

}

/*================================================================*/

/*===========================Boss Battle==========================*/

void Boss_Battle(){

  erase();

  for(int i = 0; i < WIN_H; i++) mvaddch(i, WIN_W, '#');

  wea_i = 0;
  las_i = 0;

  bos_hit = false;
  bos_barrier = 0;
  las_cnt = 0;

  while(!end){
    usleep(50000);
    mvaddstr(5, WIN_OUT-strlen(message3)-11, message4);
    move(5, WIN_OUT-12);
    clrtoeol();
    mvprintw(5, WIN_OUT-12, "%d", bos_hp);
    Message_Out();
    refresh();

    P.Move();

    B.Move();

    if(rand()%wea_pro == 0 && wea_exist[wea_cnt%WEA_MAX] == -1){
      wea_point[wea_cnt%WEA_MAX] = new Weak();
      wea_exist[wea_cnt%WEA_MAX] = 1;
      ++wea_cnt;
    }

    // ウィーク動作
    for(int i = wea_i; i < wea_cnt; i++){
      wea_lost = true;
      wea_point[i%WEA_MAX] -> Move();
      if(!wea_lost){
	delete wea_point[i%WEA_MAX];
	wea_exist[i%WEA_MAX] = -1;
	++wea_i;
      }
    }

    if(score < 0) score = 0;

    if(pla_hit || bos_hit){
      mvaddstr(WIN_H/2-1, (WIN_W-strlen(message2))/2, message2);
      mvprintw(WIN_H/2+1, (WIN_W-5)/2, "%g", score);
      refresh();
      break;
    }

    // ミサイル動作
    for(int i = mis_i; i < mis_cnt; i++){
      mis_lost = true;
      mis_point[i%MIS_MAX] -> Move();
      if(!mis_lost){
	delete mis_point[i%MIS_MAX];
	++mis_i;
      }
    }

    for(int i = las_i; i < las_cnt; i++){
      las_lost = true;
      las_point[i%EMIS_MAX] -> Move();
      if(!las_lost){
	delete las_point[i%EMIS_MAX];
	++las_i;
      }
    }

  }
  
}

/*================================================================*/

/*============================Main Game===========================*/

void Run(){

  // 初期化
  wea_i = 0;
  mis_i = 0;
  pla_hit = false;
  pla_finisher = true;
  next = false;
  end = false;
  wea_cnt = 0;
  ene_cnt = 0;
  mis_cnt = 0;
  mis_cnt_temp = 0;
  pla_barrier = 0;
  ene_broken = 0;
  score = 0;
  memset(ene_grave, -1, sizeof(ene_grave));
  memset(wea_exist, -1, sizeof(wea_exist));
  memset(emis_j, 0, sizeof(emis_j));
  memset(emis_cnt, 0, sizeof(emis_cnt));
  memset(emis_cnt, 0, sizeof(emis_cnt));

  while(!end){
    usleep(50000);
    // メッセージの表示
    mvaddstr(5, WIN_OUT-strlen(message3)-11, message1);
    move(5, WIN_OUT-11);
    clrtoeol();
    mvprintw(5, WIN_OUT-11, "%d", ENE_MAX-ene_broken);
    Message_Out();
    refresh();

    // プレイヤー動作
    P.Move();

    // エネミー出現
    if(ene_cnt < ENE_MAX){
      int appear = rand()*ene_cnt%5;
      if(appear == 0){
	ene_point[ene_cnt] = new Enemy(ene_cnt, secret);
	++ene_cnt;
      }
    }

    // エネミー動作
    for(int i = 0; i < ene_cnt; i++){
      ene_hit = true;
      if(ene_grave[i] != 1){
	ene_point[i] -> Move(i);
	if(!ene_hit){
	  delete ene_point[i];
	  ene_grave[i] = 1;
	  ++ene_broken;
	  score += 10000;
	}
      }
    }

    if(rand()%wea_pro == 0 && wea_exist[wea_cnt%WEA_MAX] == -1){
      wea_point[wea_cnt%WEA_MAX] = new Weak();
      wea_exist[wea_cnt%WEA_MAX] = 1;
      ++wea_cnt;
    }

    // ウィーク動作
    for(int i = wea_i; i < wea_cnt; i++){
      wea_lost = true;
      wea_point[i%WEA_MAX] -> Move();
      if(!wea_lost){
	delete wea_point[i%WEA_MAX];
	wea_exist[i%WEA_MAX] = -1;
	++wea_i;
      }
    }
 
    if(score < 0) score = 0;

    // プレイヤー勝利
    if(ene_broken >= ENE_MAX){
      next = true;
      pla_hit = false;
      ene_cnt = 0;
      wea_cnt = 0;
      ene_broken = 0;
      memset(ene_grave, -1, sizeof(ene_grave));
      memset(wea_exist, -1, sizeof(wea_exist));
      memset(emis_j, 0, sizeof(emis_j));
      memset(emis_cnt, 0, sizeof(emis_cnt));
      break;
    }

    // プレイヤー敗北
    if(pla_hit){
      mvaddstr(WIN_H/2-1, (WIN_W-strlen(message2))/2, message2);
      mvprintw(WIN_H/2+1, (WIN_W-5)/2, "%g", score);
      refresh();
      break;
    }

    // ミサイル動作
    for(int i = mis_i; i < mis_cnt; i++){
      mis_lost = true;
      mis_point[i%MIS_MAX] -> Move();
      if(!mis_lost){
	delete mis_point[i%MIS_MAX];
	++mis_i;
      }
    }

     // ミサイル動作(敵ミサイル)
    for(int i = 0; i < ene_cnt; i++){
      for(int j = emis_j[i]; j < emis_cnt[i]; j ++){
	emis_lost = true;
	emis_point[i][j%EMIS_MAX] -> EMove();
	if(!emis_lost){
	  delete emis_point[i][j%EMIS_MAX];
	  ++emis_j[i];
	}
      }
    }

  }

}

/*================================================================*/

/*===========================Main Function========================*/

int main(){

  srand((unsigned)time(NULL));

  initscr();

  Setting(); // 端末設定

 Home:
  erase();
  master = false;
  secret = false;
  Level_Select();

 Retry:
  erase();
  for(int i = 0; i < WIN_H; i++) mvaddch(i, WIN_W, '#');
  refresh();

  Run(); // ゲーム本体
  if(next) Boss_Battle();

  if(!end) sleep(2);
  if(secret){
    P.Reset();
    B.Reset();
    goto Home;
  }
  mvaddstr(WIN_H/2-1, (WIN_W-strlen(message14))/2, message14);
  mvaddstr(WIN_H/2, (WIN_W-strlen(message14))/2, message15);
  mvaddstr(WIN_H/2+1, (WIN_W-strlen(message14))/2, message16);
  refresh();
  int x = (WIN_W-strlen(message14))/2; 
  int y = WIN_H/2-1;
  int ch;
  P.Reset();
  B.Reset();
  pla_hp = pla_hp_temp;
  bos_hp = bos_hp_temp;
  do{
    mvaddch(y, x, '>');
    ch = getch();
    int px = x;
    int py = y;
    switch(ch){
    case '\n':
      if(y == WIN_H/2-1) goto Retry;
      else if(y == WIN_H/2) goto Home;
      break;
    case KEY_UP: --y; break;
    case KEY_DOWN: ++y; break;
    }
    if(y < WIN_H/2-1) y = WIN_H/2-1;
    else if(y > WIN_H/2+1) y = WIN_H/2+1;
    mvaddch(py, px, ' ');
    refresh();
  }while(ch != '\n');
  
  endwin();
  
}

/*================================================================*/

