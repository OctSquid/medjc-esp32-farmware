# MEDJC09-HUB-Farmware

## 概要

乾式筋電センサ`MEDJC-09`を最大 4 台まで制御できるハブデバイスのファームウェア</br>
センサの電圧読み取りは`ADS1115`を使用する．

## ハードウェア構成

- マイコン : ESP32
- ADC : `ADS1115` 最大 4 台
- 筋電センサ : `MEDJC-09` 最大 4 台

### ADC アドレス

| ADC 番号 | I2C アドレス | 接続ピン |
| :------: | :----------: | :------: |
|   `0`    |    `0x48`    |   GND    |
|   `1`    |    `0x49`    |   VDD    |
|   `2`    |    `0x4A`    |   SDA    |
|   `3`    |    `0x4B`    |   SCL    |

### 筋電センサピン配置

| 筋電センサ番号 | 対応 ADC 番号 |       ME ピン       | SME ピン |
| :------------: | :-----------: | :-----------------: | :------: |
|      `0`       |      `0`      | `A1` (`A0`との差動) |   `A2`   |
|      `1`       |      `1`      | `A1` (`A0`との差動) |   `A2`   |
|      `1`       |      `2`      | `A1` (`A0`との差動) |   `A2`   |
|      `1`       |      `3`      | `A1` (`A0`との差動) |   `A2`   |

### ベース電圧ピン配置

| ピン名 | 対応 ADC 番号 | ピン  |
| :----: | :-----------: | :---: |
|  `VB`  |      `0`      | `A3`  |

## コマンドフォーマット

ホストから UART で特定のコマンドを定められたバイナリフォーマットで送信する．</br>
バイナリの送信受信には[PacketSerial](https://github.com/bakercp/PacketSerial/tree/master)が使用される．</br>
送受信されるパケットは`COBS`エンコードされる．以下のフォーマットはデコード後のバイナリフォーマットである．

### 送信コマンド(HOST -> ESP32)

```bash
# 0x02 <コマンド> <パラメータ(オプション)> 0x03
[STX] [CMD] [PRM] [ETX]
```

- `STX` : コマンド送信の開始を示す．コマンドの開始には必ず STX`0x02`を送信する．
- `コマンド` : コマンド一覧のコマンドを送信する．コマンド一覧を参照．
- `パラメータ` : コマンドによって送信するパラメータのサイズは異なる．
- `ETX` : コマンド送信の終了を示す．コマンドの終了には必ず ETX`0x03`を送信する．

### 応答コマンド(ESP32 -> HOST)

コマンドが正常に処理された場合，ACK 応答を経た後，応答コマンドを返す．</br>

```bash
# 0x02 <応答コマンド> <パラメータ(オプション)> 0x03
[STX] [CMD] [PRM] [ETX]
```

- `STX` : 応答コマンドの開始を示す．応答コマンドの開始には必ず STX`0x02`を返す．
- `応答コマンド` : コマンド一覧の応答コマンドを返す．
- `パラメータ` : コマンドによって返すパラメータのサイズは異なる．
- `ETX` : 応答コマンドの終了を示す．応答コマンドの終了には必ず ETX`0x03`を返す．

### ERR 応答(ESP32 -> HOST)

コマンドが正常に処理されなかった場合，エラーコマンドを返す．</br>
特定のコマンドに対応するエラーコードがある．

```bash
# 0xFE <ERRコード> 0xFD
[SER] [ERRCode] [EER]
```

- `SERTX` : エラー応答の開始を示す．エラー応答の開始には必ず SERTX`0xFE`を返す．
- `ERRコード` : エラーコードを返す．ERR コード一覧を参照．
- `EERTX` : エラー応答の終了を示す．エラー応答の終了には必ず EERTX`0xFD`を返す．

#### ERR コード一覧(共通)

| ERR コード |               エラー内容               |
| :--------: | :------------------------------------: |
|   `0x01`   |            コマンド構文不正            |
|   `0x11`   |   コマンド無効(対応するコマンドなし)   |
|   `0x21`   | パラメータ無効(対応するパラメータなし) |

### コマンド一覧

- ファームウェアバージョンの取得
- ベース電圧の取得
- センサの接続状態の確認
- センサの ME 読み取り
- センサの SME 読み取り

| コマンド | コマンド名 |          コマンド説明          |
| :------: | :--------: | :----------------------------: |
|  `0x01`  |  `GETVER`  | ファームウェアバージョンの取得 |
|  `0x02`  |  `GETBV`   |        ベース電圧の取得        |
|  `0x20`  |  `GETCON`  |     センサの接続状態の確認     |
|  `0x30`  |  `GETME`   |      センサの ME 読み取り      |
|  `0x31`  |  `GETSME`  |     センサの SME 読み取り      |
|  `0x40`  |  `STAPRM`  | ポーリングレポートモードの開始 |
|  `0x41`  |  `ENDPRM`  | ポーリングレポートモードの終了 |
|  `0x42`  |  `SETPRR`  | ポーリングレポートレートの設定 |
|  `0x43`  |  `GETPRR`  | ポーリングレポートレートの取得 |
|  `0x4F`  |  `GETPR`   |    ポーリングレポートの取得    |

#### ファームウェアバージョンの取得

デバイスのファームウェアバージョンを取得する．

- 送信コマンド

```bash
# 0x02 0x01 0x03
[STX] 0x01 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x01 <メジャーバージョン> <マイナーバージョン> <パッチバージョン> 0x03
# EX) 0x02 0x01 0x01 0x00 0x00 0x03 -> v1.0.0
[STX] 0x01 [MJV] [MIV] [PTV] [ETX]
```

| パラメータ | サイズ  |        説明        |
| :--------: | :-----: | :----------------: |
|   `MJV`    | `1byte` | メジャーバージョン |
|   `MIV`    | `1byte` | マイナーバージョン |
|   `PTV`    | `1byte` |  パッチバージョン  |

#### ベース電圧の取得

デバイスのベース電圧を取得する．

- 送信コマンド

```bash
# 0x02 0x02 0x03
[STX] 0x02 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x02 <ベース電圧> 0x03
# EX) 0x02 0x02 0xFFFF 0x03 -> 5V
 [STX] 0x02 [VB] [ETX]
```

| パラメータ | サイズ  |                                  説明                                  |
| :--------: | :-----: | :--------------------------------------------------------------------: |
|    `VB`    | `2byte` | ベース電圧.符号あり 16bit 整数.`0-5`V を`0-32767`の範囲に置き換えた値. |

- ベース電圧について

コマンドで得られた値を$`v_B`$とすると，ベース電圧$`V_B`$は次のように表される．

$$
\begin{align}
V_B = \frac{5}{32767} \times v
\end{align}
$$

#### センサの接続状態の確認

デバイスに接続されているセンサの接続状態を確認する．

- 送信コマンド

```bash
# 0x02 0x20 0x03
[STX] 0x20 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x20 <センサ0状態> <センサ1状態> <センサ2状態> <センサ3状態> 0x03
# EX) 0x02 0x20 0x01 0x00 0x00 0x00 0x03 -> センサ0: 接続, センサ1: 未接続, センサ2: 未接続, センサ3: 未接続
[STX] 0x20 [CON0] [CON1] [CON2] [CON3] [ETX]
```

|      パラメータ      | サイズ  |                             説明                              |
| :------------------: | :-----: | :-----------------------------------------------------------: |
| `CON{n}` (`n`:`0-3`) | `1byte` | センサの接続状態(`0x00`: `disconnected`, `0x01`: `connected`) |

#### センサの ME 読み取り

デバイスに接続されているセンサの ME 値を読み取る．

- 送信コマンド

```bash
# 0x02 0x30 0x03
[STX] 0x30 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x30 <ME値0> <ME値1> <ME値2> <ME値3> 0x03
# EX) 0x02 0x30 0x03 0xE8 0x03 0xE9 0x00 0x00 0x00 0x00 0x03 -> ME0: 1000, ME1: 1001, ME2: 0, ME3: 0
[STX] 0x30 [ME0] [ME1] [ME2] [ME3] [ETX]
```

|     パラメータ     | サイズ  |                                      説明                                      |
| :----------------: | :-----: | :----------------------------------------------------------------------------: |
| `ME{n}`(`n`:`0-3`) | `2byte` | ME 値. 符号あり 16bit 整数の上位 bit・下位 bit.センサが接続されていない場合`0` |

- ME 値について

筋電信号 ME を $`E`$ とすると

$$
\begin{align}
- 32768 \leq E \leq 32767 \\
\end{align}
$$

ベース電圧(V)を $`V_B`$ すると，筋電信号電圧 $`V_{E}`$ は次のように表される．

$$
\begin{align}

V_{E} = \begin{cases}
            \frac{E}{32767} \times V_B & (E \geq 0) \\
            \frac{E}{32768} \times V_B & (E < 0) \\
        \end{cases}
\\

- \frac{1}{2} V_B \leq V_{E} \leq \frac{1}{2} V_B \\

\end{align}
$$

#### センサの SME 読み取り

デバイスに接続されているセンサの SME 値を読み取る．

- 送信コマンド

```bash
# 0x02 0x31 0x03
[STX] 0x31 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x31 <SME値0> <SME値1> <SME値2> <SME値3> 0x03
# EX) 0x02 0x31 0x07 0xD0 0x07 0xD1 0x00 0x00 0x00 0x00 0x03 -> SME0: 2000, SME1: 2001, SME2: 0, SME3: 0
[STX] 0x31 [SME0] [SME1] [SME2] [SME3] [ETX]
```

|     パラメータ      | サイズ  |                                      説明                                      |
| :-----------------: | :-----: | :----------------------------------------------------------------------------: |
| `SME{n}`(`n`:`0-3`) | `2byte` | SME 値.符号あり 16bit 整数の上位 bit・下位 bit.センサが接続されていない場合`0` |

- SME 値について

整流・平滑筋電信号 SME を $`E'`$ とすると

$$
\begin{align}
0 \leq E' \leq 32767 \\
\end{align}
$$

ベース電圧(V)を $`V_B`$ すると，整流・平滑筋電信号電圧 $`V_{E'}$ は次のように表される．

$$
\begin{align}
V_{E'} = \frac{E'}{32767} \times V_B \\
0 \leq V_{E'} \leq V_B \\
\end{align}
$$

#### ポーリングレポートモードの開始

デバイスの情報一定間隔で自動的に送信するモードの開始．
[ポーリングレポートの取得](#ポーリングレポートの取得)で得られる応答コマンドを設定されたレートで送信する．

- 送信コマンド

```bash
# 0x02 0x40 0x03
[STX] 0x40 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x40 0x03
[STX] 0x40 [ETX]
```

#### ポーリングレポートモードの終了

デバイスのポーリングレポートモードを終了する．

- 送信コマンド

```bash
# 0x02 0x41 0x03
[STX] 0x41 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x41 0x03
[STX] 0x41 [ETX]
```

#### ポーリングレポートレートの設定

デバイスのポーリングレポートレートをミリ秒で設定する．

- 送信コマンド

```bash
# 0x02 0x42 <レート> 0x03
# EX) 0x02 0x42 0x00 0x64 0x03 -> 100ms
[STX] 0x42 [RATE] [ETX]
```

| パラメータ | サイズ  |                                       説明                                       |
| :--------: | :-----: | :------------------------------------------------------------------------------: |
|   `RATE`   | `2byte` | ポーリングレポートレート.符号あり 16bit 整数の上位 bit・下位 bit. Hzで設定する． |

- 応答コマンド

```bash
# 0x02 0x42 0x03
[STX] 0x42 [ETX]
```

#### ポーリングレポートレートの取得

デバイスのポーリングレポートレートを取得する．

- 送信コマンド

```bash
# 0x02 0x43 0x03
[STX] 0x43 [ETX]
```

- 応答コマンド

```bash
# 0x02 0x43 <レート> 0x03
# EX) 0x02 0x43 0x00 0x64 0x03 -> 100Hz
[STX] 0x43 [RATE] [ETX]
```

| パラメータ | サイズ  |                                 説明                                  |
| :--------: | :-----: | :-------------------------------------------------------------------: |
|   `RATE`   | `2byte` | ポーリングレポートレート(Hz).符号あり 16bit 整数の上位 bit・下位 bit. |

#### ポーリングレポートの取得

デバイスのポーリングレポートを取得する．

- 送信コマンド

```bash
# 0x02 0x4F 0x03
[STX] 0x4F [ETX]
```

- 応答コマンド

```bash
# 0x02 0x40 <ベース電圧> <ME0> <ME1> <ME2> <ME3> <SME0> <SME1> <SME2> <SME3> <タイムスタンプ> 0x03
[STX] 0x40 [VB] [ME0] [ME1] [ME2] [ME3] [SME0] [SME1] [SME2] [SME3] [TMP] [ETX]
```

|     パラメータ      | サイズ  |                                                   説明                                                    |
| :-----------------: | :-----: | :-------------------------------------------------------------------------------------------------------: |
|        `VB`         | `2byte` |                  ベース電圧.符号あり 16bit 整数.`0-5`V を`0-32767`の範囲に置き換えた値.                   |
| `ME{n}`(`n`:`0-3`)  | `2byte` |              ME 値. 符号あり 16bit 整数の上位 bit・下位 bit.センサが接続されていない場合`0`               |
| `SME{n}`(`n`:`0-3`) | `2byte` |              SME 値.符号あり 16bit 整数の上位 bit・下位 bit.センサが接続されていない場合`0`               |
|        `TMP`        | `4byte` | レポート送信時のタイムスタンプ. ESP32 が起動してからの時間(ms). 符号なし 32bit 整数を 4 分割した byte 列. |

各パラメータの詳細は該当するコマンドの説明を参照．
