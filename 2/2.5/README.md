# Задание 2.5

### Программа, бесконечно выводящая в файл 1 раз в секунду значение внутреннего счетчика (1 2 3 ...). Запускается в фоновом режиме (./main &). 

### Ниже приведены участки терминала при тестировании программы с использованием различных команд управления (SIGINT, SIGQUIT, SIGABRT и т.д.).

## kill -SIGINT <pid>: прерывает выполнение процесса (аналогично нажатию Ctrl+C в терминале).

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21476
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGINT 21476
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### vboxuser   21088  0.0  0.1 4255232 14492 ?       Sl   14:28   0:00 /home/vboxuser/.vscode/extensions/ms-vscode.cpptools-1.19.9-linux-x64/bin/cpptools-srv 20060 {A5E95684-D548-487F-905D-7C480C000CF7}
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21508  0.0  0.0  13032  3456 pts/2    R+   14:40   0:00 ps aux
### root       21509  0.0  0.0   9232  2432 pts/2    S+   14:40   0:00 tail -4
### [1]+  Прерывание    ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21510

## kill -SIGQUIT <pid>: завершает процесс и создает дамп памяти (core dump).

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGQUIT 21510
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### vboxuser   21088  0.0  0.1 4255232 14492 ?       Sl   14:28   0:00 /home/vboxuser/.vscode/extensions/ms-vscode.cpptools-1.19.9-linux-x64/bin/cpptools-srv 20060 {A5E95684-D548-487F-905D-7C480C000CF7}
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21529  0.0  0.0  13032  3328 pts/2    R+   14:40   0:00 ps aux
### root       21530  0.0  0.0   9232  2432 pts/2    S+   14:40   0:00 tail -4
### [1]+  Выход              (образ памяти сброшен на диск) ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21531

## kill -SIGABRT <pid>: аварийно завершает процесс c кодом ошибки.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGABRT 21531
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### vboxuser   21088  0.0  0.1 4255232 14492 ?       Sl   14:28   0:00 /home/vboxuser/.vscode/extensions/ms-vscode.cpptools-1.19.9-linux-x64/bin/cpptools-srv 20060 {A5E95684-D548-487F-905D-7C480C000CF7}
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21553  0.0  0.0  13032  3456 pts/2    R+   14:40   0:00 ps aux
### root       21554  0.0  0.0   9232  2432 pts/2    S+   14:40   0:00 tail -4
### [1]+  Аварийный останов         (образ памяти сброшен на диск) ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21555

## kill -SIGKILL <pid>: немедленно убивает процесс.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGKILL 21555
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### vboxuser   21088  0.0  0.1 4255232 14492 ?       Sl   14:28   0:00 /home/vboxuser/.vscode/extensions/ms-vscode.cpptools-1.19.9-linux-x64/bin/cpptools-srv 20060 {A5E95684-D548-487F-905D-7C480C000CF7}
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21570  0.0  0.0  13032  3456 pts/2    R+   14:41   0:00 ps aux
### root       21571  0.0  0.0   9232  2432 pts/2    S+   14:41   0:00 tail -4
### [1]+  Убито              ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21572

## kill -SIGTERM <pid>: завершает процесс по умолчанию.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGTERM 21572
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### vboxuser   21088  0.0  0.1 4255232 14492 ?       Sl   14:28   0:00 /home/vboxuser/.vscode/extensions/ms-vscode.cpptools-1 19.9-linux-x64/bin/cpptools-srv 20060 {A5E95684-D548-487F-905D-7C480C000CF7}
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21586  0.0  0.0  13032  3328 pts/2    R+   14:41   0:00 ps aux
### root       21587  0.0  0.0   9232  2432 pts/2    R+   14:41   0:00 tail -4
### [1]+  Завершено      ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [1] 21588

## kill -SIGTSTP <pid>: приостанавливает выполнение процесса.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGTSTP 21588
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21588  0.0  0.0   2644  1408 pts/2    T    14:41   0:00 ./main
### root       21605  0.0  0.0  13032  3328 pts/2    R+   14:41   0:00 ps aux
### root       21606  0.0  0.0   9232  2432 pts/2    S+   14:41   0:00 tail -4
### 
### [1]+  Остановлен    ./main

## kill -SIGSTOP <pid>: приостанавливает выполнение процесса без возможности возобновления.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ./main &
### [2] 21609
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGSTOP 21609
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21609  0.0  0.0   2644  1408 pts/2    T    14:42   0:00 ./main
### root       21625  0.0  0.0  13032  3456 pts/2    R+   14:42   0:00 ps aux
### root       21626  0.0  0.0   9232  2432 pts/2    R+   14:42   0:00 tail -4
### 
### [2]+  Остановлен    ./main

## kill -SIGCONT <pid>: возобновляет выполнение приостановленного процесса.

### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGTSTP 21627
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21627  0.0  0.0   2644  1408 pts/2    T    14:42   0:00 ./main
### root       21684  0.0  0.0  13032  3456 pts/2    R+   14:43   0:00 ps aux
### root       21685  0.0  0.0   9232  2432 pts/2    R+   14:43   0:00 tail -4
### 
### [3]+  Остановлен    ./main
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# kill -SIGCONT 21627
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# ps aux | tail -4
### root       21285  0.0  0.0      0     0 ?        I    14:33   0:00 [kworker/1:1]
### root       21627  0.0  0.0   2644  1408 pts/2    S    14:42   0:00 ./main
### root       21687  0.0  0.0  13032  3456 pts/2    R+   14:43   0:00 ps aux
### root       21688  0.0  0.0   9232  2304 pts/2    S+   14:43   0:00 tail -4
### root@Pink404:/home/vboxuser/eltmod2/3/2/2.5# 