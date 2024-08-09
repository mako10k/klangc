#pragma once

#include "klangc.h"
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/**
 * 入力
 */
typedef struct klangc_input klangc_input_t;

/**
 * 新しい入力を作成
 * @param fp ファイルポインタ
 * @param name 入力の名前
 * @return 作成した入力
 */
klangc_input_t *klangc_input_new(FILE *fp, const char *name);

/**
 * 入力の名前を取得
 * @param input 入力
 * @return 入力の名前
 */
const char *klangc_input_get_name(klangc_input_t *input);

/**
 * 入力位置を返す
 * @param input 入力
 * @return 入力位置
 */
klangc_ipos_t klangc_input_save(klangc_input_t *input);

/**
 * 入力位置を復元
 * @param input 入力
 * @param ipos 入力位置
 */
void klangc_input_restore(klangc_input_t *input, klangc_ipos_t ipos);

/**
 * 入力から1文字読み込む
 * @param input 入力
 * @return 読み込んだ文字
 */
int klangc_getc(klangc_input_t *input);

/**
 * 入力文字がスペースかどうかチェック
 * @param in_comment コメント中かどうかを格納する変数へのポインタ
 * @return スペースの場合は1、それ以外の場合は0
 */
int klangc_isspace(int c, int *in_comment);

/**
 * スペース以外の1文字を入力する
 * @param input 入力
 * @return スペースをスキップした後の文字
 */
int klangc_getc_skipspaces(klangc_input_t *input);

/**
 * スペースをスキップ
 * @param input 入力
 * @return スペースをスキップした後の入力位置
 */
klangc_ipos_t klangc_skipspaces(klangc_input_t *input);

/**
 * ファイル名と位置を出力
 * @param input 入力
 * @param ipos 入力位置
 */
void klangc_ipos_print(klangc_output_t *output, klangc_ipos_t ipos);