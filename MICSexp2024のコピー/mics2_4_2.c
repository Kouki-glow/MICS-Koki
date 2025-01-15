#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>  

#define BUCKET_SIZE   17   /* ハッシュ表のサイズ */
#define ARY_UNIT  256   /* トランザクションを保持する配列サイズの単位 */

/* 実行時間の出力(古いやり方?) */
void  printExecSec( struct timeval  st, struct timeval  et, char  *msg ){
  if( et.tv_usec > st.tv_usec ){
    printf("exectime(sec)\t%s:\t%ld.%06ld\n", msg, (et.tv_sec - st.tv_sec), (et.tv_usec - st.tv_usec));
  } else {
    printf("exectime(sec)\t%s:\t%ld.%06ld\n", msg, (et.tv_sec - st.tv_sec), (1000000 + et.tv_usec - st.tv_usec));
  }

  return;
}

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

/* 頻度カウント処理 */
void countFrequency(int *tran, int tlen) {
  for (int i = 0; i < tlen; i++) {
    struct cell *p = searchHashTab(tran[i]);
    if (p != NULL) {
      p->count++;
    } else {
      p = newCell();
      p->item = tran[i];
      p->count = 1;
      int h = hash(tran[i]);
      p->next = htab[h];
      htab[h] = p;
    }
  }
}

/* 頻出アイテム決定処理 */
void findFrequentItems(int minsup) {
  printf("Frequent items (minsup = %d):\n", minsup);
  for (int i = 0; i < BUCKET_SIZE; i++) {
    struct cell *p = htab[i];
    struct cell *prev = NULL;
    while (p != NULL) {
      if (p->count >= minsup) {
        printf("Item: %d, Count: %d\n", p->item, p->count);
        prev = p;
        p = p->next;
      } else {
        struct cell *temp = p;
        if (prev == NULL) {
          htab[i] = p->next;
        } else {
          prev->next = p->next;
        }
        p = p->next;
        free(temp);
      }
    }
  }
}

/* 長さ2のアイテムセットを保持する構造体の定義 */
struct pair {
  int item1;
  int item2;
  int count;
  struct pair *next;
};

static struct pair *pair_htab[BUCKET_SIZE];  /* ハッシュ表: 各バケットの先頭のペアへのポインタ */

/* ハッシュ表の配列を初期化 (ペア用) */
void initPairHashTab() {
  for (int i = 0; i < BUCKET_SIZE; i++) {
    pair_htab[i] = NULL;
  }
}

/* ハッシュ値の計算 (ペア用) */
int pair_hash(int key1, int key2) {
  int h = (key1 + key2) % BUCKET_SIZE;
  if ((h < 0) || (h >= BUCKET_SIZE)) {
    fprintf(stderr, "Error: hash value = %d, keys=%d,%d\n", h, key1, key2);
    exit(1);
  }
  return h;
}

/* ハッシュ表の検索 (ペア用) */
struct pair *searchPairHashTab(int key1, int key2) {
  struct pair *p;
  for (p = pair_htab[pair_hash(key1, key2)]; p != NULL; p = p->next) {
    if (key1 == p->item1 && key2 == p->item2) {
      return p;
    }
  }
  return NULL;
}

/* 新たなpair領域を確保 */
struct pair *newPair() {
  struct pair *p;
  if ((p = (struct pair *)malloc(sizeof(struct pair))) == NULL) {
    fprintf(stderr, "Error: malloc\n");
    exit(1);
  }
  p->item1 = -1;
  p->item2 = -1;
  p->count = 0;
  p->next = NULL;
  return p;
}

/* ハッシュ表に挿入 (ペア用) */
void insertPairHashTab(int key1, int key2) {
  struct pair *p = newPair();
  p->item1 = key1;
  p->item2 = key2;
  int h = pair_hash(key1, key2);
  p->next = pair_htab[h];
  pair_htab[h] = p;
}

/* 長さ1の頻出アイテムセットから長さ2の候補アイテムセットを作成 */
void generateCandidatePairs() {
  for (int i = 0; i < BUCKET_SIZE; i++) {
    struct cell *p1 = htab[i];
    while (p1 != NULL) {
      for (int j = i; j < BUCKET_SIZE; j++) {
        struct cell *p2 = (j == i) ? p1->next : htab[j];
        while (p2 != NULL) {
          insertPairHashTab(p1->item, p2->item);
          p2 = p2->next;
        }
      }
      p1 = p1->next;
    }
  }
}

/* 頻度カウント処理 (ペア用) */
void countPairFrequency(int *tran, int tlen) {
  for (int i = 0; i < tlen; i++) {
    for (int j = i + 1; j < tlen; j++) {
      struct pair *p = searchPairHashTab(tran[i], tran[j]);
      if (p != NULL) {
        p->count++;
      }
    }
  }
}

/* 頻出アイテム決定処理 (ペア用) */
void findFrequentPairs(int minsup) {
  printf("Frequent pairs (minsup = %d):\n", minsup);
  for (int i = 0; i < BUCKET_SIZE; i++) {
    struct pair *p = pair_htab[i];
    struct pair *prev = NULL;
    while (p != NULL) {
      if (p->count >= minsup) {
        printf("Pair: (%d, %d), Count: %d\n", p->item1, p->item2, p->count);
        prev = p;
        p = p->next;
      } else {
        struct pair *temp = p;
        if (prev == NULL) {
          pair_htab[i] = p->next;
        } else {
          prev->next = p->next;
        }
        p = p->next;
        free(temp);
      }
    }
  }
}

/* ハッシュ表の領域を解放 (ペア用) */
void freePairHashTab() {
  for (int i = 0; i < BUCKET_SIZE; i++) {
    struct pair *p = pair_htab[i];
    while (p != NULL) {
      struct pair *temp = p->next;
      free(p);
      p = temp;
    }
  }
  initPairHashTab();
}

int main(int argc, char **argv ) { //最小指示度,ファイル名の順でコマンドライン引数により指定

if( argc != 3 ){  //引数が2つであることを確認
    fprintf(stderr, "引数が2つではありません。最小指示度 ファイル名 を指定して下さい。\n");
    return -1;
  }

  int minsup;//最小指示度
  int  i;
  int  trans;  /* トランザクション数を数える変数 */
  int  tlen;   /* 1件のトランザクションの長さを保持する変数 */
  int  *tran;  /* 1件のトランザクションを保持する配列 */
  char  *tranfile;  /* トランザクションファイル名を保持する変数 */
  FILE  *fp;
  struct timeval  stime, etime;  /* 処理時間の計測に用いる変数 */

/*最小指示度を取得*/
  argv++;
  minsup = atoi( *argv );

  /* ファイル名を取得 */
  argv++;
  if( (tranfile = (char *)malloc((strlen(*argv)+1)*sizeof(char)))
      == NULL ){
    fprintf(stderr, "Error: malloc for tranfile\n");
    return -1;
  }
  strcpy( tranfile, *argv );
  
  /* トランザクションを保持する配列を確保 */
  if( (tran = (int *)malloc(sizeof(int)*ARY_UNIT))
      == NULL ){
    fprintf(stderr, "Error: malloc for tran ary\n");
    return -1;
  }
  
  /* 開始時刻を取得 */
  gettimeofday( &stime, NULL );

  /* トランザクションファイルを開く */
  if( (fp = fopen(tranfile,"r")) == NULL ){
    fprintf(stderr, "Error: file(%s) open\n", tranfile);
    return -1;
  }

  /* ハッシュ表を初期化 */
  initHashTab();

  /* トランザクションファイルの1行を読み出す */
  trans = 0;
  while( fscanf(fp, "%d", &tlen) != EOF ){
    /* 先頭の値が-1のとき、終了 */
    if( tlen < 0 ){
      break;
    }

    /* トランザクション長をチェック */
    if( tlen > sizeof(tran) ){
      /* 読み出すトランザクションより配列サイズが小さいので領域を確保し直す
       *   一旦 削除して、新たに十分な領域を確保する  */
      free(tran);   /* 一旦 削除 */
      if( (tran = (int *)malloc(sizeof(int)*((int)ceil((double)tlen/ARY_UNIT)*ARY_UNIT)))
	  == NULL ){   /* 配列のサイズをARY_UNITの倍数となるようにした */
	fprintf(stderr, "Error: malloc for tran ary [%d]%d\n", trans, tlen);
	return -1;
      }}
    
    /* トランザクション長だけアイテムを読み込む */
    trans++;
    for( i = 0; i < tlen; i++ ){
      /* アイテムを1つ読み込む */
      fscanf(fp, " %d", &tran[i]);
    }
    /* 1行の末尾の改行 */
    fscanf(fp, "\n");

    /* 読み込んだ1件のトランザクションを出力 */
    for( i = 0; i < tlen; i++ ){
      printf("%d ", tran[i]);
    }
    printf("\n");

    /* 頻度カウント処理 */
    countFrequency(tran, tlen);
  }
  /* ファイルを閉じる */
  fclose(fp);

  /* 頻出アイテム決定処理 (パス1) */
  findFrequentItems(minsup);

  /* 長さ2の候補アイテムセットを作成 */
  generateCandidatePairs();

  /* トランザクションファイルを再度開く */
  if ((fp = fopen(tranfile, "r")) == NULL) {
    fprintf(stderr, "Error: file(%s) open\n", tranfile);
    return -1;
  }

  /* トランザクションファイルの1行を読み出す (パス2) */
  while (fscanf(fp, "%d", &tlen) != EOF) {
    if (tlen < 0) {
      break;
    }
    if (tlen > sizeof(tran)) {
      free(tran);
      if ((tran = (int *)malloc(sizeof(int) * ((int)ceil((double)tlen / ARY_UNIT) * ARY_UNIT))) == NULL) {
        fprintf(stderr, "Error: malloc for tran ary [%d]%d\n", trans, tlen);
        return -1;
      }
    }
    for (i = 0; i < tlen; i++) {
      fscanf(fp, " %d", &tran[i]);
    }
    fscanf(fp, "\n");

    /* 頻度カウント処理 (ペア用) */
    countPairFrequency(tran, tlen);
  }
  fclose(fp);

  /* 頻出アイテム決定処理 (ペア用) */
  findFrequentPairs(minsup);

  /* ハッシュ表の領域を解放 (ペア用) */
  freePairHashTab();

  /* 終了時刻を取得し、処理時間を出力 */
  gettimeofday( &etime, NULL );
  printExecSec( stime, etime, "readTransaction" );

  /* その他情報を出力 */
  printf("---\n");
  printf("FileName: %s\n", tranfile);
  printf("Transactions: %d\n", trans);

  /* ハッシュ表の領域を解放 */
  freeHashTab();

    return 0;
}