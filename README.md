# f767zi_lcd-timer
## 概要
このプロジェクトは、f767ziボードでlcdディスプレイに3分タイマーを表示するものとなっております。使用するlcdディスプレイは、Newhaven Display社のNHD-0216K3Z-NSW-BBW-V3です。

## 制御について
### 通信
このプロジェクトでは、lcdディスプレイに、uart5のpc12を使用して、txのみのuart通信で表示内容を送信します。このlcdディスプレイ自体は、spi通信、i2c通信にも対応しております。仕様書は以下の通りになります。<br>
[NHD-0216K3Z-NSW-BBW-V3の仕様書はこちら](https://newhavendisplay.com/content/specs/NHD-0216K3Z-NSW-BBW-V3.pdf)<br>
ちなみに使用するlcdディスプレイのピンは、vdd, vss, rxとなっております。

### 別branch (feature/cpp-userlibs) について
main branchには、制御プログラムが自作ライブラリも合わせてCoreフォルダの中で完結するようになっております。使用言語はcです。対して、別branch (feature/cpp-userlibs) は、ライブラリをUserLibsに入れております。Core/Inc/lcd-timer.hでfreertos.cから呼び出す関数を定義しており、Core/Src/lcd-timer.cppでc++によるタイマー表示クラスを定義しております。

### cubemxの設定
- Pinout Viewでpc12がUART5_TXに設定されていることを確認（これはデフォルトで設定されているはずです）<br>
  もし必要であれば、pd2をUART5_RXに設定
- ConnectivityでUART5のModeをAsynchronousに設定
- Parameter SettingsでBaud rateを9600bpsに変更
- System CoreでGPIOのUARTからPC12のGPIO modeをAlternate Function Open Drainに設定<br>
（簡単にいうとハイインピーダンスになる設定で、この設定の理由は後述します）<br>
  もし必要であれば、pd2も有効化しておく
- Middleware and Software PacksのFREERTOS選んで、Task and QueuesでTasksでAddして、NameをLCDTimerTask、PriorityをNormal、Stack sizeを512、Entry functionをStartLCDTImerTaskに設定

### Open Drainに設定した理由
まず、このlcdディスプレイは、5V駆動です。5VをVddに入れるとすると、仕様上HIGH入力電圧が最低でもVdd * 0.7V、つまり3.5V必要になります。しかし、f767ziが送信出力できるピンの電圧は3.3Vであるので、制御信号を送れたとしても動作が不安定になってしまいます。そこでtxピンのpush pull設定をopen drain設定に変更することで、信号を送る際に出力がハイインピーダンスという状態になります。その状態でVddから5Vをとってあげることで、きちんと仕様に合わせた電圧で制御信号を送信することが可能になるというわけです。
