/*
 * トランザクションデータをファイルから読み出すサンプル
 *  Usage: exec transactionFile
 *
 *  Time-stamp: <2023-11-28 10:16:22 shintani>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>     /* コンパイルのオプション -lm が必要 */

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


/* サンプル */
int main( int argc, char **argv ){
  int  i;
  int  trans;  /* トランザクション数を数える変数 */
  int  tlen;   /* 1件のトランザクションの長さを保持する変数 */
  int  *tran;  /* 1件のトランザクションを保持する配列 */
  char  *tranfile;  /* トランザクションファイル名を保持する変数 */
  FILE  *fp;
  struct timeval  stime, etime;  /* 処理時間の計測に用いる変数 */

  if( argc != 2 ){
    fprintf(stderr, "Usage: %s transactionfile\n", *argv);
    return -1;
  }

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
  }
  /* ファイルを閉じる */
  fclose(fp);

  /* 終了時刻を取得し、処理時間を出力 */
  gettimeofday( &etime, NULL );
  printExecSec( stime, etime, "readTransaction" );

  /* その他情報を出力 */
  printf("---\n");
  printf("FileName: %s\n", tranfile);
  printf("Transactions: %d\n", trans);

  exit(0);
}


