/*
 * コマインドライン引数を読み込むサンプルプログラム
 *  オプションで指定する引数が 3つ (文字列、整数、小数)の例です
 *
 *  Usage: ./a.out 引数1 引数2 引数3
 *
 *  Time-stamp: <2023-11-29 15:09:53 shintani>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* サンプル */
int main( int argc, char **argv ){
  /* argc 実行時に指定されたコマンドライン引数の数が入っている */
  /* **argv 実行時に指定されたコマンドライン引数が文字列で入っている */

  char  *mojiretu;   /* 1つめの引数である文字列を保持する変数 */
  int  suuji;        /* 2つめの引数である整数を保持する変数 */
  double  syousuu;   /* 3つめの引数である小数を保持する変数 (とりあえずdoubleにした) */

  /* 引数が3つ指定されたかを確認 */ 
  if( argc != 4 ){  /* コマンドも引数の1つなので、1+3=4 となる*/
    fprintf(stderr, "arguments %d < 3\n", argc-1);
    return -1;
  }

  /* 1つめの引数 文字列 を読み込む */
  argv++;  /* コマンドの次の引数の位置にずらしておく */
  if( (mojiretu = (char *)malloc((strlen(*argv)+1)*sizeof(char)))
      == NULL ){  /* 引数で指定された文字列を保持できるサイズの領域を確保 */
    fprintf(stderr, "Error: malloc for mojiretu\n");
    return -1;
  }
  strcpy( mojiretu, *argv );  /* 変数に引数の文字列をコピー */

  printf("(1) %s\n", mojiretu);  /* 出力して確認 */


  /* 2つめの引数 整数 を読み込む */
  argv++;  /* 次の引数の位置にずらしておく */
  suuji = atoi( *argv );

  printf("(2) %d\n", suuji);


  /* 3つめの引数 小数(doubleの例) を読み込む */
  argv++;  /* 次の引数の位置にずらしておく */
  syousuu = atof( *argv );

  printf("(3) %lf\n", syousuu);
  

  exit(0);
}
