/*
 * int型のメモリ領域のブロックを管理するサンプルプログラム
 *
 *  Time-stamp: <2011-12-02 18:33:12 shintani>
 */

#include <stdio.h>
#include <stdlib.h>

#define  INT_MEM 128      /* intバッファのサイズ */

/* ブロックの構造定義 */
struct  intblock {
  int  buff[INT_MEM];      /* intバッファとなるint型の配列 */
  struct intblock  *next;  /* 次のメモリブロックへのリンク */
};

struct intblock  *topib;  /* 先頭のメモリブロック */
struct intblock  *curib; /* 現在のメモリブロックへのポインタ */
int  *curibp;   /* 現在のメモリブロックのintバッファの現在位置 */
int  curibr;    /* 現在のメモリブロックのintバッファの未利用数 */


/* メモリブロックを確保する関数 */
struct intblock  *AllocIntBlock( struct intblock  *cur ){
  /* 新しいメモリブロックを確保 */
  struct intblock  *newblk;
  if( !(newblk
	= (struct intblock *)malloc(sizeof(struct intblock))) ){
    fprintf(stderr, "Error: malloc for intblock\n");
    exit(1);
  }
  /* 新しいメモリブロックを初期化 */
  newblk->next = NULL;

  /* 現在のメモリブロックの次に繋げる */
  if( cur ){
    cur->next = newblk;
  }

  /* 確保したメモリブロックの位置を返す */
  return( newblk );
}


/* 現在のメモリブロックのint型領域を割り当てる関数 */
int  *GetIntBuff( int  len ){
  /* 割り当てようとしている数が多すぎる場合のエラー処理 */
  if( len > INT_MEM ){
    fprintf(stderr, "Error: INT_MEM too small\n");
    exit(1);
  }

  int  *prevp;
  /* 割り当てたい数(len)のint型領域が余っていないとき、次のメモリブロックを確保 */
  if( len > curibr ){
    curib = AllocIntBlock( curib );  /* 次のメモリブロックを確保 */
    curibp = curib->buff;  /* 現在のメモリブロックのintバッファの位置を初期化 */
    curibr = INT_MEM;      /* 現在のメモリブロックのintバッファの未利用数を初期化 */
  }
  prevp = curibp;   /* 割り当てた先頭位置を保持 */
  curibp += len;    /* 使った分だけintバッファの位置をずらす */
  curibr -= len;    /* 使った分だけintバッファの未利用数から引く */

  return( prevp );  /* 割り当てた先頭位置を返す */
}


/* 確保した全メモリバッファを解放する関数 */
void  FreeIntBuff(){
  struct intblock  *cur, *tmp;
  /* 先頭のメモリバッファから順に解放する */
  cur = topib;
  while( cur ){
    tmp = cur;
    cur = cur->next;
    free( tmp );
    printf("free intblock\n");
  }
  return;
}


int  main( int  argc, char  **argv ){
  /* 先頭のメモリブロックを初期化 */
  topib = AllocIntBlock( NULL );

  /* 利用するメモリブロックの情報(現在のメモリブロック)の初期化 */
  curib = topib;   /* 現在のメモリブロックのポインタの保持 */
  curibp = curib->buff;   /* 現在のメモリブロックのint型バッファの先頭を保持 */
  curibr = INT_MEM;  /* 現在のメモリブロックのint型バッファの未利用数を設定 */

  /*
   * int型の領域を使う例1
   */
  int  i;
  int  *intary;
  intary = GetIntBuff( 32 );  /* intpに32個のint型、つまりsizeof(int)*32 を割り当て */
  for( i = 0; i < 32; i++ ){
    intary[i] = i+10;
  }

  /*
   * int型の領域を使う例2
   */
  int  *inttab[10];  /* 10個のint型ポインタの配列を用意しておく */
  int  inttabn[10];  /* それぞれのint型ポインタに割り当てるint型のサイズを保持 */
  for( i = 0; i < 10; i++ ){
    /* i番目にlen個のint型を割り当てる */
    int  len = (i+3)*3;   /* lenの値を適当に設定 */
    inttab[i] = GetIntBuff( len );  /* len個のint型を割り当て */
    inttabn[i] = len;
  }
  /* 適当に数字を入れてみる */
  int  h;
  for( h = 0; h < 10; h++){
    for( i = 0; i < inttabn[h]; i++ ){
      inttab[h][i] = h+i;
    }}

  /* 数字が入っているか見てみる */
  for( i = 0; i < 32; i++ ){
    printf("[%d] %d\n", i, intary[i]);
  }
  for( h = 0; h < 10; h++){
    for( i = 0; i < inttabn[h]; i++ ){
      printf("[%d][%d] %d\n", h, i, inttab[h][i]);
    }}


  /* 全メモリバッファを解放 */
  FreeIntBuff();

  exit(0);
}


