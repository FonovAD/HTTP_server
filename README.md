
# Yet HTTP Server

## Для чего?
Я решил написать свой http сервер на С, чтобы:
 - потренироваться в использовании C;
 - закрепить знания об устройстве http серверов;
 - поменять в http сервере то, что мне не нравиться;
 - после перенести сервер на Go;


## Что будет уметь сервер?
 1. Обрабатывать запросы (кто бы мог подумать);
    а. Получать запрос;
    б. Разбирать путь, по которому сделан запрос и соотносить с нужной функцией;
    в. Вызывать функцию/создавать задачу для пула потоков;
    г. Отправлять ответ;
 2. Логировать запросы (у меня еще не было проектов, где не нужно было логирование. Почему бы его не сделать встроенным по-умолчанию);
 3. Встроенный механизм кеширования ответов;
 4. Встроенная база данных (под вопросом);

Так же есть следующая мысль: по моим наблюдениям, увеличение производительности приложения часто приводит к увеличению потребляемой памяти(кешерование, преаллокация), поэтому приложение это всегда баланс между потребляемой память и скоростью.

Я хочу попробовать привязать некоторую выделенную память к каждому потоку, чтобы во время выполнения программы не нужно было аллоцировать память. Но тут же появляется проблема: \
**Как узнать, сколько нужно выделить памяти?** \
    `Пример: один пользовател как коментарий оставит "Товар пришел, еще не поользовался", а другой напишеть простыню текста из 500+ символов.`
Эту проблему я собираюсь решить статистически: сначала будет запускаться сервер с макисмальным потреблением памяти(у всех потоков будет максимальный* размер памяти). По мере работы сервера буду получаться оценки показателей(сколько запросов с каким размером приходит). После чего у приложения будут "срезаться углы" - сервер будет освобождать ненужную память. Появляется еще проблема: \
**Сервер настроиться на оптимальные параметры, после чего начнется спам большими запросами. Что делать в таком случае?** \
Этот вопрос еще предстоит решить.

\* Максимальный размер - у каждого запроса есть какой-то лимит, иначе пользователь сможет занять всю память одним запросом.