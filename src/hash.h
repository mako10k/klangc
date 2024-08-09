#pragma once

#include "klangc.h"

/**
 * ハッシュテーブルを作成する
 * @param capacity ハッシュテーブルの初期容量
 * @return 作成されたハッシュテーブル
 */
klangc_hash_t *klangc_hash_new(int capacity);

/**
 * ハッシュテーブルからキーに対応する値を取得する
 * @param hash ハッシュテーブル
 * @param key キー
 * @param value 値を格納する変数へのポインタ
 * @return キーに対応する値が存在する場合は1、存在しない場合は0
 */
int klangc_hash_get(klangc_hash_t *hash, const char *key, void **value);

/**
 * ハッシュテーブルにキーと値を追加する
 * @param hash ハッシュテーブル
 * @param key キー
 * @param value 値
 * @param old_value 古い値を格納する変数へのポインタ
 * @return キーに対応する値が存在する場合は1、存在しない場合は0
 */
int klangc_hash_put(klangc_hash_t *hash, const char *key, void *value,
                    void **old_value);

/**
 * ハッシュテーブルからキーに対応する値を削除する
 * @param hash ハッシュテーブル
 * @param key キー
 * @param value 削除された値を格納する変数へのポインタ
 * @return キーに対応する値が存在する場合は1、存在しない場合は0
 */
int klangc_hash_remove(klangc_hash_t *hash, const char *key, void **value);

/**
 * ハッシュテーブルの各エントリに対してコールバック関数を実行する
 * @param hash ハッシュテーブル
 * @param callback コールバック関数
 * @param data コールバック関数に渡すデータ
 */
void klangc_hash_foreach(klangc_hash_t *hash,
                         void (*callback)(const char *, void *, void *),
                         void *data);
