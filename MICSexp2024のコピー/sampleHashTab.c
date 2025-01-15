/*
 * ハッシュ表のサンプルプログラム
 *  ハッシュ表をグローバル変数で実現
 *
 *  Time-stamp: <2023-11-27 13:28:04 shintani>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define BUCKET_SIZE   17   /* ハッシュ表のサイズ */


/* アイテムを保持する構造体の定義 */
struct cell{
  int  item;     /* アイテムを保持 */
  int count;    /* 頻度を保持 */
  struct cell *next;   /* 次のセルへのポインタ */
};

static struct cell  *htab[BUCKET_SIZE];  /* ハッシュ表: 各バケットの先頭のセルへのポインタ */


/* ハッシュ表の配列を初期化 */
void  initHashTab(){
  int   i;

  for( i = 0; i < BUCKET_SIZE; i++ ){
    htab[i] = NULL;
  }

  return;
}


/* ハッシュ値の計算 */
int  hash( int  key ){
  int  h;

  /* 値をハッシュ表のサイズで割った余りをハッシュ値とする例 */
  h = key % BUCKET_SIZE;
  if( (h < 0) || (h >= BUCKET_SIZE) ){
    fprintf(stderr, "Error: hash value = %d, key=%d\n", h, key);
    exit(1);
  }
  return( h );
}


/* ハッシュ表の検索 */
struct cell  *searchHashTab( int  key ){
  struct cell  *p;

  /* keyのハッシュ値を求め、バケットのセルを辿って調べる */
  for(p = htab[hash(key)]; p != NULL; p = p->next ){
    /* keyと等しいアイテムをもつセルを見つける */
    if(key == p->item){
      /* keyを発見 */
      return( p );
    }}

  /* keyが見つからなかった */
  return( NULL );
}


/* 新たなcell領域を確保 */
struct cell  *newCell(){
  struct cell  *p;

  /* cell領域を確保 */
  if( (p = (struct cell  *)malloc(sizeof(struct cell)))
      == NULL ){
    fprintf(stderr, "Error: malloc\n");
    exit(1);
  }
  /* 初期値を設定 */
  p->item = -1;
  p->next = NULL;

  return( p );
}


/* ハッシュ表に挿入 */
int  insertHashTab( int  key ){
  int  h;
  struct cell  *p;

  /* 既に登録されているかチェック */
  if( searchHashTab( key ) != NULL){
    /* 複製を発見 */
    return(1);
  }

  /* 新たにセル領域を確保し、アイテム、頻度の初期値を設定 */
  p = newCell();
  p->item = key;

  /* 作成したセルをハッシュ表に挿入 */
  h = hash( key );
  p->next = htab[h];
  htab[h] = p;

  return( 0 );
}


/* ハッシュ表に保持されたアイテムを出力 */
void  scanHashTab(){
  int  i;
  struct cell  *p;
 
  printf("---\n");
  for( i = 0; i < BUCKET_SIZE; i++ ){
    printf("htab[%d]:", i);

    /* htab[i]に保持されたセルを出力 */
    p = htab[i];
    while( p != NULL ){
      printf("\t%d", p->item);
      p = p->next;
    }
    printf("\n");
  }
  printf("---\n");

  return;
}


/* ハッシュ表の領域を解放 */
void  freeHashTab(){
  int  i = 0;
  struct cell  *p, *temp;
 
  for( i = 0; i < BUCKET_SIZE; i++ ){
    p = htab[i];
    /* リストの解放 */
    while( p != NULL ){
      temp = p->next;
      free( p );
      p = temp;
    }}
  initHashTab();

  return;
}


int  main( int  argc,  char **argv ){

  /* ハッシュ表の初期化 */
  initHashTab();

  /* アイテムの登録例 */
  struct cell  *p;
  int  v;
 
  v = 12583;
  insertHashTab( v );  /* "12583"を登録 */

  v = 15;
  insertHashTab( v );   /* "1"5を登録 */

  /* データを検索例 */
  /** 発見したときの例 **/
  v = 12583;
  p = searchHashTab( v );  /* "12583"を探索 */
  if( p != NULL ){
    /* 発見したときに出力 */
    printf("Found\tItem=%d\n", p->item);
  }

  /** 発見できなかったときの例 */
  v = 100;
  p = searchHashTab( v );  /* "100"を探索 */
  if( p != NULL ){
    /* 発見したときに出力 */
    printf("Found\tItem=%d\n", p->item);
  }

  /* ハッシュ表をスキャンする例 (値が挿入されていることを確認) */
  scanHashTab();

  /* ハッシュ表を空にする例 */
  freeHashTab();

  /* ハッシュ表をスキャンする例 (空になっていることを確認) */
  scanHashTab();

  exit(0);
}
